#pragma once

#include <algorithm>
#include <chrono>
#include <cinttypes> // PRIu64
#include <cstring> // memset
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include <vector>

#include "Common.h"

U64 twoKingsIndex[25*25]; // 625 (compacted index from 0..552)
U8 twoKings[(24*23+1)*2]; // 553*2 (24-k0, 24-k1)
U8 twoPawns[(23*22/2)*2]; // 253*2 (p0, p1 where p0 < p1; colexicographical order)
U8 threePawns[(23*22*21/6)*3]; // 1771*3 (p0, p1, p2 where p0 < p1 < p2; colexicographical order)
U8 fourPawns[(23*22*21*20/24)*4]; // 8855*4 (p0, p1, p2, p3 where p0 < p1 < p2 < p3; colexicographical order)

const U64 OFFSET_5V5 = 0;
const U64 OFFSET_5V4 = (OFFSET_5V5 + 18980054940); // (24+23*23) * (23*22*21*20/24) * (19*18*17*16/24)
const U64 OFFSET_4V5 = (OFFSET_5V4 + 4745013735); // (24+23*23) * (23*22*21*20/24) * (19*18*17/6)

#if TB_PIECES == 8 && INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_5V3 = 0;
#else
    const U64 OFFSET_5V3 = (OFFSET_4V5 + 4745013735); // (24+23*23) * (23*22*21*20/24) * (19*18*17/6)
#endif

const U64 OFFSET_3V5 = (OFFSET_5V3 + 837355365); // (24+23*23) * (23*22*21*20/24) * (19*18/2)
const U64 OFFSET_5V2 = (OFFSET_3V5 + 837355365); // (24+23*23) * (23*22*21*20/24) * (19*18/2)
const U64 OFFSET_2V5 = (OFFSET_5V2 + 93039485); // (24+23*23) * (23*22*21*20/24) * 19

#if TB_PIECES != 6 || !INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_5V1 = (OFFSET_2V5 + 93039485); // (24+23*23) * (23*22*21*20/24) * 19
    const U64 OFFSET_1V5 = (OFFSET_5V1 + 4896815); // (24+23*23) * (23*22*21*20/24)
#endif

#if TB_PIECES == 8 && !INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_4V4 = 0;
#else
    const U64 OFFSET_4V4 = (OFFSET_1V5 + 4896815); // (24+23*23) * (23*22*21*20/24)
#endif

const U64 OFFSET_4V3 = (OFFSET_4V4 + 1116473820); // (24+23*23) * (23*22*21/6) * (20*19*18/6)
const U64 OFFSET_3V4 = (OFFSET_4V3 + 186078970); // (24+23*23) * (23*22*21/6) * (20*19/2)

#if TB_PIECES == 6 && INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_5V1 = 0;
    const U64 OFFSET_1V5 = (OFFSET_5V1 + 4896815); // (24+23*23) * (23*22*21*20/24)
    const U64 OFFSET_4V2 = (OFFSET_1V5 + 4896815); // (24+23*23) * (23*22*21*20/24)
#else
    const U64 OFFSET_4V2 = (OFFSET_3V4 + 186078970); // (24+23*23) * (23*22*21/6) * (20*19/2)
#endif

const U64 OFFSET_2V4 = (OFFSET_4V2 + 19587260); // (24+23*23) * (23*22*21/6) * 20
const U64 OFFSET_4V1 = (OFFSET_2V4 + 19587260); // (24+23*23) * (23*22*21/6) * 20
const U64 OFFSET_1V4 = (OFFSET_4V1 + 979363); // (24+23*23) * (23*22*21/6)

#if TB_PIECES == 6 && !INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_3V3 = 0;
#else
    const U64 OFFSET_3V3 (OFFSET_1V4 + 979363); // (24+23*23) * (23*22*21/6)
#endif

const U64 OFFSET_3V2 = (OFFSET_3V3 + 29380890); // (24+23*23) * (23*22/2) * (21*20/2)
const U64 OFFSET_2V3 = (OFFSET_3V2 + 2938089); // (24+23*23) * (23*22/2) * 21

#if TB_PIECES == 4 && INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_3V1 = 0;
#else
    const U64 OFFSET_3V1 = (OFFSET_2V3 + 2938089); // (24+23*23) * (23*22/2) * 21
#endif

const U64 OFFSET_1V3 = (OFFSET_3V1 + 139909); // (24+23*23) * (23*22/2)

#if TB_PIECES == 4 && !INCLUDE_IMBALANCED_STATES
    const U64 OFFSET_2V2 = 0;
#else
    const U64 OFFSET_2V2 = (OFFSET_1V3 + 139909); // (24+23*23) * (23*22/2)
#endif

const U64 OFFSET_2V1 = (OFFSET_2V2 + 279818); // (24+23*23) * 23 * 22
const U64 OFFSET_1V2 = (OFFSET_2V1 + 12719); // (24+23*23) * 23
const U64 OFFSET_1V1 = (OFFSET_1V2 + 12719); // (24+23*23) * 23

const U64 KING_OFFSETS[] = {
    0, 1*23, 2*23, 2*23, 3*23, 4*23, 5*23, 6*23, 7*23, 8*23, 9*23, 10*23, 11*23, 12*23,
    13*23, 14*23, 15*23, 16*23, 17*23, 18*23, 19*23, 20*23, 21*23, 22*23+1, 23*23+1, 24*23+1};

const U64 PAWN_STATES_5V5 = (23*22*21*20/24) * (19*18*17*16/24);
const U64 PAWN_STATES_5V4 = (23*22*21*20/24) * (19*18*17/6);
const U64 PAWN_STATES_4V5 = PAWN_STATES_5V4;
const U64 PAWN_STATES_5V3 = (23*22*21*20/24) * (19*18/2);
const U64 PAWN_STATES_3V5 = PAWN_STATES_5V3;
const U64 PAWN_STATES_5V2 = (23*22*21*20/24) * 19;
const U64 PAWN_STATES_2V5 = PAWN_STATES_5V2;
const U64 PAWN_STATES_5V1 = 23*22*21*20/24;
const U64 PAWN_STATES_1V5 = PAWN_STATES_5V1;

const U64 PAWN_STATES_4V4 = (23*22*21/6) * (20*19*18/6);
const U64 PAWN_STATES_4V3 = (23*22*21/6) * (20*19/2);
const U64 PAWN_STATES_3V4 = PAWN_STATES_4V3;
const U64 PAWN_STATES_4V2 = (23*22*21/6) * 20;
const U64 PAWN_STATES_2V4 = PAWN_STATES_4V2;
const U64 PAWN_STATES_4V1 = 23*22*21/6;
const U64 PAWN_STATES_1V4 = PAWN_STATES_4V1;

const U64 PAWN_STATES_3V3 = (23*22/2) * (21*20/2);
const U64 PAWN_STATES_3V2 = (23*22/2) * 21;
const U64 PAWN_STATES_2V3 = PAWN_STATES_3V2;
const U64 PAWN_STATES_3V1 = 23*22/2;
const U64 PAWN_STATES_1V3 = PAWN_STATES_3V1;

const U64 PAWN_STATES_2V2 = 23 * 22;
const U64 PAWN_STATES_2V1 = 23;
const U64 PAWN_STATES_1V2 = PAWN_STATES_2V1;

const U64 PAWN_STATES_1V1 = 1;

template <bool invert>
U64 i1v1(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp0); UNUSED(bbp1);
    U64 p0k, p1k;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
    }

    return OFFSET_1V1 + twoKingsIndex[p0k*25 + p1k];
}

template <bool invert, typename T>
void u1v1(U64 i, U64 cardIndex, T visitor) {
    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    if (invert) {
        visitor(
            1ULL << k1,
            1ULL << k0,
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            1ULL << (24-k0),
            1ULL << (24-k1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i1v2(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp0);
    U64 p0k, p1k, p1p0;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = 24 - __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    }

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);

    return OFFSET_1V2 + twoKingsIndex[p0k*25 + p1k] * 23 + p1p0;
}

template <bool invert, typename T>
void u1v2(U64 i, U64 cardIndex, T visitor) {
    U64 p1p0 = i % 23;
    i /= 23;

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)),
            1ULL << k0,
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            1ULL << (24-k0),
            (1ULL << (24-k1)) | (1ULL << p1p0),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i2v1(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp1);
    U64 p0k, p0p0, p1k;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = 24 - __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k);

    return OFFSET_2V1 + twoKingsIndex[p0k*25 + p1k] * 23 + p0p0;
}

template <bool invert, typename T>
void u2v1(U64 i, U64 cardIndex, T visitor) {
    U64 p0p0 = i % 23;
    i /= 23;

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P0 pawns skip over kings and P1 pawns
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0;

    if (invert) {
        visitor(
            1ULL << k1,
            (1ULL << k0) | (1ULL << (24-p0p0)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0),
            1ULL << (24-k1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i2v2(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p1k, p1p0;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = 24 - __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = 24 - __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);

    return OFFSET_2V2 + (twoKingsIndex[p0k*25 + p1k] * 23 + p1p0) * 22 + p0p0;
}

template <bool invert, typename T>
void u2v2(U64 i, U64 cardIndex, T visitor) {
    U64 p0p0 = i % 22;
    i /= 22;

    U64 p1p0 = i % 23;
    i /= 23;

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0;

    // P0 pawns skip over kings and P1 pawns
    material |= 1ULL << p1p0;
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)),
            (1ULL << k0) | (1ULL << (24-p0p0)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0),
            (1ULL << (24-k1)) | (1ULL << p1p0),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i1v3(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp0);
    U64 p0k, p1k, p1p0, p1p1;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);

    return OFFSET_1V3 + twoKingsIndex[p0k*25 + p1k] * (23*22/2) + p1p1 * (p1p1-1) / 2 + p1p0;
}

template <bool invert, typename T>
void u1v3(U64 i, U64 cardIndex, T visitor) {
    U64 p1i = i % (23*22/2);
    i /= 23*22/2;
    U64 p1p0 = twoPawns[p1i*2];
    U64 p1p1 = twoPawns[p1i*2+1];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)),
            1ULL << k0,
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            1ULL << (24-k0),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i3v1(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp1);
    U64 p0k, p0p0, p0p1, p1k;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k);

    return OFFSET_3V1 + twoKingsIndex[p0k*25 + p1k] * (23*22/2) + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u3v1(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (23*22/2);
    i /= 23*22/2;
    U64 p0p0 = twoPawns[p0i*2];
    U64 p0p1 = twoPawns[p0i*2+1];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P0 pawns skip over kings and P1 pawns
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;

    if (invert) {
        visitor(
            1ULL << k1,
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1),
            1ULL << (24-k1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i2v3(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p1k, p1p0, p1p1;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = 24 - __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);

    return OFFSET_2V3 + (twoKingsIndex[p0k*25 + p1k] * (23*22/2) + p1p1 * (p1p1-1) / 2 + p1p0) * 21 + p0p0;
}

template <bool invert, typename T>
void u2v3(U64 i, U64 cardIndex, T visitor) {
    U64 p0p0 = i % 21;
    i /= 21;

    U64 p1i = i % (23*22/2);
    i /= 23*22/2;
    U64 p1p0 = twoPawns[p1i*2];
    U64 p1p1 = twoPawns[p1i*2+1];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)),
            (1ULL << k0) | (1ULL << (24-p0p0)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i3v2(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p1k, p1p0;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = 24 - __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);

    return OFFSET_3V2 + (twoKingsIndex[p0k*25 + p1k] * 23 + p1p0) * (22*21/2) + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u3v2(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (22*21/2);
    i /= 22*21/2;
    U64 p0p0 = twoPawns[p0i*2];
    U64 p0p1 = twoPawns[p0i*2+1];

    U64 p1p0 = i % 23;
    i /= 23;

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0;

    // P0 pawns skip over kings and P1 pawns
    material |= 1ULL << p1p0;
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1),
            (1ULL << (24-k1)) | (1ULL << p1p0),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i3v3(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p1k, p1p0, p1p1;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);

    return OFFSET_3V3 + (twoKingsIndex[p0k*25 + p1k] * (23*22/2) + p1p1 * (p1p1-1) / 2 + p1p0) * (21*20/2) + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u3v3(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (21*20/2);
    i /= 21*20/2;
    U64 p0p0 = twoPawns[p0i*2];
    U64 p0p1 = twoPawns[p0i*2+1];

    U64 p1i = i % (23*22/2);
    i /= 23*22/2;
    U64 p1p0 = twoPawns[p1i*2];
    U64 p1p1 = twoPawns[p1i*2+1];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i1v4(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp0);
    U64 p0k, p1k, p1p0, p1p1, p1p2;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);

    return OFFSET_1V4 + twoKingsIndex[p0k*25 + p1k] * (23*22*21/6) + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0;
}

template <bool invert, typename T>
void u1v4(U64 i, U64 cardIndex, T visitor) {
    U64 p1i = i % (23*22*21/6);
    i /= 23*22*21/6;
    U64 p1p0 = threePawns[p1i*3];
    U64 p1p1 = threePawns[p1i*3+1];
    U64 p1p2 = threePawns[p1i*3+2];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)),
            1ULL << k0,
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            1ULL << (24-k0),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i4v1(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp1);
    U64 p0k, p0p0, p0p1, p0p2, p1k;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k);

    return OFFSET_4V1 + twoKingsIndex[p0k*25 + p1k] * (23*22*21/6) + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u4v1(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (23*22*21/6);
    i /= 23*22*21/6;
    U64 p0p0 = threePawns[p0i*3];
    U64 p0p1 = threePawns[p0i*3+1];
    U64 p0p2 = threePawns[p0i*3+2];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P0 pawns skip over kings and P1 pawns
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;

    if (invert) {
        visitor(
            1ULL << k1,
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2),
            1ULL << (24-k1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i2v4(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p1k, p1p0, p1p1, p1p2;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = 24 - __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);

    return OFFSET_2V4 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21/6) + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * 20 + p0p0;
}

template <bool invert, typename T>
void u2v4(U64 i, U64 cardIndex, T visitor) {
    U64 p0p0 = i % 20;
    i /= 20;

    U64 p1i = i % (23*22*21/6);
    i /= 23*22*21/6;
    U64 p1p0 = threePawns[p1i*3];
    U64 p1p1 = threePawns[p1i*3+1];
    U64 p1p2 = threePawns[p1i*3+2];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)),
            (1ULL << k0) | (1ULL << (24-p0p0)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i4v2(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p1k, p1p0;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = 24 - __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);

    return OFFSET_4V2 + (twoKingsIndex[p0k*25 + p1k] * 23 + p1p0) * (22*21*20/6) + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u4v2(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (22*21*20/6);
    i /= 22*21*20/6;
    U64 p0p0 = threePawns[p0i*3];
    U64 p0p1 = threePawns[p0i*3+1];
    U64 p0p2 = threePawns[p0i*3+2];

    U64 p1p0 = i % 23;
    i /= 23;

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0;

    // P0 pawns skip over kings and P1 pawns
    material |= 1ULL << p1p0;
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2),
            (1ULL << (24-k1)) | (1ULL << p1p0),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i3v4(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p1k, p1p0, p1p1, p1p2;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1) + (p0p1 > p1p2);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);

    return OFFSET_3V4 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21/6) + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * (20*19/2) + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u3v4(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (20*19/2);
    i /= 20*19/2;
    U64 p0p0 = twoPawns[p0i*2];
    U64 p0p1 = twoPawns[p0i*2+1];

    U64 p1i = i % (23*22*21/6);
    i /= 23*22*21/6;
    U64 p1p0 = threePawns[p1i*3];
    U64 p1p1 = threePawns[p1i*3+1];
    U64 p1p2 = threePawns[p1i*3+2];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i4v3(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p1k, p1p0, p1p1;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0) + (p0p2 > p1p1);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);

    return OFFSET_4V3 + (twoKingsIndex[p0k*25 + p1k] * (23*22/2) + p1p1 * (p1p1-1) / 2 + p1p0) * (21*20*19/6) + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u4v3(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (21*20*19/6);
    i /= 21*20*19/6;
    U64 p0p0 = threePawns[p0i*3];
    U64 p0p1 = threePawns[p0i*3+1];
    U64 p0p2 = threePawns[p0i*3+2];

    U64 p1i = i % (23*22/2);
    i /= 23*22/2;
    U64 p1p0 = twoPawns[p1i*2];
    U64 p1p1 = twoPawns[p1i*2+1];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i4v4(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p1k, p1p0, p1p1, p1p2;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1) + (p0p1 > p1p2);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0) + (p0p2 > p1p1) + (p0p2 > p1p2);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);

    return OFFSET_4V4 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21/6)
        + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * (20*19*18/6)
        + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u4v4(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (20*19*18/6);
    i /= 20*19*18/6;
    U64 p0p0 = threePawns[p0i*3];
    U64 p0p1 = threePawns[p0i*3+1];
    U64 p0p2 = threePawns[p0i*3+2];

    U64 p1i = i % (23*22*21/6);
    i /= 23*22*21/6;
    U64 p1p0 = threePawns[p1i*3];
    U64 p1p1 = threePawns[p1i*3+1];
    U64 p1p2 = threePawns[p1i*3+2];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i1v5(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp0);
    U64 p0k, p1k, p1p0, p1p1, p1p2, p1p3;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p3 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p3 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);
    p1p3 -= (p1p3 > p0k) + (p1p3 > p1k);

    return OFFSET_1V5 + twoKingsIndex[p0k*25 + p1k] * (23*22*21*20/24) + p1p3 * (p1p3-1) * (p1p3-2) * (p1p3-3) / 24 + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0;
}

template <bool invert, typename T>
void u1v5(U64 i, U64 cardIndex, T visitor) {
    U64 p1i = i % (23*22*21*20/24);
    i /= 23*22*21*20/24;
    U64 p1p0 = fourPawns[p1i*4];
    U64 p1p1 = fourPawns[p1i*4+1];
    U64 p1p2 = fourPawns[p1i*4+2];
    U64 p1p3 = fourPawns[p1i*4+3];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)) | (1ULL << (24-p1p3)),
            1ULL << k0,
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            1ULL << (24-k0),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i5v1(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    UNUSED(bbp1);
    U64 p0k, p0p0, p0p1, p0p2, p0p3, p1k;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p3 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p3 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k);
    p0p3 -= (p0p3 > p0k) + (p0p3 > p1k);

    return OFFSET_5V1 + twoKingsIndex[p0k*25 + p1k] * (23*22*21*20/24) + p0p3 * (p0p3-1) * (p0p3-2) * (p0p3-3) / 24 + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u5v1(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (23*22*21*20/24);
    i /= 23*22*21*20/24;
    U64 p0p0 = fourPawns[p0i*4];
    U64 p0p1 = fourPawns[p0i*4+1];
    U64 p0p2 = fourPawns[p0i*4+2];
    U64 p0p3 = fourPawns[p0i*4+3];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P0 pawns skip over kings and P1 pawns
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;

    if (invert) {
        visitor(
            1ULL << k1,
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)) | (1ULL << (24-p0p3)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3),
            1ULL << (24-k1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i2v5(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p1k, p1p0, p1p1, p1p2, p1p3;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p3 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = 24 - __builtin_ia32_tzcnt_u64(bbp0 - bbk0);
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p3 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2) + (p0p0 > p1p3);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);
    p1p3 -= (p1p3 > p0k) + (p1p3 > p1k);

    return OFFSET_2V5 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21*20/24)
        + p1p3 * (p1p3-1) * (p1p3-2) * (p1p3-3) / 24 + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * 19
        + p0p0;
}

template <bool invert, typename T>
void u2v5(U64 i, U64 cardIndex, T visitor) {
    U64 p0p0 = i % 19;
    i /= 19;

    U64 p1i = i % (23*22*21*20/24);
    i /= 23*22*21*20/24;
    U64 p1p0 = fourPawns[p1i*4];
    U64 p1p1 = fourPawns[p1i*4+1];
    U64 p1p2 = fourPawns[p1i*4+2];
    U64 p1p3 = fourPawns[p1i*4+3];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)) | (1ULL << (24-p1p3)),
            (1ULL << k0) | (1ULL << (24-p0p0)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i5v2(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p0p3, p1k, p1p0;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p3 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p3 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = 24 - __builtin_ia32_tzcnt_u64(bbp1 - bbk1);
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0);
    p0p3 -= (p0p3 > p0k) + (p0p3 > p1k) + (p0p3 > p1p0);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);

    return OFFSET_5V2 + (twoKingsIndex[p0k*25 + p1k] * 23
        + p1p0) * (22*21*20*19/24)
        + p0p3 * (p0p3-1) * (p0p3-2) * (p0p3-3) / 24 + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u5v2(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (22*21*20*19/24);
    i /= 22*21*20*19/24;
    U64 p0p0 = fourPawns[p0i*4];
    U64 p0p1 = fourPawns[p0i*4+1];
    U64 p0p2 = fourPawns[p0i*4+2];
    U64 p0p3 = fourPawns[p0i*4+3];

    U64 p1p0 = i % 23;
    i /= 23;

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0;

    // P0 pawns skip over kings and P1 pawns
    material |= 1ULL << p1p0;
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)) | (1ULL << (24-p0p3)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3),
            (1ULL << (24-k1)) | (1ULL << p1p0),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i3v5(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p1k, p1p0, p1p1, p1p2, p1p3;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p3 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p3 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2) + (p0p0 > p1p3);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1) + (p0p1 > p1p2) + (p0p1 > p1p3);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);
    p1p3 -= (p1p3 > p0k) + (p1p3 > p1k);

    return OFFSET_3V5 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21*20/24)
        + p1p3 * (p1p3-1) * (p1p3-2) * (p1p3-3) / 24 + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * (19*18/2)
        + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u3v5(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (19*18/2);
    i /= 19*18/2;
    U64 p0p0 = twoPawns[p0i*2];
    U64 p0p1 = twoPawns[p0i*2+1];

    U64 p1i = i % (23*22*21*20/24);
    i /= 23*22*21*20/24;
    U64 p1p0 = fourPawns[p1i*4];
    U64 p1p1 = fourPawns[p1i*4+1];
    U64 p1p2 = fourPawns[p1i*4+2];
    U64 p1p3 = fourPawns[p1i*4+3];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)) | (1ULL << (24-p1p3)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i5v3(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p0p3, p1k, p1p0, p1p1;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p3 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p3 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0) + (p0p2 > p1p1);
    p0p3 -= (p0p3 > p0k) + (p0p3 > p1k) + (p0p3 > p1p0) + (p0p3 > p1p1);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);

    return OFFSET_5V3 + (twoKingsIndex[p0k*25 + p1k] * (23*22/2)
        + p1p1 * (p1p1-1) / 2 + p1p0) * (21*20*19*18/24)
        + p0p3 * (p0p3-1) * (p0p3-2) * (p0p3-3) / 24 + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u5v3(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (21*20*19*18/24);
    i /= 21*20*19*18/24;
    U64 p0p0 = fourPawns[p0i*4];
    U64 p0p1 = fourPawns[p0i*4+1];
    U64 p0p2 = fourPawns[p0i*4+2];
    U64 p0p3 = fourPawns[p0i*4+3];

    U64 p1i = i % (23*22/2);
    i /= 23*22/2;
    U64 p1p0 = twoPawns[p1i*2];
    U64 p1p1 = twoPawns[p1i*2+1];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)) | (1ULL << (24-p0p3)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i4v5(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p1k, p1p0, p1p1, p1p2, p1p3;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p3 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p3 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2) + (p0p0 > p1p3);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1) + (p0p1 > p1p2) + (p0p1 > p1p3);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0) + (p0p2 > p1p1) + (p0p2 > p1p2) + (p0p2 > p1p3);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);
    p1p3 -= (p1p3 > p0k) + (p1p3 > p1k);

    return OFFSET_4V5 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21*20/24)
        + p1p3 * (p1p3-1) * (p1p3-2) * (p1p3-3) / 24 + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * (19*18*17/6)
        + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u4v5(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (19*18*17/6);
    i /= 19*18*17/6;
    U64 p0p0 = threePawns[p0i*3];
    U64 p0p1 = threePawns[p0i*3+1];
    U64 p0p2 = threePawns[p0i*3+2];

    U64 p1i = i % (23*22*21*20/24);
    i /= 23*22*21*20/24;
    U64 p1p0 = fourPawns[p1i*4];
    U64 p1p1 = fourPawns[p1i*4+1];
    U64 p1p2 = fourPawns[p1i*4+2];
    U64 p1p3 = fourPawns[p1i*4+3];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)) | (1ULL << (24-p1p3)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i5v4(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p0p3, p1k, p1p0, p1p1, p1p2;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p3 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p3 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1) + (p0p1 > p1p2);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0) + (p0p2 > p1p1) + (p0p2 > p1p2);
    p0p3 -= (p0p3 > p0k) + (p0p3 > p1k) + (p0p3 > p1p0) + (p0p3 > p1p1) + (p0p3 > p1p2);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);

    return OFFSET_5V4 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21/6)
        + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * (20*19*18*17/24)
        + p0p3 * (p0p3-1) * (p0p3-2) * (p0p3-3) / 24 + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u5v4(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (20*19*18*17/24);
    i /= 20*19*18*17/24;
    U64 p0p0 = fourPawns[p0i*4];
    U64 p0p1 = fourPawns[p0i*4+1];
    U64 p0p2 = fourPawns[p0i*4+2];
    U64 p0p3 = fourPawns[p0i*4+3];

    U64 p1i = i % (23*22*21/6);
    i /= 23*22*21/6;
    U64 p1p0 = threePawns[p1i*3];
    U64 p1p1 = threePawns[p1i*3+1];
    U64 p1p2 = threePawns[p1i*3+2];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)) | (1ULL << (24-p0p3)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

template <bool invert>
U64 i5v5(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    U64 p0k, p0p0, p0p1, p0p2, p0p3, p1k, p1p0, p1p1, p1p2, p1p3;
    if (!invert) {
        p0k = __builtin_ia32_tzcnt_u64(bbk0);
        p0p0 = __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p1 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p2 = __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p3 = 63 - __builtin_ia32_lzcnt_u64(bbp0);
        p1k = __builtin_ia32_tzcnt_u64(bbk1);
        p1p0 = __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p1 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p2 = __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p3 = 63 - __builtin_ia32_lzcnt_u64(bbp1);
    } else {
        p0k = 24 - __builtin_ia32_tzcnt_u64(bbk0);
        p0p3 = 24 - __builtin_ia32_tzcnt_u64(bbp0 -= bbk0);
        p0p2 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p1 = 24 - __builtin_ia32_tzcnt_u64(bbp0 &= bbp0-1);
        p0p0 = __builtin_ia32_lzcnt_u64(bbp0) - 39;
        p1k = 24 - __builtin_ia32_tzcnt_u64(bbk1);
        p1p3 = 24 - __builtin_ia32_tzcnt_u64(bbp1 -= bbk1);
        p1p2 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p1 = 24 - __builtin_ia32_tzcnt_u64(bbp1 &= bbp1-1);
        p1p0 = __builtin_ia32_lzcnt_u64(bbp1) - 39;
    }

    // P0 pawns skip over kings and P1 pawns
    p0p0 -= (p0p0 > p0k) + (p0p0 > p1k) + (p0p0 > p1p0) + (p0p0 > p1p1) + (p0p0 > p1p2) + (p0p0 > p1p3);
    p0p1 -= (p0p1 > p0k) + (p0p1 > p1k) + (p0p1 > p1p0) + (p0p1 > p1p1) + (p0p1 > p1p2) + (p0p1 > p1p3);
    p0p2 -= (p0p2 > p0k) + (p0p2 > p1k) + (p0p2 > p1p0) + (p0p2 > p1p1) + (p0p2 > p1p2) + (p0p2 > p1p3);
    p0p3 -= (p0p3 > p0k) + (p0p3 > p1k) + (p0p3 > p1p0) + (p0p3 > p1p1) + (p0p3 > p1p2) + (p0p3 > p1p3);

    // P1 pawns skip over kings
    p1p0 -= (p1p0 > p0k) + (p1p0 > p1k);
    p1p1 -= (p1p1 > p0k) + (p1p1 > p1k);
    p1p2 -= (p1p2 > p0k) + (p1p2 > p1k);
    p1p3 -= (p1p3 > p0k) + (p1p3 > p1k);

    return OFFSET_5V5 + (twoKingsIndex[p0k*25 + p1k] * (23*22*21*20/24)
        + p1p3 * (p1p3-1) * (p1p3-2) * (p1p3-3) / 24 + p1p2 * (p1p2-1) * (p1p2-2) / 6 + p1p1 * (p1p1-1) / 2 + p1p0) * (19*18*17*16/24)
        + p0p3 * (p0p3-1) * (p0p3-2) * (p0p3-3) / 24 + p0p2 * (p0p2-1) * (p0p2-2) / 6 + p0p1 * (p0p1-1) / 2 + p0p0;
}

template <bool invert, typename T>
void u5v5(U64 i, U64 cardIndex, T visitor) {
    U64 p0i = i % (19*18*17*16/24);
    i /= 19*18*17*16/24;
    U64 p0p0 = fourPawns[p0i*4];
    U64 p0p1 = fourPawns[p0i*4+1];
    U64 p0p2 = fourPawns[p0i*4+2];
    U64 p0p3 = fourPawns[p0i*4+3];

    U64 p1i = i % (23*22*21*20/24);
    i /= 23*22*21*20/24;
    U64 p1p0 = fourPawns[p1i*4];
    U64 p1p1 = fourPawns[p1i*4+1];
    U64 p1p2 = fourPawns[p1i*4+2];
    U64 p1p3 = fourPawns[p1i*4+3];

    U64 k0 = twoKings[i*2];
    U64 k1 = twoKings[i*2+1];

    // To "unskip" positions, we need to start from the lowest set bit and work upwards
    // P1 pawns skip over kings
    U64 material = (1ULL << (24-k0)) | (1ULL << (24-k1));
    U64 b = __builtin_ia32_tzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p1p0 += b <= p1p0; p1p1 += b <= p1p1; p1p2 += b <= p1p2; p1p3 += b <= p1p3;

    // P0 pawns skip over kings and P1 pawns
    material |= (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3);
    b = __builtin_ia32_tzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = __builtin_ia32_tzcnt_u64(material &= material-1);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;
    b = 63 - __builtin_ia32_lzcnt_u64(material);
    p0p0 += b <= p0p0; p0p1 += b <= p0p1; p0p2 += b <= p0p2; p0p3 += b <= p0p3;

    if (invert) {
        visitor(
            (1ULL << k1) | (1ULL << (24-p1p0)) | (1ULL << (24-p1p1)) | (1ULL << (24-p1p2)) | (1ULL << (24-p1p3)),
            (1ULL << k0) | (1ULL << (24-p0p0)) | (1ULL << (24-p0p1)) | (1ULL << (24-p0p2)) | (1ULL << (24-p0p3)),
            1ULL << k1,
            1ULL << k0,
            cardIndex);
    } else {
        visitor(
            (1ULL << (24-k0)) | (1ULL << p0p0) | (1ULL << p0p1) | (1ULL << p0p2) | (1ULL << p0p3),
            (1ULL << (24-k1)) | (1ULL << p1p0) | (1ULL << p1p1) | (1ULL << p1p2) | (1ULL << p1p3),
            1ULL << (24-k0),
            1ULL << (24-k1),
            cardIndex);
    }
}

void initializeIndexer() {
    // create two kings lookup table
    for (U64 k0 = 0; k0 < 25; ++k0) {
        if (k0 == PLAYER_0_WIN_POSITION) continue;
        for (U64 k1 = 0; k1 < 25; ++k1) {
            if (k1 == k0 || k1 == PLAYER_1_WIN_POSITION) continue;

            // P1 king skips P0 king, and both kings skip over their win positions
            U64 sk1 = k1 - (k1 > k0) - (k1 > PLAYER_1_WIN_POSITION && k0 != PLAYER_1_WIN_POSITION) + (k0 > PLAYER_1_WIN_POSITION);
            U64 sk0 = k0 - (k0 > PLAYER_0_WIN_POSITION);
            U64 i = sk0 * 23 + sk1;
            twoKingsIndex[k0*25 + k1] = i;

            twoKings[i*2] = 24-k0;
            twoKings[i*2+1] = 24-k1;
        }
    }

    // create two pawns colexicographical unrank lookup tables
    for (U64 p0 = 0; p0 < 22; ++p0) {
        for (U64 p1 = p0+1; p1 < 23; ++p1) {
            U64 i = p1 * (p1-1) / 2 + p0;
            twoPawns[i*2] = p0;
            twoPawns[i*2+1] = p1;
        }
    }

    // create three pawns colexicographical unrank lookup tables
    for (U64 p0 = 0; p0 < 21; ++p0) {
        for (U64 p1 = p0+1; p1 < 22; ++p1) {
            for (U64 p2 = p1+1; p2 < 23; ++p2) {
                U64 i = p2 * (p2-1) * (p2-2) / 6 + p1 * (p1-1) / 2 + p0;
                threePawns[i*3] = p0;
                threePawns[i*3+1] = p1;
                threePawns[i*3+2] = p2;
            }
        }
    }

    // create three pawns colexicographical unrank lookup tables
    for (U64 p0 = 0; p0 < 20; ++p0) {
        for (U64 p1 = p0+1; p1 < 21; ++p1) {
            for (U64 p2 = p1+1; p2 < 22; ++p2) {
                for (U64 p3 = p2+1; p3 < 23; ++p3) {
                    U64 i = p3 * (p3-1) * (p3-2) * (p3-3) / 24 + p2 * (p2-1) * (p2-2) / 6 + p1 * (p1-1) / 2 + p0;
                    fourPawns[i*4] = p0;
                    fourPawns[i*4+1] = p1;
                    fourPawns[i*4+2] = p2;
                    fourPawns[i*4+3] = p3;
                }
            }
        }
    }
}

template <typename T>
void visitAllPawnStates(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1, U64 pawnPlayer, T visitor) {
    U64 material = bbp0 | bbp1;
    U64 topPawnIdx = 63 - __builtin_ia32_lzcnt_u64(pawnPlayer == 0 ? bbp0 & ~bbk0 : bbp1 & ~bbk1);
    for (int p = topPawnIdx+1; p < 25; ++p) {
        if ((material & (1 << p)) != 0) continue; // don't add pawn on top of existing pieces

        U64 newBbp0 = bbp0, newBbp1 = bbp1;
        if (pawnPlayer == 0) newBbp0 |= 1 << p;
        else newBbp1 |= 1 << p;

        visitor(newBbp0, newBbp1, bbk0, bbk1);

        // avoid adding duplicate states
        U64 numP0Pawns = __builtin_popcountll(newBbp0) - __builtin_popcountll(bbk0);
        U64 numP1Pawns = __builtin_popcountll(newBbp1) - __builtin_popcountll(bbk1);
        if (numP0Pawns + numP1Pawns + 2 < TB_PIECES) {
            if (pawnPlayer == 0 && numP0Pawns < (TB_PIECES-1)/2)
                visitAllPawnStates(newBbp0, newBbp1, bbk0, bbk1, PLAYER_0, visitor);
            if (numP1Pawns < (TB_PIECES-1)/2)
                visitAllPawnStates(newBbp0, newBbp1, bbk0, bbk1, PLAYER_1, visitor);
        }
    }
}

// visitor arguments: bbp0, bbp1, bbk0, bbk1
template <typename T>
void visitAllStates(U64 numThreads, T visitor) {
    auto visitLambda = [numThreads, &visitor](U64 threadId) {
        U64 i1 = threadId * (24+23*23) / numThreads;
        U64 i2 = (threadId+1) * (24+23*23) / numThreads;

        for (U64 k = i1; k < i2; ++k) {
            U64 p0k = 24-twoKings[k*2];
            U64 p1k = 24-twoKings[k*2+1];

            U64 bbp0 = 1 << p0k;
            U64 bbp1 = 1 << p1k;
            U64 bbk0 = 1 << p0k;
            U64 bbk1 = 1 << p1k;

            visitor(bbp0, bbp1, bbk0, bbk1);

            for (U64 player = 0; player < 2; ++player)
                visitAllPawnStates(bbp0, bbp1, bbk0, bbk1, player, visitor);
        }
    };

    std::vector<std::thread> threads;
    for (U64 t = 0; t < numThreads; ++t)
        threads.push_back(std::thread(visitLambda, t));
    for (auto& thread : threads)
        thread.join();
}

U64 (*indexMethods[6][6])(U64, U64, U64, U64) = {
    {0, 0, 0, 0, 0, 0},
    {0, i1v1<false>, i1v2<false>, i1v3<false>, i1v4<false>, i1v5<false>},
    {0, i2v1<false>, i2v2<false>, i2v3<false>, i2v4<false>, i2v5<false>},
    {0, i3v1<false>, i3v2<false>, i3v3<false>, i3v4<false>, i3v5<false>},
    {0, i4v1<false>, i4v2<false>, i4v3<false>, i4v4<false>, i4v5<false>},
    {0, i5v1<false>, i5v2<false>, i5v3<false>, i5v4<false>, i5v5<false>}};

U64 (*invertedIndexMethods[6][6])(U64, U64, U64, U64) = {
    {0, 0, 0, 0, 0},
    {0, i1v1<true>, i2v1<true>, i3v1<true>, i4v1<true>, i5v1<true>},
    {0, i1v2<true>, i2v2<true>, i3v2<true>, i4v2<true>, i5v2<true>},
    {0, i1v3<true>, i2v3<true>, i3v3<true>, i4v3<true>, i5v3<true>},
    {0, i1v4<true>, i2v4<true>, i3v4<true>, i4v4<true>, i5v4<true>},
    {0, i1v5<true>, i2v5<true>, i3v5<true>, i4v5<true>, i5v5<true>}};

template <bool invert>
U64 indexForState(U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
    return invert
        ? invertedIndexMethods[__builtin_popcountll(bbp0)][__builtin_popcountll(bbp1)](bbp1, bbp0, bbk1, bbk0)
        : indexMethods[__builtin_popcountll(bbp0)][__builtin_popcountll(bbp1)](bbp0, bbp1, bbk0, bbk1);
}

// visitor arguments: bbp0, bbp1, bbk0, bbk1, cardIndex
template <bool invert, typename T>
void stateForIndex(U64 i, U64 cardIndex, T visitor) {
    #if TB_PIECES > 8
        if (i < OFFSET_5V4) return u5v5<invert>(i - OFFSET_5V5, cardIndex, visitor);
        if (i < OFFSET_4V5) return u5v4<invert>(i - OFFSET_5V4, cardIndex, visitor);
        if (i < OFFSET_5V3) return u4v5<invert>(i - OFFSET_4V5, cardIndex, visitor);
    #endif

    #if TB_PIECES > 8 || (TB_PIECES == 8 && INCLUDE_IMBALANCED_STATES)
        if (i < OFFSET_3V5) return u5v3<invert>(i - OFFSET_5V3, cardIndex, visitor);
        if (i < OFFSET_5V2) return u3v5<invert>(i - OFFSET_3V5, cardIndex, visitor);
        if (i < OFFSET_2V5) return u5v2<invert>(i - OFFSET_5V2, cardIndex, visitor);
        if (i < OFFSET_5V1) return u2v5<invert>(i - OFFSET_2V5, cardIndex, visitor);
        if (i < OFFSET_1V5) return u5v1<invert>(i - OFFSET_5V1, cardIndex, visitor);
        if (i < OFFSET_4V4) return u1v5<invert>(i - OFFSET_1V5, cardIndex, visitor);
    #endif

    #if TB_PIECES > 6
        if (i < OFFSET_4V3) return u4v4<invert>(i - OFFSET_4V4, cardIndex, visitor);
        if (i < OFFSET_3V4) return u4v3<invert>(i - OFFSET_4V3, cardIndex, visitor);
        if (i < OFFSET_4V2) return u3v4<invert>(i - OFFSET_3V4, cardIndex, visitor);
    #endif

    #if TB_PIECES == 6 && INCLUDE_IMBALANCED_STATES
        if (i < OFFSET_1V5) return u5v1<invert>(i - OFFSET_5V1, cardIndex, visitor);
        if (i < OFFSET_4V2) return u1v5<invert>(i - OFFSET_1V5, cardIndex, visitor);
    #endif

    #if TB_PIECES > 6 || (TB_PIECES == 6 && INCLUDE_IMBALANCED_STATES)
        if (i < OFFSET_2V4) return u4v2<invert>(i - OFFSET_4V2, cardIndex, visitor);
        if (i < OFFSET_4V1) return u2v4<invert>(i - OFFSET_2V4, cardIndex, visitor);
        if (i < OFFSET_1V4) return u4v1<invert>(i - OFFSET_4V1, cardIndex, visitor);
        if (i < OFFSET_3V3) return u1v4<invert>(i - OFFSET_1V4, cardIndex, visitor);
    #endif

    #if TB_PIECES > 4
        if (i < OFFSET_3V2) return u3v3<invert>(i - OFFSET_3V3, cardIndex, visitor);
        if (i < OFFSET_2V3) return u3v2<invert>(i - OFFSET_3V2, cardIndex, visitor);
        if (i < OFFSET_3V1) return u2v3<invert>(i - OFFSET_2V3, cardIndex, visitor);
    #endif

    #if TB_PIECES > 4 || (TB_PIECES == 4 && INCLUDE_IMBALANCED_STATES)
        if (i < OFFSET_1V3) return u3v1<invert>(i - OFFSET_3V1, cardIndex, visitor);
        if (i < OFFSET_2V2) return u1v3<invert>(i - OFFSET_1V3, cardIndex, visitor);
    #endif

    if (i < OFFSET_2V1) return u2v2<invert>(i - OFFSET_2V2, cardIndex, visitor);
    if (i < OFFSET_1V2) return u2v1<invert>(i - OFFSET_2V1, cardIndex, visitor);
    if (i < OFFSET_1V1) return u1v2<invert>(i - OFFSET_1V2, cardIndex, visitor);
    u1v1<invert>(i - OFFSET_1V1, cardIndex, visitor);
}

int testCompactIndexer() {
    initializeIndexer();

    U64 numThreads = 12;
    U64 numStates = OFFSET_1V1 + (24+23*23);

    for (U64 test = 0; test < 1; ++test) {
        auto startTime = std::chrono::high_resolution_clock::now();

        visitAllStates(numThreads, [](U64 bbp0, U64 bbp1, U64 bbk0, U64 bbk1) {
            U64 i = indexForState<true>(bbp0, bbp1, bbk0, bbk1);
            stateForIndex<true>(i, 0, [i, bbp0, bbp1, bbk0, bbk1](U64 ubbp0, U64 ubbp1, U64 ubbk0, U64 ubbk1, U64 cardIndex) {
                UNUSED(cardIndex);
                if (bbp0 != ubbp0 || bbp1 != ubbp1 || bbk0 != ubbk0 || bbk1 != ubbk1) {
                    printf("%" PRIx64 ": bbp0 = %" PRIx64 " / %" PRIx64 ", bbp1 = %" PRIx64 " / %" PRIx64 ", bbk0 = %" PRIx64 " / %" PRIx64 ", bbk1 = %" PRIx64 " / %" PRIx64 "\n", i, bbp0, ubbp0, bbp1, ubbp0, bbk0, ubbk0, bbk1, ubbk1);
                    exit(0);
                }});
        });

        double elapsedSecs = std::chrono::duration<double, std::ratio<1>>(std::chrono::high_resolution_clock::now() - startTime).count();
        printf("Indexed %" PRIu64 " states in %.3f s (%.2f ns / state)\n", numStates, elapsedSecs, elapsedSecs*1e9 / numStates);
    }

    return 0;
}
