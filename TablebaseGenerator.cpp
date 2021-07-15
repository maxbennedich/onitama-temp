// Endgame tablebase ("TB") generator for the board game Onitama.
//
// This implementation generates the TB by using (forward) moves for player 1 (P1) and unmoves for P0. This is a more
// memory efficient way than going through "out-counts", queue sets or P1 unmove caches, and at least for Win/Draw/Loss
// (WDL) data it may be more efficient than the "out-counts" approach. For computing a WDL table, the memory usage is
// only 2 bits per state.
//
// This implementation interleaves P1 moves and P0 unmoves, but by storing the current-iteration P0 wins with a special
// value in the win/draw/loss (WDL) table, we are able to properly construct the table one ply at the time. For all
// even plies >= STORE_DTM_AFTER_PLIES, we store all the wins to disk. See DtmStorage.h for details on that. This
// allows us to find the optimal line of play (PV; principal variation) for any position within a few minutes. For 10
// pieces and STORE_DTM_AFTER_PLIES = 30, this data uses around 6 GiB storage.
//
// On a 64 thread Threadripper 3970X, the 8 piece TB is generated in ~5 minutes at ~12 GiB memory, and the 10 piece TB
// in 5 hours at ~240 GiB memory.
//
// Among several compilers tested, `clang` with profile-guided optimization yielded the best performance.

#include <array>
#include <chrono>
#include <cinttypes> // PRIu64
#include <cstring> // memset
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <thread>
#include <vector>

// To profile on Mac:
// - include the below header
// - use linker flags: -L/usr/local/Cellar/gperftools/2.9.1/lib -lprofiler
// - to start profiling: ProfilerStart("tbfwd.prof");
// - to stop: ProfilerStop();
// #include </usr/local/include/gperftools/profiler.h>

#include "Card.h"
#include "Common.h"

// Only compact indexing is supported here
#if USE_COMPACT_INDEXER || INCLUDE_IMBALANCED_STATES
    #include "CompactIndexer.h"
#endif

// For better CPU utilization, we use a lot more threads than available cores. For a 12-thread machine, 180 threads
// and 36 merge threads seemed efficient. For a 64-thread machine, 384 threads and 64 merge threads were used.
#define NUM_THREADS 180LLU
#define NUM_MERGE_THREADS 36LLU

U64 NUM_ARRAY_UPDATE_THREADS = std::max(1U, std::thread::hardware_concurrency()) / 2;

const bool STORE_DTM_DATA = true;
const U64 STORE_DTM_AFTER_PLIES = 30;

// Maps internally used card ids (0 - 4) to the ids in Card.h, for each of the 30 card combinations.
U64 cards[NUM_CARD_COMBINATIONS][NUM_CARDS_PER_GAME];

// The WDL table assumes that P0 is to move. It stores 2 bits per state, with 0 = draw, 1 = P0 win, 2 = P1 win, and
// 3 = current-iteration P0 win. As a minor optimization, the lower bit for each state is stored in the 32 lower bits
// of the U64, and similar for the upper bits. I.e. value 3 would be encoded as 0x100000001 shifted left by 0-31.
// (card index / board index order improves cache locality)
std::array<std::array<std::atomic<U64>, WDL_SIZE>, NUM_CARD_COMBINATIONS>* wdl = new std::array<std::array<std::atomic<U64>, WDL_SIZE>, NUM_CARD_COMBINATIONS>;

#include "WinInOne.h" // a bit of a hack -- needs `wdl` and `cards`
#include "DtmStorage.h"

U64 depthPlies;

void unmoveP0(U64 ibbp0, U64 ibbp1, U64 ibbk0, U64 ibbk1, U64 icardIndex) {
    U64 allPieces = ibbp0 | ibbp1;
    U64* moveBitmasks = MOVE_BITMASK_PCP[PLAYER_1][cards[icardIndex][NEXT_CARD]];
    U64* nextCards = NEXT_CARD_INDEX[icardIndex][PLAYER_0];
    U64 bbp1 = ibbp1;

    // try unmoving each piece
    for (U64 playerBitmask = ibbp0; playerBitmask != 0; playerBitmask &= playerBitmask-1) {
        U64 pos = __builtin_ia32_tzcnt_u64(playerBitmask);
        U64 curPosMask = 1ULL << pos;
        bool unmovingKing = ibbk0 == curPosMask;

        // iterate over all unmoves that could have led to this state
        U64 moveBitmask = moveBitmasks[pos] & ~allPieces; // avoid other pieces
        if (unmovingKing) moveBitmask &= PLAYER_0_WIN_MASK; // don't unmove king to win position since that state should already have been added
        for (; moveBitmask != 0; moveBitmask &= moveBitmask-1) {
            U64 np = __builtin_ia32_tzcnt_u64(moveBitmask);
            U64 prevPosMask = 1ULL << np;

            // move piece without uncapture
            U64 bbp0 = (ibbp0 & ~curPosMask) | prevPosMask;
            U64 bbk0 = unmovingKing ? prevPosMask : ibbk0;
            U64 prevStateIndex = indexForState<false>(bbp0, ibbp1, bbk0, ibbk1);
            for (U64 move = 0; move < NUM_CARDS_PER_HAND; ++move) {
                U64 cardIndex = nextCards[move];
                U64 curWdl = (*wdl)[cardIndex][prevStateIndex>>5];
                if (!((curWdl >> (prevStateIndex&31)) & 1))
                    while (!(*wdl)[cardIndex][prevStateIndex>>5].compare_exchange_weak(curWdl, curWdl | 0x100000001ULL << (prevStateIndex & 31)));
            }
        }
    }

    bool uncaptureAllowed = INCLUDE_IMBALANCED_STATES
        ? __builtin_popcountll(ibbp0) + __builtin_popcountll(ibbp1) < TB_PIECES && __builtin_popcountll(ibbp1) < 5
        : __builtin_popcountll(ibbp1) < TB_PIECES/2;
    if (uncaptureAllowed) {
        // try unmoving each piece
        for (U64 playerBitmask = ibbp0; playerBitmask != 0; playerBitmask &= playerBitmask-1) {
            U64 pos = __builtin_ia32_tzcnt_u64(playerBitmask);
            U64 curPosMask = 1ULL << pos;
            bool unmovingKing = ibbk0 == curPosMask;
            bbp1 = ibbp1 | curPosMask;

            // iterate over all unmoves that could have led to this state
            U64 moveBitmask = moveBitmasks[pos] & ~allPieces; // avoid other pieces
            if (unmovingKing) moveBitmask &= PLAYER_0_WIN_MASK; // don't unmove king to win position since that state should already have been added
            for (; moveBitmask != 0; moveBitmask &= moveBitmask-1) {
                U64 np = __builtin_ia32_tzcnt_u64(moveBitmask);
                U64 prevPosMask = 1ULL << np;

                // move piece with uncapture
                U64 bbp0 = (ibbp0 & ~curPosMask) | prevPosMask;
                U64 bbk0 = unmovingKing ? prevPosMask : ibbk0;
                U64 prevStateIndex = indexForState<false>(bbp0, ibbp1 | curPosMask, bbk0, ibbk1);
                for (U64 move = 0; move < NUM_CARDS_PER_HAND; ++move) {
                    U64 cardIndex = nextCards[move];
                    U64 curWdl = (*wdl)[cardIndex][prevStateIndex>>5];
                    if (!((curWdl >> (prevStateIndex&31)) & 1))
                        while (!(*wdl)[cardIndex][prevStateIndex>>5].compare_exchange_weak(curWdl, curWdl | 0x100000001ULL << (prevStateIndex & 31)));
                }
            }
        }
    }
}

// Iterate over all unresolved states to find P1 wins. Note that this approach was slightly faster than the "for loop"
// method used in WinInOne.h that avoids the `stateForIndex` calls.
// Return value: [number of wins / elapsed time in seconds to store DTM data to disk; or -1 if not applicable]
template <bool storeDtmToDisk>
std::tuple<U64, double> moveP1() {
    U64 numWins[NUM_THREADS];
    memset(numWins, 0, sizeof(numWins));

    // TODO: Vector is both a bit slow, and might use excessive memory. Perhaps a deque is a better choice.
    // Though this will likely OOM anyway in case of a large number of depth-30 states. Perhaps we can delta
    // encode this data in 32 bits or even variable number of bits to get around that. Or flush it to disk
    // if it becomes too big.
    std::array<std::vector<U64>, NUM_THREADS> newlyWonIdxs;

    auto moveLambda = [&numWins, &newlyWonIdxs](U64 threadId) {
        U64* threadNumWins = numWins + threadId;

        U64 ci1 = threadId * NUM_CARD_COMBINATIONS * WDL_SIZE / NUM_THREADS;
        U64 ci2 = (threadId+1) * NUM_CARD_COMBINATIONS * WDL_SIZE / NUM_THREADS;

        U64 c1 = ci1 / WDL_SIZE;
        U64 i1 = ci1 % WDL_SIZE;
        U64 c2 = (ci2-1) / WDL_SIZE;
        U64 i2 = ((ci2-1) % WDL_SIZE) + 1;

        std::vector<U64>* threadNewlyWonIdx = &newlyWonIdxs[threadId];

        for (U64 cardIndex = c1; cardIndex <= c2; ++cardIndex) {
            U64 invCardIndex = cardIndex+5-2*(cardIndex%6);
            for (U64 wIdx = (cardIndex == c1 ? i1 : 0), wEnd = (cardIndex == c2 ? i2 : WDL_SIZE); wIdx < wEnd; ++wIdx) {
                U64 newP1Wins = 0;
                U64 bits = (*wdl)[cardIndex][wIdx];
                // loop over unresolved states
                for (bits = (~(bits | (bits >> 32))) & 0xffffffff; bits != 0; bits &= bits-1) {
                    U64 entryIdx = __builtin_ia32_tzcnt_u64(bits);
                    stateForIndex<true>((wIdx << 5) + entryIdx, invCardIndex, [entryIdx, &newP1Wins](U64 ibbp0, U64 ibbp1, U64 ibbk0, U64 ibbk1, U64 icardIndex) {
                        U64* p1Cards = &cards[icardIndex][PLAYER_1_CARDS];
                        // try moving each player 1 piece
                        for (U64 playerBitmask = ibbp1; playerBitmask != 0; playerBitmask &= playerBitmask-1) {
                            U64 curPos = __builtin_ia32_tzcnt_u64(playerBitmask);
                            U64 curPosMask = 1ULL << curPos;
                            for (U64 move = 0; move < NUM_CARDS_PER_HAND; ++move) {
                                std::atomic<U64>* successorWdl = &(*wdl)[NEXT_CARD_INDEX[icardIndex][PLAYER_1][move]][0];

                                // iterate over all moves (avoiding other P1 pieces)
                                for (U64 moveBitmask = MOVE_BITMASK_PCP[PLAYER_1][p1Cards[move]][curPos] & ~ibbp1; moveBitmask != 0; moveBitmask &= moveBitmask-1) {
                                    U64 sp = __builtin_ia32_tzcnt_u64(moveBitmask);
                                    U64 nextPosMask = 1ULL << sp;

                                    // try to remove opponent piece; Note: no need to try to remove the king or check for won position
                                    // since all won states have already been resolved
                                    U64 successorBbP0 = ibbp0 & ~nextPosMask;

                                    // move piece
                                    U64 successorBbP1 = (ibbp1 & ~curPosMask) | nextPosMask;
                                    U64 successorBbK1 = ibbk1 == curPosMask ? nextPosMask : ibbk1;

                                    // for P1 moves, we need *all* successors to be wins; if we find any non-win, break
                                    U64 successorIndex = indexForState<false>(successorBbP0, successorBbP1, ibbk0, successorBbK1);
                                    if (((successorWdl[successorIndex>>5] >> (successorIndex&31)) & 0x100000001ULL) != 1)
                                        return;
                                }
                            }
                        }
                        // If we reach here, all successors are P0 wins, and we have a "P0 win with P1 to move". Since the WDL
                        // table assumes that P0 is to move, we invert the state so that it becomes "P1 wins with P0 to move".
                        newP1Wins |= 1ULL << entryIdx;
                        unmoveP0(ibbp0, ibbp1, ibbk0, ibbk1, icardIndex); // set all predecessors to P0 wins
                    });
                }
                if (newP1Wins) {
                    *threadNumWins += __builtin_popcountll(newP1Wins);
                    for (U64 curWdl = (*wdl)[cardIndex][wIdx]; !(*wdl)[cardIndex][wIdx].compare_exchange_weak(curWdl, curWdl | (newP1Wins << 32)); );
                    if (storeDtmToDisk) {
                        for (bits = newP1Wins; bits != 0; bits &= bits-1) {
                            // Swap the card index / table index "dimensions" of the index (leads to better compression)
                            U64 tableIndex = (wIdx << 5) + __builtin_ia32_tzcnt_u64(bits);
                            threadNewlyWonIdx->push_back(cardIndex + tableIndex * NUM_CARD_COMBINATIONS);
                        }
                    }
                }
            }
        }
    };

    std::vector<std::thread> threads;
    for (U64 t = 0; t < NUM_THREADS; ++t)
        threads.push_back(std::thread(moveLambda, t));
    for (auto& thread : threads)
        thread.join();

    U64 totalNumWins = 0;
    for (U64 t = 0; t < NUM_THREADS; ++t)
        totalNumWins += numWins[t];

    double storeDtmToDiskTimeInSeconds = -1;
    if (storeDtmToDisk) {
        auto startTime = std::chrono::high_resolution_clock::now();
        mergeAndCompressFixedRangeChunks("dtm", depthPlies, newlyWonIdxs);
        storeDtmToDiskTimeInSeconds = std::chrono::duration<double, std::ratio<1>>(std::chrono::high_resolution_clock::now() - startTime).count();
    }

    return {totalNumWins, storeDtmToDiskTimeInSeconds};
}

// Replace "current iteration P0 win" (0x100000001) with just "P0 win" (0x1)
void cleanUpP0Wins() {
    U64* w = (U64*)(&(*wdl)[0][0]);

    auto lambda = [w](U64 threadId) {
        U64 i1 = threadId * NUM_CARD_COMBINATIONS * WDL_SIZE / NUM_ARRAY_UPDATE_THREADS;
        U64 i2 = (threadId+1) * NUM_CARD_COMBINATIONS * WDL_SIZE / NUM_ARRAY_UPDATE_THREADS;

        for (U64 i = i1; i < i2; ++i) {
            U64 bits = w[i];
            w[i] = bits ^ (bits & (bits << 32));
        }
    };

    std::vector<std::thread> threads;
    for (U64 t = 0; t < NUM_ARRAY_UPDATE_THREADS; ++t)
        threads.push_back(std::thread(lambda, t));
    for (auto& thread : threads)
        thread.join();
}

void printWdlStats() {
    U64* w = (U64*)(&(*wdl)[0][0]);
    U64 hash = 5381, wins = 0;

    for (U64 i = 0; i < NUM_CARD_COMBINATIONS * WDL_SIZE; ++i) {
        hash = (hash*33) ^ w[i];
        wins += __builtin_popcountll(w[i]);
    }

    printf("Total %" PRIu64 " wins (hash %" PRIx64 ")\n", wins, hash);
}

void parseArgs(int argc, char** argv) {
    if (argc == 2) {
        int seconds = atoi(argv[1]);
        printf("Profiling mode: Program will exit in %d seconds\n", seconds);
        std::thread([=]() {
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
            // ProfilerStop();
            exit(0);
        }).detach();
    }
}

int main(int argc, char** argv) {
    initializeIndexer();

    parseArgs(argc, argv);

    // The 5 cards we're building the TB for.
    U64 fiveCards[] = {BOAR, OX, ELEPHANT, HORSE, CRAB}; // "perft" cards

    std::string cardsStr = "";
    for (U64 c = 0; c < 5; ++c) cardsStr += (c ? ", " : "") + CARD_NAMES[fiveCards[c]];
    printf("%d pieces | %s\n%" PRIu64 " retrograde threads | %" PRIu64 " compression threads | %" PRIu64 " array update threads\n",
        TB_PIECES, cardsStr.c_str(), NUM_THREADS, NUM_MERGE_THREADS, NUM_ARRAY_UPDATE_THREADS);
    if (STORE_DTM_DATA)
        printf("Storing all wins for depths >= %" PRIu64 "\n", STORE_DTM_AFTER_PLIES);

    memset(wdl, 0, sizeof(*wdl));

    // Set the invalid entries at the end of the WDL table as resolved (P0 wins, arbitrarily),
    // so that we don't need special code to handle them in the move functions.
    U64 wdlLastEntryBitmask = (0xffffffff << (32 - (WDL_SIZE << 5) + TABLE_SIZE)) & 0xffffffff;
    for (U64 cardIndex = 0; cardIndex < NUM_CARD_COMBINATIONS; ++cardIndex)
        (*wdl)[cardIndex][WDL_SIZE-1] = wdlLastEntryBitmask;

    auto startTime = std::chrono::high_resolution_clock::now();
    // ProfilerStart("tbfwd.prof");

    for (U64 a = 0; a < NUM_CARD_COMBINATIONS; ++a)
        for (U64 b = 0; b < NUM_CARDS_PER_GAME; ++b)
            cards[a][b] = fiveCards[CARD_COMBINATIONS[a][b]];

    U64 numWins = 1;
    for (U64 iter = 0; numWins; ++iter) {
        auto startTime = std::chrono::high_resolution_clock::now();
        double storeDtmToDiskTimeInSeconds = -1;

        // TODO: queue approach instead of iterating over all P1 moves after some threshold
        if (iter == 0) {
            generateWinInOne();
            printf("Depth 1");
        } else {
            if (iter * 2 < STORE_DTM_AFTER_PLIES || !STORE_DTM_DATA) {
                std::tie(numWins, storeDtmToDiskTimeInSeconds) = moveP1<false>();
            } else {
                depthPlies = iter * 2;
                std::tie(numWins, storeDtmToDiskTimeInSeconds) = moveP1<true>();
            }
            cleanUpP0Wins();
            // TODO: if no new P0 wins, we can skip the next P1 iteration
            printf("Depth %llu: %llu wins", iter * 2, numWins);
        }

        double plyTimeInSeconds = std::chrono::duration<double, std::ratio<1>>(std::chrono::high_resolution_clock::now() - startTime).count();
        printf(" in %.3f s", plyTimeInSeconds);
        if (storeDtmToDiskTimeInSeconds >= 0)
            printf(" (wrote wins in %.3f s)", storeDtmToDiskTimeInSeconds);
        printf("\n");
    }

    // ProfilerStop();
    printf("Elapsed time: %.3f s\n", std::chrono::duration<double, std::ratio<1>>(std::chrono::high_resolution_clock::now() - startTime).count());

    // Clear the invalid entries at the end of the WDL table.
    for (U64 cardIndex = 0; cardIndex < NUM_CARD_COMBINATIONS; ++cardIndex)
        (*wdl)[cardIndex][WDL_SIZE-1] &= ~wdlLastEntryBitmask;

    printWdlStats();

    delete wdl;
}
