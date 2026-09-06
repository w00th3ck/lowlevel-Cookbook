#include "ai.h"

uint64_t accumulate_cell_1(uint32_t *current_game){
    uint64_t accumulator = 0;

    // Calculate the initial cell 1 weight
    // On ne compte que les coups encore jouable sur cette case
    accumulator = __builtin_popcountll(((*current_game | (*current_game >> 9)) & 0x000001FFU) ^ 0x0000015EU);

    // Victories bonus
    accumulator += (((*current_game & 0x000001FFU) >> 2) & ((*current_game & 0x000001FFU) >> 1)) << 2;
    accumulator += (((*current_game & 0x000001FFU) >> 8) & ((*current_game & 0x000001FFU) >> 4)) << 2;
    accumulator += (((*current_game & 0x000001FFU) >> 6) & ((*current_game & 0x000001FFU) >> 3)) << 2;

    // Defenses bonus
    accumulator += ((((*current_game >> 9) & 0x000001FFU) >> 2) & (((*current_game >> 9) & 0x000001FFU) >> 1)) << 2;
    accumulator += ((((*current_game >> 9) & 0x000001FFU) >> 8) & (((*current_game >> 9) & 0x000001FFU) >> 4)) << 2;
    accumulator += ((((*current_game >> 9) & 0x000001FFU) >> 6) & (((*current_game >> 9) & 0x000001FFU) >> 3)) << 2;

    return (accumulator << 0);
}

uint64_t accumulate_cell_2(uint32_t *current_game){
    uint64_t accumulator = 0;

    // Calculate the initial cell 1 weight
    // On ne compte que les coups encore jouable sur cette case
    accumulator = __builtin_popcountll(((*current_game | (*current_game >> 9)) & 0x000001FFU) ^ 0x00000095U);

    // Victories bonus
    accumulator += (((*current_game & 0x000001FFU) >> 2) & ((*current_game & 0x000001FFU) >> 0)) << 2;
    accumulator += (((*current_game & 0x000001FFU) >> 7) & ((*current_game & 0x000001FFU) >> 4)) << 2;

    // Defenses bonus
    accumulator += ((((*current_game >> 9) & 0x000001FFU) >> 2) & (((*current_game >> 9) & 0x000001FFU) >> 0)) << 2;
    accumulator += ((((*current_game >> 9) & 0x000001FFU) >> 7) & (((*current_game >> 9) & 0x000001FFU) >> 4)) << 2;

    return (accumulator << 4);
}
