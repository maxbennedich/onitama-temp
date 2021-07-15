// Exposes a method to generate all win-in-one states, i.e. all states where P0 can win in a single move, by either
// moving its king to the winning position, or capturing P1's king. The implementation loops over every possible state,
// in the same order they're defined in CompactIndexer, and checks whether it's a win-in-one. This is considerably
// faster than the "backwards" approach of generating all wins and then marking all predecessors as win-in-ones -- for
// 8 pcs, it's over 40x faster on a 32-core Threadripper (<1 second vs 41 seconds). Since a large number of states
// are win-in-ones (6pcs: ~50%, 8pcs: ~57%, 10 pcs: ~64%), the fact that we're iterating over every possible state
// is not so wasteful.
//
// This implementation will spawn up to 250 threads; one per board position (for the P0 king position) and unique
// P0 card tuple (usually 10, though may be fewer depending on the cards).
//
// As a bit of a hack, this implementation relies on `wdl` and `cards` being defined globally. A cleaner
// implementation that encapsulated the logic in a class was ~5% slower, so we're stuck with this ugliness for now.

#include <thread>
#include <unordered_map>

#include "Card.h"
#include "Common.h"
#include "CompactIndexer.h"

inline void checkWinInOne(U64 cardIndex, U64& tbIdx, U64 bbp0, U64 bbk0, U64 p1k, U64 preWonPositions) {
    // The compiler seems able to reuse the variable below, so it's fine to put it in the inner loop this way
    U64 captureKingPositions = MOVE_BITMASK[cards[cardIndex][PLAYER_0_CARDS + 0]][PLAYER_1][p1k] | MOVE_BITMASK[cards[cardIndex][PLAYER_0_CARDS + 1]][PLAYER_1][p1k];
    bool winPosition = preWonPositions & bbk0;
    bool win =
        (captureKingPositions & bbp0) || // win by capturing opponent's king
        (winPosition && !(bbp0 & PLAYER_WIN_BIT[0])); // win by moving king to win position

    if (win) {
        // A very small number of entries at the thread `tbIdx` boundaries will be accessed by two threads
        // concurrently, so we need to do an atomic update here. This seems to add ~10% execution time overall to the
        // win-in-one generator. We could probably optimize this by handling the boundary entries individually, however
        // that comes at the cost of more complicated and less readable code, so it hasn't been considered worthwhile.
        U64 curWdl = (*wdl)[cardIndex][tbIdx>>5];
        while (!(*wdl)[cardIndex][tbIdx>>5].compare_exchange_weak(curWdl, curWdl | 1ULL << (tbIdx&31)));
    }
    ++tbIdx;
}

// Copy the newly completed win-in-one WDL data to all duplicate card sets.
void copyDuplicateWins(U64 cardIndex, U64 tbStartIdx, U64 tbEndIdxExclusive, std::vector<U64>& duplicates) {
    if (tbStartIdx == tbEndIdxExclusive) return;

    U64 wStartIdx = tbStartIdx >> 5;
    U64 wEndIdxInclusive = (tbEndIdxExclusive-1) >> 5;
    S64 numEntriesToCopy = wEndIdxInclusive - wStartIdx - 1;

    for (U64 duplicateCardIndex : duplicates) {
        // The first and last WDL entry may be updated concurrently with another thread, so we must update them atomically
        for (U64 wIdx : {wStartIdx, wEndIdxInclusive}) {
            U64 newWins = (*wdl)[cardIndex][wIdx];
            U64 curWdl = (*wdl)[duplicateCardIndex][wIdx];
            while (!(*wdl)[duplicateCardIndex][wIdx].compare_exchange_weak(curWdl, curWdl | newWins));
        }

        // For all entries in between, the current thread has exclusive access, so we can just copy them
        if (numEntriesToCopy > 0)
            memcpy(&(*wdl)[duplicateCardIndex][wStartIdx+1], &(*wdl)[cardIndex][wStartIdx+1], numEntriesToCopy * sizeof((*wdl)[0][0]));
    }
}

#define FOR_KINGS(indexOffset, numPawnStates, x) \
    U64 tbStartIdx = indexOffset + KING_OFFSETS[p0k] * numPawnStates; \
    tbIdx = tbStartIdx; \
    for (U64 p1k = 0; p1k < 25; ++p1k) { \
        if (p1k == p0k || p1k == PLAYER_WIN_POSITION[1]) continue; \
        x; \
    } \
    copyDuplicateWins(cardIndex, tbStartIdx, tbIdx, duplicates);

#define FOR_P0_ZERO_PAWNS(x) \
    UNUSED(used); \
    x;

#define FOR_P1_ZERO_PAWNS(x) \
    U64 used = (1ULL << p0k) | (1ULL << p1k); \
    x;

#define FOR_P0_ONE_PAWN(x) \
    for (U64 p0p0 = 0; p0p0 < 25; ++p0p0) { \
        if ((1ULL << p0p0) & used) continue; \
        x; \
    }

#define FOR_P1_ONE_PAWN(x) \
    for (U64 p1p0 = 0; p1p0 < 25; ++p1p0) { \
        if (p1p0 == p0k || p1p0 == p1k) continue; \
        U64 used = (1ULL << p0k) | (1ULL << p1k) | (1ULL << p1p0); \
        x; \
    }

#define FOR_P0_TWO_PAWNS(x) \
    for (U64 p0p1 = 1; p0p1 < 25; ++p0p1) { \
        if ((1ULL << p0p1) & used) continue; \
        for (U64 p0p0 = 0; p0p0 < p0p1; ++p0p0) { \
            if ((1ULL << p0p0) & used) continue; \
            x; \
        } \
    }

#define FOR_P1_TWO_PAWNS(x) \
    for (U64 p1p1 = 1; p1p1 < 25; ++p1p1) { \
        if (p1p1 == p0k || p1p1 == p1k) continue; \
        for (U64 p1p0 = 0; p1p0 < p1p1; ++p1p0) { \
            if (p1p0 == p0k || p1p0 == p1k) continue; \
            U64 used = (1ULL << p0k) | (1ULL << p1k) | (1ULL << p1p0) | (1ULL << p1p1); \
            x; \
        } \
    }

#define FOR_P0_THREE_PAWNS(x) \
    for (U64 p0p2 = 2; p0p2 < 25; ++p0p2) { \
        if ((1ULL << p0p2) & used) continue; \
        for (U64 p0p1 = 1; p0p1 < p0p2; ++p0p1) { \
            if ((1ULL << p0p1) & used) continue; \
            for (U64 p0p0 = 0; p0p0 < p0p1; ++p0p0) { \
                if ((1ULL << p0p0) & used) continue; \
                x; \
            } \
        } \
    }

#define FOR_P1_THREE_PAWNS(x) \
    for (U64 p1p2 = 2; p1p2 < 25; ++p1p2) { \
        if (p1p2 == p0k || p1p2 == p1k) continue; \
        for (U64 p1p1 = 1; p1p1 < p1p2; ++p1p1) { \
            if (p1p1 == p0k || p1p1 == p1k) continue; \
            for (U64 p1p0 = 0; p1p0 < p1p1; ++p1p0) { \
                if (p1p0 == p0k || p1p0 == p1k) continue; \
                U64 used = (1ULL << p0k) | (1ULL << p1k) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2); \
                x; \
            } \
        } \
    }

#define FOR_P0_FOUR_PAWNS(x) \
    for (U64 p0p3 = 3; p0p3 < 25; ++p0p3) { \
        if ((1ULL << p0p3) & used) continue; \
        for (U64 p0p2 = 2; p0p2 < p0p3; ++p0p2) { \
            if ((1ULL << p0p2) & used) continue; \
            for (U64 p0p1 = 1; p0p1 < p0p2; ++p0p1) { \
                if ((1ULL << p0p1) & used) continue; \
                for (U64 p0p0 = 0; p0p0 < p0p1; ++p0p0) { \
                    if ((1ULL << p0p0) & used) continue; \
                    x; \
                } \
            } \
        } \
    }

#define FOR_P1_FOUR_PAWNS(x) \
    for (U64 p1p3 = 3; p1p3 < 25; ++p1p3) { \
        if (p1p3 == p0k || p1p3 == p1k) continue; \
        for (U64 p1p2 = 2; p1p2 < p1p3; ++p1p2) { \
            if (p1p2 == p0k || p1p2 == p1k) continue; \
            for (U64 p1p1 = 1; p1p1 < p1p2; ++p1p1) { \
                if (p1p1 == p0k || p1p1 == p1k) continue; \
                for (U64 p1p0 = 0; p1p0 < p1p1; ++p1p0) { \
                    if (p1p0 == p0k || p1p0 == p1k) continue; \
                    U64 used = (1ULL << p0k) | (1ULL << p1k) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3); \
                    x; \
                } \
            } \
        } \
    }

void w5v5(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_5V5, PAWN_STATES_5V5, FOR_P1_FOUR_PAWNS(FOR_P0_FOUR_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3), 1ULL << p0k, p1k, preWonPositions))));
}

void w5v4(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_5V4, PAWN_STATES_5V4, FOR_P1_THREE_PAWNS(FOR_P0_FOUR_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3), 1ULL << p0k, p1k, preWonPositions))));
}

void w4v5(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_4V5, PAWN_STATES_4V5, FOR_P1_FOUR_PAWNS(FOR_P0_THREE_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2), 1ULL << p0k, p1k, preWonPositions))));
}

void w5v3(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_5V3, PAWN_STATES_5V3, FOR_P1_TWO_PAWNS(FOR_P0_FOUR_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3), 1ULL << p0k, p1k, preWonPositions))));
}

void w3v5(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_3V5, PAWN_STATES_3V5, FOR_P1_FOUR_PAWNS(FOR_P0_TWO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1), 1ULL << p0k, p1k, preWonPositions))));
}

void w5v2(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_5V2, PAWN_STATES_5V2, FOR_P1_ONE_PAWN(FOR_P0_FOUR_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3), 1ULL << p0k, p1k, preWonPositions))));
}

void w2v5(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_2V5, PAWN_STATES_2V5, FOR_P1_FOUR_PAWNS(FOR_P0_ONE_PAWN(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0), 1ULL << p0k, p1k, preWonPositions))));
}

void w5v1(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_5V1, PAWN_STATES_5V1, FOR_P1_ZERO_PAWNS(FOR_P0_FOUR_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3), 1ULL << p0k, p1k, preWonPositions))));
}

void w1v5(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_1V5, PAWN_STATES_1V5, FOR_P1_FOUR_PAWNS(FOR_P0_ZERO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k, 1ULL << p0k, p1k, preWonPositions))));
}

void w4v4(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_4V4, PAWN_STATES_4V4, FOR_P1_THREE_PAWNS(FOR_P0_THREE_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2), 1ULL << p0k, p1k, preWonPositions))));
}

void w4v3(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_4V3, PAWN_STATES_4V3, FOR_P1_TWO_PAWNS(FOR_P0_THREE_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2), 1ULL << p0k, p1k, preWonPositions))));
}

void w3v4(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_3V4, PAWN_STATES_3V4, FOR_P1_THREE_PAWNS(FOR_P0_TWO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1), 1ULL << p0k, p1k, preWonPositions))));
}

void w4v2(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_4V2, PAWN_STATES_4V2, FOR_P1_ONE_PAWN(FOR_P0_THREE_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2), 1ULL << p0k, p1k, preWonPositions))));
}

void w2v4(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_2V4, PAWN_STATES_2V4, FOR_P1_THREE_PAWNS(FOR_P0_ONE_PAWN(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0), 1ULL << p0k, p1k, preWonPositions))));
}

void w4v1(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_4V1, PAWN_STATES_4V1, FOR_P1_ZERO_PAWNS(FOR_P0_THREE_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2), 1ULL << p0k, p1k, preWonPositions))));
}

void w1v4(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_1V4, PAWN_STATES_1V4, FOR_P1_THREE_PAWNS(FOR_P0_ZERO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k, 1ULL << p0k, p1k, preWonPositions))));
}

void w3v3(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_3V3, PAWN_STATES_3V3, FOR_P1_TWO_PAWNS(FOR_P0_TWO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1), 1ULL << p0k, p1k, preWonPositions))));
}

void w3v2(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_3V2, PAWN_STATES_3V2, FOR_P1_ONE_PAWN(FOR_P0_TWO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1), 1ULL << p0k, p1k, preWonPositions))));
}

void w2v3(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_2V3, PAWN_STATES_2V3, FOR_P1_TWO_PAWNS(FOR_P0_ONE_PAWN(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0), 1ULL << p0k, p1k, preWonPositions))));
}

void w3v1(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_3V1, PAWN_STATES_3V1, FOR_P1_ZERO_PAWNS(FOR_P0_TWO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0) | (1ULL << p0p1), 1ULL << p0k, p1k, preWonPositions))));
}

void w1v3(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_1V3, PAWN_STATES_1V3, FOR_P1_TWO_PAWNS(FOR_P0_ZERO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k, 1ULL << p0k, p1k, preWonPositions))));
}

void w2v2(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_2V2, PAWN_STATES_2V2, FOR_P1_ONE_PAWN(FOR_P0_ONE_PAWN(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0), 1ULL << p0k, p1k, preWonPositions))));
}

void w2v1(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_2V1, PAWN_STATES_2V1, FOR_P1_ZERO_PAWNS(FOR_P0_ONE_PAWN(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k | (1ULL << p0p0), 1ULL << p0k, p1k, preWonPositions))));
}

void w1v2(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_1V2, PAWN_STATES_1V2, FOR_P1_ONE_PAWN(FOR_P0_ZERO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k, 1ULL << p0k, p1k, preWonPositions))));
}

void w1v1(U64 cardIndex, U64& tbIdx, U64 preWonPositions, U64 p0k, std::vector<U64>& duplicates) {
    FOR_KINGS(OFFSET_1V1, PAWN_STATES_1V1, FOR_P1_ZERO_PAWNS(FOR_P0_ZERO_PAWNS(
        checkWinInOne(cardIndex, tbIdx, 1ULL << p0k, 1ULL << p0k, p1k, preWonPositions))));
}

#undef FOR_KINGS
#undef FOR_P0_ZERO_PAWNS
#undef FOR_P1_ZERO_PAWNS
#undef FOR_P0_ONE_PAWN
#undef FOR_P1_ONE_PAWN
#undef FOR_P0_TWO_PAWNS
#undef FOR_P1_TWO_PAWNS
#undef FOR_P0_THREE_PAWNS
#undef FOR_P1_THREE_PAWNS
#undef FOR_P0_FOUR_PAWNS
#undef FOR_P1_FOUR_PAWNS

// Find all duplicate P0 card tuples among all 30 card combinations. There are at most 10 unique card tuples, since
// we can choose the P0 cards in 10 ways (5 * 4 / 2). However, depending on the specific cards, there may be fewer
// unique tuples, since different P0 tuples can combine into the same set of available moves.
void findDuplicateP0CardTuples(std::vector<std::pair<U64, std::vector<U64>>>& duplicatesForP0CardSet) {
    std::unordered_map<U64, U64> firstIndexForTupleBitmask;

    for (U64 c = 0; c < NUM_CARD_COMBINATIONS; ++c) {
        U64 tupleBitmask = MOVE_BITMASK[cards[c][PLAYER_0_CARDS + 0]][PLAYER_0][12] | MOVE_BITMASK[cards[c][PLAYER_0_CARDS + 1]][PLAYER_0][12]; // 12 = center of board
        auto e = firstIndexForTupleBitmask.find(tupleBitmask);
        if (e == firstIndexForTupleBitmask.end()) {
            firstIndexForTupleBitmask[tupleBitmask] = duplicatesForP0CardSet.size();
            duplicatesForP0CardSet.push_back(std::make_pair(c, std::vector<U64>()));
        } else {
            duplicatesForP0CardSet[e->second].second.push_back(c);
        }
    }
}

void generateWinInOne() {
    std::vector<std::pair<U64, std::vector<U64>>> duplicatesForP0CardSet;
    findDuplicateP0CardTuples(duplicatesForP0CardSet);

    #if INCLUDE_IMBALANCED_STATES && TB_PIECES < 10
        // this would be trivial to add if needed; just process the desired piece counts in the order given in CompactIndexer
        printf("Imbalanced states currently not supported in win-in-one generator!\n");
        exit(0);
    #endif

    auto moveLambda = [&duplicatesForP0CardSet](U64 threadId) {
        auto [cardIndex, duplicates] = duplicatesForP0CardSet[threadId / 25];
        U64 p0k = threadId % 25;
        if (p0k == PLAYER_WIN_POSITION[0]) return; // already won state, so not a win-in-one

        U64 preWonPositions = MOVE_BITMASK[cards[cardIndex][PLAYER_0_CARDS + 0]][PLAYER_1][PLAYER_WIN_POSITION[0]] | MOVE_BITMASK[cards[cardIndex][PLAYER_0_CARDS + 1]][PLAYER_1][PLAYER_WIN_POSITION[0]];

        #define w(x) x(cardIndex, tbIdx, preWonPositions, p0k, duplicates)

        U64 tbIdx = 0;

        #if TB_PIECES == 10
            w(w5v5); w(w5v4); w(w4v5); w(w5v3); w(w3v5); w(w5v2); w(w2v5); w(w5v1); w(w1v5);
            w(w4v4); w(w4v3); w(w3v4); w(w4v2); w(w2v4); w(w4v1); w(w1v4);
            w(w3v3); w(w3v2); w(w2v3); w(w3v1); w(w1v3);
            w(w2v2); w(w2v1); w(w1v2); w(w1v1);
        #elif TB_PIECES == 8
            w(w4v4); w(w4v3); w(w3v4); w(w4v2); w(w2v4); w(w4v1); w(w1v4);
            w(w3v3); w(w3v2); w(w2v3); w(w3v1); w(w1v3);
            w(w2v2); w(w2v1); w(w1v2); w(w1v1);
        #elif TB_PIECES == 6
            w(w3v3); w(w3v2); w(w2v3); w(w3v1); w(w1v3);
            w(w2v2); w(w2v1); w(w1v2); w(w1v1);
        #elif TB_PIECES == 4
            w(w2v2); w(w2v1); w(w1v2); w(w1v1);
        #endif

        #undef w
    };

    std::vector<std::thread> threads;
    for (U64 t = 0; t < duplicatesForP0CardSet.size() * 25; ++t)
        threads.push_back(std::thread(moveLambda, t));
    for (auto& thread : threads)
        thread.join();
}
