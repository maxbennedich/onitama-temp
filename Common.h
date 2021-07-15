#pragma once

#include <cstdint>
#include <string>

typedef uint64_t U64;
typedef uint32_t U32;
typedef uint16_t U16;
typedef uint8_t U8;

typedef int64_t S64;
typedef int32_t S32;
typedef int16_t S16;
typedef int8_t S8;

#define TB_PIECES 6

// If this is true, also include 3v1 for 4 pieces, 4v1, 4v2 and 5v1 for 6 pieces, and 5v1, 5v2 and 5v3 for 8 pieces
// (plus their inverses, i.e. 1v3 etc). Note that for 6 pieces, 5v1 adds quite a bit of processing time and might not
// be very useful for some applications, since those states are likely easily searched by an alpha-beta search.
// Also note that this setting is only applicable with the compact indexer.
// Including these states might not be very helpful for a game-playing AI, where we only need to find the best next
// move, since the imbalanced states are most likely wins/losses anyway. But when it comes to proving that a state
// is a win or loss through alpha-beta search, they reduce the search time by about an order of magnitude, since
// otherwise a lot of time will be spent searching and proving these imbalanced states.
#define INCLUDE_IMBALANCED_STATES false

#define USE_COMPACT_INDEXER true

#if TB_PIECES == 4
    #if INCLUDE_IMBALANCED_STATES
        const U64 TABLE_SIZE = 585'627;
    #else
        #if USE_COMPACT_INDEXER
            const U64 TABLE_SIZE = 305'809;
        #else
            const U64 TABLE_SIZE = 25*25*25*25; // 390'625
        #endif
    #endif
    const U64 STATE_QUEUE_SIZE = 100'000;
#elif TB_PIECES == 6
    #if INCLUDE_IMBALANCED_STATES
        const U64 TABLE_SIZE = 86'769'571;
    #else
        #if USE_COMPACT_INDEXER
            const U64 TABLE_SIZE = 35'842'695;
        #else
            const U64 TABLE_SIZE = 25*25*26*25/2*26*25/2; // 66'015'625
        #endif
    #endif
    const U64 STATE_QUEUE_SIZE = 2'000'000;
#elif TB_PIECES == 8
    #if INCLUDE_IMBALANCED_STATES
        const U64 TABLE_SIZE = 3'436'191'031;
    #else
        const U64 TABLE_SIZE = 1'565'607'701;
    #endif
    const U64 STATE_QUEUE_SIZE = 12'000'000; // TODO: probably can increase this quite a bit! (if memory allows)
#elif TB_PIECES == 10
    const U64 TABLE_SIZE = 31'906'273'441;
    const U64 STATE_QUEUE_SIZE = 40'000'000;
#endif

const U64 NUM_CARD_COMBINATIONS = 30;

const U64 WDL_SIZE = (TABLE_SIZE + 31) >> 5;

const U64 PLAYER_0 = 0;
const U64 PLAYER_1 = 1;

const U64 PLAYER_0_WIN_POSITION = 2;
const U64 PLAYER_1_WIN_POSITION = 22;
const U64 PLAYER_WIN_POSITION[] = {PLAYER_0_WIN_POSITION, PLAYER_1_WIN_POSITION};

const U64 PLAYER_0_WIN_BIT = 1ULL << PLAYER_0_WIN_POSITION;
const U64 PLAYER_1_WIN_BIT = 1ULL << PLAYER_1_WIN_POSITION;
const U64 PLAYER_WIN_BIT[] = {PLAYER_0_WIN_BIT, PLAYER_1_WIN_BIT};

const U64 PLAYER_0_WIN_MASK = ~PLAYER_0_WIN_BIT;
const U64 PLAYER_1_WIN_MASK = ~PLAYER_1_WIN_BIT;

const U64 NUM_CARDS_PER_HAND = 2;
const U64 NUM_CARDS_PER_GAME = 5;
const U64 NUM_PLAYERS = 2;

// card indices
const U64 NEXT_CARD = 0;
const U64 PLAYER_0_CARDS = 1;
const U64 PLAYER_1_CARDS = 3;

#define UNUSED(a) (void)(a)

template<typename ... Args>
static std::string format(const std::string& fmt, Args ... args) {
    size_t size = snprintf(nullptr, 0, fmt.c_str(), args ...) + 1; // Extra space for '\0'
    std::unique_ptr<char[]> buf(new char[size]);
    snprintf(buf.get(), size, fmt.c_str(), args ...);
    return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
}
