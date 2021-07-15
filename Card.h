#pragma once

#include <algorithm>
#include <array>
#include <vector>

#include "Common.h"

#define NUM_CARDS 16

#define BOAR        0
#define COBRA       1
#define CRAB        2
#define CRANE       3
#define DRAGON      4
#define EEL         5
#define ELEPHANT    6
#define FROG        7
#define GOOSE       8
#define HORSE       9
#define MANTIS      10
#define MONKEY      11
#define OX          12
#define RABBIT      13
#define ROOSTER     14
#define TIGER       15

U64 MIRRORED_CARD[NUM_CARDS] = {
    BOAR,       // BOAR
    EEL,        // COBRA
    CRAB,       // CRAB
    CRANE,      // CRANE
    DRAGON,     // DRAGON
    COBRA,      // EEL
    ELEPHANT,   // ELEPHANT
    RABBIT,     // FROG
    ROOSTER,    // GOOSE
    OX,         // HORSE
    MANTIS,     // MANTIS
    MONKEY,     // MONKEY
    HORSE,      // OX
    FROG,       // RABBIT
    GOOSE,      // ROOSTER
    TIGER};     // TIGER

std::string CARD_NAMES[] = {"Boar", "Cobra", "Crab", "Crane", "Dragon", "Eel", "Elephant", "Frog", "Goose", "Horse", "Mantis", "Monkey", "Ox", "Rabbit", "Rooster", "Tiger"};

// Move bitmask for each card / player / board position. Created by the Java method Card.printAllCardMoveBitmasks
U64 MOVE_BITMASK[NUM_CARDS][NUM_PLAYERS][25] = {
    {{2, 5, 10, 20, 8, 65, 162, 324, 648, 272, 2080, 5184, 10368, 20736, 8704, 66560, 165888, 331776, 663552, 278528, 2129920, 5308416, 10616832, 21233664, 8912896},
    {34, 69, 138, 276, 520, 1088, 2208, 4416, 8832, 16640, 34816, 70656, 141312, 282624, 532480, 1114112, 2260992, 4521984, 9043968, 17039360, 2097152, 5242880, 10485760, 20971520, 8388608}},
    {{64, 129, 258, 516, 8, 2050, 4132, 8264, 16528, 256, 65600, 132224, 264448, 528896, 8192, 2099200, 4231168, 8462336, 16924672, 262144, 65536, 1179648, 2359296, 4718592, 8388608},
    {2, 36, 72, 144, 256, 64, 1153, 2306, 4612, 8200, 2048, 36896, 73792, 147584, 262400, 65536, 1180672, 2361344, 4722688, 8396800, 2097152, 4227072, 8454144, 16908288, 262144}},
    {{4, 8, 17, 2, 4, 129, 258, 548, 72, 144, 4128, 8256, 17536, 2304, 4608, 132096, 264192, 561152, 73728, 147456, 4227072, 8454144, 17956864, 2359296, 4718592},
    {36, 72, 145, 258, 516, 1152, 2304, 4640, 8256, 16512, 36864, 73728, 148480, 264192, 528384, 1179648, 2359296, 4751360, 8454144, 16908288, 4194304, 8388608, 17825792, 2097152, 4194304}},
    {{64, 160, 320, 640, 256, 2049, 5122, 10244, 20488, 8208, 65568, 163904, 327808, 655616, 262656, 2098176, 5244928, 10489856, 20979712, 8404992, 32768, 65536, 131072, 262144, 524288},
    {32, 64, 128, 256, 512, 1026, 2053, 4106, 8212, 16392, 32832, 65696, 131392, 262784, 524544, 1050624, 2102272, 4204544, 8409088, 16785408, 65536, 163840, 327680, 655360, 262144}},
    {{64, 160, 320, 640, 256, 2052, 5128, 10257, 20482, 8196, 65664, 164096, 328224, 655424, 262272, 2101248, 5251072, 10503168, 20973568, 8392704, 131072, 262144, 557056, 65536, 131072},
    {128, 256, 544, 64, 128, 4098, 8197, 17418, 2068, 4104, 131136, 262304, 557376, 66176, 131328, 4196352, 8393728, 17836032, 2117632, 4202496, 65536, 163840, 327680, 655360, 262144}},
    {{2, 36, 72, 144, 256, 64, 1153, 2306, 4612, 8200, 2048, 36896, 73792, 147584, 262400, 65536, 1180672, 2361344, 4722688, 8396800, 2097152, 4227072, 8454144, 16908288, 262144},
    {64, 129, 258, 516, 8, 2050, 4132, 8264, 16528, 256, 65600, 132224, 264448, 528896, 8192, 2099200, 4231168, 8462336, 16924672, 262144, 65536, 1179648, 2359296, 4718592, 8388608}},
    {{2, 5, 10, 20, 8, 66, 165, 330, 660, 264, 2112, 5280, 10560, 21120, 8448, 67584, 168960, 337920, 675840, 270336, 2162688, 5406720, 10813440, 21626880, 8650752},
    {66, 165, 330, 660, 264, 2112, 5280, 10560, 21120, 8448, 67584, 168960, 337920, 675840, 270336, 2162688, 5406720, 10813440, 21626880, 8650752, 2097152, 5242880, 10485760, 20971520, 8388608}},
    {{64, 128, 257, 514, 4, 2048, 4097, 8226, 16452, 136, 65536, 131104, 263232, 526464, 4352, 2097152, 4195328, 8423424, 16846848, 139264, 0, 32768, 1114112, 2228224, 4456448},
    {68, 136, 272, 512, 0, 2176, 4353, 8706, 16388, 8, 69632, 139296, 278592, 524416, 256, 2228224, 4457472, 8914944, 16781312, 8192, 4194304, 8421376, 16842752, 131072, 262144}},
    {{66, 133, 266, 532, 8, 2112, 4257, 8514, 17028, 264, 67584, 136224, 272448, 544896, 8448, 2162688, 4359168, 8718336, 17436672, 270336, 2097152, 5275648, 10551296, 21102592, 8650752},
    {66, 133, 266, 532, 8, 2112, 4257, 8514, 17028, 264, 67584, 136224, 272448, 544896, 8448, 2162688, 4359168, 8718336, 17436672, 270336, 2097152, 5275648, 10551296, 21102592, 8650752}},
    {{32, 65, 130, 260, 520, 1025, 2082, 4164, 8328, 16656, 32800, 66624, 133248, 266496, 532992, 1049600, 2131968, 4263936, 8527872, 17055744, 32768, 1114112, 2228224, 4456448, 8912896},
    {34, 68, 136, 272, 512, 1089, 2178, 4356, 8712, 16400, 34848, 69696, 139392, 278784, 524800, 1115136, 2230272, 4460544, 8921088, 16793600, 2129920, 4259840, 8519680, 17039360, 524288}},
    {{32, 64, 128, 256, 512, 1026, 2053, 4106, 8212, 16392, 32832, 65696, 131392, 262784, 524544, 1050624, 2102272, 4204544, 8409088, 16785408, 65536, 163840, 327680, 655360, 262144},
    {64, 160, 320, 640, 256, 2049, 5122, 10244, 20488, 8208, 65568, 163904, 327808, 655616, 262656, 2098176, 5244928, 10489856, 20979712, 8404992, 32768, 65536, 131072, 262144, 524288}},
    {{64, 160, 320, 640, 256, 2050, 5125, 10250, 20500, 8200, 65600, 164000, 328000, 656000, 262400, 2099200, 5248000, 10496000, 20992000, 8396800, 65536, 163840, 327680, 655360, 262144},
    {64, 160, 320, 640, 256, 2050, 5125, 10250, 20500, 8200, 65600, 164000, 328000, 656000, 262400, 2099200, 5248000, 10496000, 20992000, 8396800, 65536, 163840, 327680, 655360, 262144}},
    {{34, 68, 136, 272, 512, 1089, 2178, 4356, 8712, 16400, 34848, 69696, 139392, 278784, 524800, 1115136, 2230272, 4460544, 8921088, 16793600, 2129920, 4259840, 8519680, 17039360, 524288},
    {32, 65, 130, 260, 520, 1025, 2082, 4164, 8328, 16656, 32800, 66624, 133248, 266496, 532992, 1049600, 2131968, 4263936, 8527872, 17055744, 32768, 1114112, 2228224, 4456448, 8912896}},
    {{4, 40, 80, 128, 256, 130, 1284, 2568, 4112, 8192, 4160, 41088, 82176, 131584, 262144, 133120, 1314816, 2629632, 4210688, 8388608, 4259840, 8519680, 17039360, 524288, 0},
    {0, 32, 65, 130, 260, 2, 1028, 2088, 4176, 8320, 64, 32896, 66816, 133632, 266240, 2048, 1052672, 2138112, 4276224, 8519680, 65536, 131072, 1310720, 2621440, 4194304}},
    {{2, 37, 74, 148, 264, 66, 1188, 2376, 4752, 8448, 2112, 38016, 76032, 152064, 270336, 67584, 1216512, 2433024, 4866048, 8650752, 2162688, 5373952, 10747904, 21495808, 8388608},
    {2, 37, 74, 148, 264, 66, 1188, 2376, 4752, 8448, 2112, 38016, 76032, 152064, 270336, 67584, 1216512, 2433024, 4866048, 8650752, 2162688, 5373952, 10747904, 21495808, 8388608}},
    {{32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32769, 65538, 131076, 262152, 524304, 1048608, 2097216, 4194432, 8388864, 16777728, 1024, 2048, 4096, 8192, 16384},
    {1024, 2048, 4096, 8192, 16384, 32769, 65538, 131076, 262152, 524304, 1048608, 2097216, 4194432, 8388864, 16777728, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288}}};

// Same as above, but reordered dimensions. Could be more cache efficient, depending on access pattern.
U64 MOVE_BITMASK_PCP[NUM_PLAYERS][NUM_CARDS][25] =
    {{{2, 5, 10, 20, 8, 65, 162, 324, 648, 272, 2080, 5184, 10368, 20736, 8704, 66560, 165888, 331776, 663552, 278528, 2129920, 5308416, 10616832, 21233664, 8912896},
    {64, 129, 258, 516, 8, 2050, 4132, 8264, 16528, 256, 65600, 132224, 264448, 528896, 8192, 2099200, 4231168, 8462336, 16924672, 262144, 65536, 1179648, 2359296, 4718592, 8388608},
    {4, 8, 17, 2, 4, 129, 258, 548, 72, 144, 4128, 8256, 17536, 2304, 4608, 132096, 264192, 561152, 73728, 147456, 4227072, 8454144, 17956864, 2359296, 4718592},
    {64, 160, 320, 640, 256, 2049, 5122, 10244, 20488, 8208, 65568, 163904, 327808, 655616, 262656, 2098176, 5244928, 10489856, 20979712, 8404992, 32768, 65536, 131072, 262144, 524288},
    {64, 160, 320, 640, 256, 2052, 5128, 10257, 20482, 8196, 65664, 164096, 328224, 655424, 262272, 2101248, 5251072, 10503168, 20973568, 8392704, 131072, 262144, 557056, 65536, 131072},
    {2, 36, 72, 144, 256, 64, 1153, 2306, 4612, 8200, 2048, 36896, 73792, 147584, 262400, 65536, 1180672, 2361344, 4722688, 8396800, 2097152, 4227072, 8454144, 16908288, 262144},
    {2, 5, 10, 20, 8, 66, 165, 330, 660, 264, 2112, 5280, 10560, 21120, 8448, 67584, 168960, 337920, 675840, 270336, 2162688, 5406720, 10813440, 21626880, 8650752},
    {64, 128, 257, 514, 4, 2048, 4097, 8226, 16452, 136, 65536, 131104, 263232, 526464, 4352, 2097152, 4195328, 8423424, 16846848, 139264, 0, 32768, 1114112, 2228224, 4456448},
    {66, 133, 266, 532, 8, 2112, 4257, 8514, 17028, 264, 67584, 136224, 272448, 544896, 8448, 2162688, 4359168, 8718336, 17436672, 270336, 2097152, 5275648, 10551296, 21102592, 8650752},
    {32, 65, 130, 260, 520, 1025, 2082, 4164, 8328, 16656, 32800, 66624, 133248, 266496, 532992, 1049600, 2131968, 4263936, 8527872, 17055744, 32768, 1114112, 2228224, 4456448, 8912896},
    {32, 64, 128, 256, 512, 1026, 2053, 4106, 8212, 16392, 32832, 65696, 131392, 262784, 524544, 1050624, 2102272, 4204544, 8409088, 16785408, 65536, 163840, 327680, 655360, 262144},
    {64, 160, 320, 640, 256, 2050, 5125, 10250, 20500, 8200, 65600, 164000, 328000, 656000, 262400, 2099200, 5248000, 10496000, 20992000, 8396800, 65536, 163840, 327680, 655360, 262144},
    {34, 68, 136, 272, 512, 1089, 2178, 4356, 8712, 16400, 34848, 69696, 139392, 278784, 524800, 1115136, 2230272, 4460544, 8921088, 16793600, 2129920, 4259840, 8519680, 17039360, 524288},
    {4, 40, 80, 128, 256, 130, 1284, 2568, 4112, 8192, 4160, 41088, 82176, 131584, 262144, 133120, 1314816, 2629632, 4210688, 8388608, 4259840, 8519680, 17039360, 524288, 0},
    {2, 37, 74, 148, 264, 66, 1188, 2376, 4752, 8448, 2112, 38016, 76032, 152064, 270336, 67584, 1216512, 2433024, 4866048, 8650752, 2162688, 5373952, 10747904, 21495808, 8388608},
    {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32769, 65538, 131076, 262152, 524304, 1048608, 2097216, 4194432, 8388864, 16777728, 1024, 2048, 4096, 8192, 16384}},
    {{34, 69, 138, 276, 520, 1088, 2208, 4416, 8832, 16640, 34816, 70656, 141312, 282624, 532480, 1114112, 2260992, 4521984, 9043968, 17039360, 2097152, 5242880, 10485760, 20971520, 8388608},
    {2, 36, 72, 144, 256, 64, 1153, 2306, 4612, 8200, 2048, 36896, 73792, 147584, 262400, 65536, 1180672, 2361344, 4722688, 8396800, 2097152, 4227072, 8454144, 16908288, 262144},
    {36, 72, 145, 258, 516, 1152, 2304, 4640, 8256, 16512, 36864, 73728, 148480, 264192, 528384, 1179648, 2359296, 4751360, 8454144, 16908288, 4194304, 8388608, 17825792, 2097152, 4194304},
    {32, 64, 128, 256, 512, 1026, 2053, 4106, 8212, 16392, 32832, 65696, 131392, 262784, 524544, 1050624, 2102272, 4204544, 8409088, 16785408, 65536, 163840, 327680, 655360, 262144},
    {128, 256, 544, 64, 128, 4098, 8197, 17418, 2068, 4104, 131136, 262304, 557376, 66176, 131328, 4196352, 8393728, 17836032, 2117632, 4202496, 65536, 163840, 327680, 655360, 262144},
    {64, 129, 258, 516, 8, 2050, 4132, 8264, 16528, 256, 65600, 132224, 264448, 528896, 8192, 2099200, 4231168, 8462336, 16924672, 262144, 65536, 1179648, 2359296, 4718592, 8388608},
    {66, 165, 330, 660, 264, 2112, 5280, 10560, 21120, 8448, 67584, 168960, 337920, 675840, 270336, 2162688, 5406720, 10813440, 21626880, 8650752, 2097152, 5242880, 10485760, 20971520, 8388608},
    {68, 136, 272, 512, 0, 2176, 4353, 8706, 16388, 8, 69632, 139296, 278592, 524416, 256, 2228224, 4457472, 8914944, 16781312, 8192, 4194304, 8421376, 16842752, 131072, 262144},
    {66, 133, 266, 532, 8, 2112, 4257, 8514, 17028, 264, 67584, 136224, 272448, 544896, 8448, 2162688, 4359168, 8718336, 17436672, 270336, 2097152, 5275648, 10551296, 21102592, 8650752},
    {34, 68, 136, 272, 512, 1089, 2178, 4356, 8712, 16400, 34848, 69696, 139392, 278784, 524800, 1115136, 2230272, 4460544, 8921088, 16793600, 2129920, 4259840, 8519680, 17039360, 524288},
    {64, 160, 320, 640, 256, 2049, 5122, 10244, 20488, 8208, 65568, 163904, 327808, 655616, 262656, 2098176, 5244928, 10489856, 20979712, 8404992, 32768, 65536, 131072, 262144, 524288},
    {64, 160, 320, 640, 256, 2050, 5125, 10250, 20500, 8200, 65600, 164000, 328000, 656000, 262400, 2099200, 5248000, 10496000, 20992000, 8396800, 65536, 163840, 327680, 655360, 262144},
    {32, 65, 130, 260, 520, 1025, 2082, 4164, 8328, 16656, 32800, 66624, 133248, 266496, 532992, 1049600, 2131968, 4263936, 8527872, 17055744, 32768, 1114112, 2228224, 4456448, 8912896},
    {0, 32, 65, 130, 260, 2, 1028, 2088, 4176, 8320, 64, 32896, 66816, 133632, 266240, 2048, 1052672, 2138112, 4276224, 8519680, 65536, 131072, 1310720, 2621440, 4194304},
    {2, 37, 74, 148, 264, 66, 1188, 2376, 4752, 8448, 2112, 38016, 76032, 152064, 270336, 67584, 1216512, 2433024, 4866048, 8650752, 2162688, 5373952, 10747904, 21495808, 8388608},
    {1024, 2048, 4096, 8192, 16384, 32769, 65538, 131076, 262152, 524304, 1048608, 2097216, 4194432, 8388864, 16777728, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144, 524288}}};

// The list of 30 combinations of 5 cards.
U64 CARD_COMBINATIONS[NUM_CARD_COMBINATIONS][NUM_CARDS_PER_GAME] = {
    {0, 1, 2, 3, 4},
    {0, 1, 3, 2, 4},
    {0, 1, 4, 2, 3},
    {0, 2, 3, 1, 4},
    {0, 2, 4, 1, 3},
    {0, 3, 4, 1, 2},
    {1, 0, 2, 3, 4},
    {1, 0, 3, 2, 4},
    {1, 0, 4, 2, 3},
    {1, 2, 3, 0, 4},
    {1, 2, 4, 0, 3},
    {1, 3, 4, 0, 2},
    {2, 0, 1, 3, 4},
    {2, 0, 3, 1, 4},
    {2, 0, 4, 1, 3},
    {2, 1, 3, 0, 4},
    {2, 1, 4, 0, 3},
    {2, 3, 4, 0, 1},
    {3, 0, 1, 2, 4},
    {3, 0, 2, 1, 4},
    {3, 0, 4, 1, 2},
    {3, 1, 2, 0, 4},
    {3, 1, 4, 0, 2},
    {3, 2, 4, 0, 1},
    {4, 0, 1, 2, 3},
    {4, 0, 2, 1, 3},
    {4, 0, 3, 1, 2},
    {4, 1, 2, 0, 3},
    {4, 1, 3, 0, 2},
    {4, 2, 3, 0, 1}};

// Card indices of the two possible next / previous moves for the given player (next card swapped with the player's
// two cards). (Since the swap action is the same, this can be used for both next / previous moves.)
U64 NEXT_CARD_INDEX[NUM_CARD_COMBINATIONS][NUM_PLAYERS][NUM_CARDS_PER_HAND] = {
    {{6, 12}, {21, 27}},
    {{7, 18}, {15, 28}},
    {{8, 24}, {16, 22}},
    {{13, 19}, {9, 29}},
    {{14, 25}, {10, 23}},
    {{20, 26}, {11, 17}},
    {{0, 12}, {19, 25}},
    {{1, 18}, {13, 26}},
    {{2, 24}, {14, 20}},
    {{15, 21}, {3, 29}},
    {{16, 27}, {4, 23}},
    {{22, 28}, {5, 17}},
    {{0, 6}, {18, 24}},
    {{3, 19}, {7, 26}},
    {{4, 25}, {8, 20}},
    {{9, 21}, {1, 28}},
    {{10, 27}, {2, 22}},
    {{23, 29}, {5, 11}},
    {{1, 7}, {12, 24}},
    {{3, 13}, {6, 25}},
    {{5, 26}, {8, 14}},
    {{9, 15}, {0, 27}},
    {{11, 28}, {2, 16}},
    {{17, 29}, {4, 10}},
    {{2, 8}, {12, 18}},
    {{4, 14}, {6, 19}},
    {{5, 20}, {7, 13}},
    {{10, 16}, {0, 21}},
    {{11, 22}, {1, 15}},
    {{17, 23}, {3, 9}}};

#define NUM_CARD_SETS (16*15*14*13*12 / 120) // 4368

// Maps each co-lexicographically ranked combination of 5 cards (0 - 4367) to a symmetrically unique
// index (0 - 2347). Negative entries are duplicates of the combination `-entry-1` (-1 - -2348).
S64 symmetricallyUniqueCardCombinations[NUM_CARD_SETS];

// The 4368 sets of 5 cards.
std::array<std::array<U64, 5>, NUM_CARD_SETS> allCardSets;

// The 2348 symmetrically unique sets of 5 cards.
std::vector<std::array<U64, 5>> uniqueCardSets;

U64 coLex(U64 a, U64 b, U64 c, U64 d, U64 e) {
    return a + b*(b-1)/2 + c*(c-1)*(c-2)/6 + d*(d-1)*(d-2)*(d-3)/24 + e*(e-1)*(e-2)*(e-3)*(e-4)/120;
};

void generateSymmetricallyUniqueCardCombinationTable() {
    for (U64 a = 0; a < NUM_CARDS; ++a) {
        for (U64 b = a+1; b < NUM_CARDS; ++b) {
            for (U64 c = b+1; c < NUM_CARDS; ++c) {
                for (U64 d = c+1; d < NUM_CARDS; ++d) {
                    for (U64 e = d+1; e < NUM_CARDS; ++e) {
                        // How to detect duplicates: Create the mirrored set of cards. If the mirrored combination
                        // is co-lexicographically before the original combination, consider it a duplicate.
                        U64 cardCombination = coLex(a, b, c, d, e);
                        U64 m[] = {MIRRORED_CARD[a], MIRRORED_CARD[b], MIRRORED_CARD[c], MIRRORED_CARD[d], MIRRORED_CARD[e]};
                        std::sort(m, m+5);
                        U64 mirroredCardCombination = coLex(m[0], m[1], m[2], m[3], m[4]);

                        allCardSets[cardCombination] = {a, b, c, d, e};
                        if (mirroredCardCombination < cardCombination) {
                            symmetricallyUniqueCardCombinations[cardCombination] = -mirroredCardCombination-1;
                        } else {
                            symmetricallyUniqueCardCombinations[cardCombination] = uniqueCardSets.size();
                            uniqueCardSets.push_back({a, b, c, d, e});
                        }
                    }
                }
            }
        }
    }

    // Mark all duplicates with the unique index they're a duplicate of
    for (U64 n = 0; n < NUM_CARD_SETS; ++n) {
        S64 i = symmetricallyUniqueCardCombinations[n];
        symmetricallyUniqueCardCombinations[n] = i >= 0 ? i : -symmetricallyUniqueCardCombinations[-i-1]-1;
    }
}