// This file contains code to sort partially sorted indexes, split the sorted data into chunks, delta encode it, and
// LZMA compress it to disk. The purpose of this is to store all indexes that are either draws or even-plied wins
// beyond a certain depth, such as >= 30 plies (storing all wins would use too much space). With this data, we are able
// to determine the DTM for any state, either directly or through an exhaustive alpha-beta search, which should take
// at most a few minutes. See VerifyCompressedDtmData for some more details and an example of consuming this data.
//
// By storing the data in chunks of pre-determined ranges, we can locate the chunk for a given index, and would only
// need to decode that chunk in order to search for the index.
//
// The reason that we need to sort the indexes, when we iterate over the table in order anyway during the retrograde
// analysis, is that we are re-ordering the indexes from [cardIndex][tableIndex] to [tableIndex][cardIndex]. The former
// leads to much better cache efficiency during retrograde analysis, while the latter leads to much better compression.
//
// Several compression formats were tested, and LZMA offered superior compression, and can be configured such that the
// compression time is negligble compared to the retrograde analysis time. The total compressed size of all wins for
// even plies >= 30 for all 2348 card sets is estimated at ~15 TB.

#include <array>
#include <stdio.h>
#include <string>
#include <thread>
#include <vector>

#include "lzma/7zFile.h"
#include "lzma/Alloc.h"
#include "lzma/LzmaDec.h"
#include "lzma/LzmaEnc.h"

void lzmaCompress(std::string fileName, U8* data, U64 dataSizeInBytes) {
    CFileOutStream outStream;
    FileOutStream_CreateVTable(&outStream);
    File_Construct(&outStream.file);
    outStream.wres = 0;

    WRes wres = OutFile_Open(&outStream.file, fileName.c_str());
    if (wres != 0) { printf("Failed to open output file '%s'\n", fileName.c_str()); exit(1); }

    CLzmaEncHandle enc = LzmaEnc_Create(&g_Alloc);
    if (enc == 0) { printf("Failed to allocate memory for LZMA compression\n"); exit(1); }

    CLzmaEncProps props;
    LzmaEncProps_Init(&props);
    props.dictSize = 1 << 12; // small dictionary for faster encoding
    props.algo = 0; // fast mode
    props.btMode = 0; // hashChain mode (this leads to much faster compression; should already be set since "algo = 0", but let's be explicit)
    props.lp = 2; // period = 32-bit data
    props.lc = 0; // recommended if specifying `lp`
    SRes res = LzmaEnc_SetProps(enc, &props);
    if (res != SZ_OK) { printf("Failed to set LZMA properties\n"); exit(1); }
    LzmaEncodeToOutStream(&outStream.vt, data, dataSizeInBytes, &props, NULL, &g_Alloc, &g_Alloc);

    File_Close(&outStream.file);
}

// Sort a list containing two individually sorted, non-overlapping lists.
// Example [62, 66, 67, 4, 8] => [4, 8, 62, 66, 67]
// In-place, linear time.
void sortTwoLists(std::vector<U64>& v) {
    // find split index
    U64 end = v.size();
    U64 split = 1;
    for (; ; ++split) {
        if (split >= end) return; // list already in order
        if (v[split] < v[split-1]) break;
    }

    for (U64 pos = 0; pos < end-1 && split < end; ) {
        U64 len = std::min(split - pos, end - split);

        for (U64 i = 0; i < len; ++i, ++pos) {
            U64 t = v[pos];
            v[pos] = v[split + i];
            v[split + i] = t;
        }

        if (pos == split)
            split += len;
    }
}

struct HeapNode {
    U64 value;
    U64 listIdx;
    U64 elemIdx;
    U64 endElemIdx;
};

// 4-heap; slightly faster than binary in tests
void minHeapify(HeapNode* heap, U64 size, U64 i) {
    U64 c0 = 4*i+1, c1 = 4*i+2, c2 = 4*i+3, c3 = 4*i+4;
    U64 min = i;
    if (c0 < size && heap[c0].value < heap[min].value) min = c0;
    if (c1 < size && heap[c1].value < heap[min].value) min = c1;
    if (c2 < size && heap[c2].value < heap[min].value) min = c2;
    if (c3 < size && heap[c3].value < heap[min].value) min = c3;
    if (min != i) {
        HeapNode tmp = heap[i]; heap[i] = heap[min]; heap[min] = tmp; // swap `i` and `min`
        minHeapify(heap, size, min);
    }
}

// Merge-sorts the partially sorted input arrays, and splits the sorted data into NUM_MERGE_THREADS chunks using a
// fixed, pre-determined index range. Each chunk will then be delta encoded, LZMA compressed, and written to disk.
// Note that the number of elements in each chunk will differ a lot, so using more threads than available cores may
// be beneficial.
//
// The merge implementation mostly follows the "Parallel multiway merge sort" described here:
// https://en.wikipedia.org/wiki/Merge_sort#Parallel_multiway_merge_sort
// One difference is that we don't need the multi-sequence select step, since we're using fixed range chunks,
// so that step just becomes a binary search (see step 2 below).
void mergeAndCompressFixedRangeChunks(std::string path, U64 depthPlies, std::array<std::vector<U64>, NUM_THREADS>& elems) {
    // Step 1: Sort partially sorted chunks.
    // Time: O(n / p) - for n total elements, p cores
    // Memory: O(1) - in place operation
    std::vector<std::thread> threads1;
    for (U64 t = 0; t < NUM_THREADS; ++t)
        threads1.push_back(std::thread([&elems](U64 t) {
            U64 n0 = NUM_CARD_COMBINATIONS * TABLE_SIZE * t / NUM_THREADS;
            U64 n1 = NUM_CARD_COMBINATIONS * TABLE_SIZE * (t + 1) / NUM_THREADS;
            U64 c0 = n0 / TABLE_SIZE;
            U64 c1 = (n1 - 1) / TABLE_SIZE;

            // if the chunk spans two card sets, their order will be incorrect, so we need to reshuffle them
            if (c0 != c1)
                sortTwoLists(elems[t]);
        }, t));
    for (auto& thread : threads1)
        thread.join();

    // Step 2: Find intra-chunk split points.
    // Time: O(t1 * t2 * log(n / t1)) - for t1 chunks, t2 merge threads, n total elements
    // Memory: O(t1 * t2)
    // In other words, this is quite cheap, so don't bother parallelizing it.
    std::array<std::array<U64, NUM_THREADS>, NUM_MERGE_THREADS-1> splits;
    for (U64 mt = 0; mt < NUM_MERGE_THREADS-1; ++mt) {
        // note: any element with the pivot value will be included in the higher chunk
        U64 pivot = NUM_CARD_COMBINATIONS * TABLE_SIZE * (mt + 1) / NUM_MERGE_THREADS;
        for (U64 t = 0; t < NUM_THREADS; ++t)
            splits[mt][t] = std::lower_bound(elems[t].begin(), elems[t].end(), pivot) - elems[t].begin();
    }

    // Step 3: K-way merge each individual chunk, delta encode, and LZMA compress it to disk.
    // Time: O(n / p * log(t1)) - for t1 chunks, n total elements, p cores
    // Memory: O(n) - elements are stored in new chunked arrays
    // We use a custom 4-heap implementation. That was slightly faster than a binary heap in tests, and a lot faster
    // than std::priority_queue. A tournament tree had virtually identical performance, but involved a bit more code.
    // (The runtime will be dominated by the LZMA compression anyway.)
    std::vector<std::thread> threads2;
    for (U64 mt = 0; mt < NUM_MERGE_THREADS; ++mt) {
        U64 chunkSize = 0;
        for (U64 t = 0; t < NUM_THREADS; ++t) {
            U64 splitStart = mt ? splits[mt-1][t] : 0;
            U64 splitEnd = mt == NUM_MERGE_THREADS - 1 ? elems[t].size() : splits[mt][t];
            chunkSize += splitEnd - splitStart;
        }
        if (!chunkSize) continue; // ignore empty chunks

        threads2.push_back(std::thread([&elems, &splits, &path, depthPlies, chunkSize](U64 mt) {
            HeapNode* heap = new HeapNode[NUM_THREADS];
            for (U64 t = 0; t < NUM_THREADS; ++t) {
                U64 splitStart = mt ? splits[mt-1][t] : 0;
                U64 splitEnd = mt == NUM_MERGE_THREADS - 1 ? elems[t].size() : splits[mt][t];
                if (splitStart < splitEnd)
                    heap[t] = {elems[t][splitStart], t, splitStart + 1, splitEnd};
                else
                    heap[t] = {U64(-1), 0, 0, 0}; // for an empty array, insert a dummy "infinite" value, that will never be chosen
            }

            // Construct initial heap (linear time approach)
            for (S64 i = (NUM_THREADS - 1)/2; i >= 0; --i)
                minHeapify(heap, NUM_THREADS, i);

            // For faster and better compression, we delta encode the indexes and store the deltas as 32 bit
            // values. Depending on the data and thread count, there may be deltas >= 2^32. This is handled by
            // storing three 32 bit values: the special value (2^32-1), followed by the 64 bit delta.
            const U64 largestDeltaValue = (1ULL << 24) - 2; // TODO: for testing; this should be (1ULL << 32) - 2
            U64 chunkStart = NUM_CARD_COMBINATIONS * TABLE_SIZE * mt / NUM_MERGE_THREADS;
            U64 chunkEndExclusive = NUM_CARD_COMBINATIONS * TABLE_SIZE * (mt + 1) / NUM_MERGE_THREADS;
            U64 upperBoundNumOverflowEntries = (chunkEndExclusive - chunkStart) / largestDeltaValue;
            U64 upperBoundDeltaArraySize = chunkSize + upperBoundNumOverflowEntries * 2;

            // Repeatedly pick the minimum element from the heap, delta encode it, and advance in the element's array
            U32* deltas = new U32[upperBoundDeltaArraySize];
            U64 deltaIdx = 0;
            U64 prevWonIdx = chunkStart - 1;
            for (U64 it = 0; it < chunkSize; ++it) {
                U64 delta = heap[0].value - prevWonIdx - 1;
                prevWonIdx = heap[0].value;
                if (delta <= largestDeltaValue) {
                    deltas[deltaIdx++] = delta;
                } else {
                    // delta doesn't fit in 32 bits: store as three entries
                    deltas[deltaIdx++] = largestDeltaValue + 1;
                    deltas[deltaIdx++] = U32(delta);
                    deltas[deltaIdx++] = U32(delta >> 32);
                }

                // Advance in the per-thread array, if any elements are remaining (otherwise, put an "infinite" value that will never be chosen)
                heap[0].value = heap[0].elemIdx < heap[0].endElemIdx ? elems[heap[0].listIdx][heap[0].elemIdx++] : U64(-1);

                minHeapify(heap, NUM_THREADS, 0);
            }

            // LZMA compress
            std::string fileName = format("%s/%llu-%llu.lzma", path.c_str(), depthPlies, mt);
            lzmaCompress(fileName, (U8*)deltas, deltaIdx * sizeof(deltas[0]));

            delete[] deltas;
        }, mt));
    }
    for (auto& thread : threads2)
        thread.join();
}
