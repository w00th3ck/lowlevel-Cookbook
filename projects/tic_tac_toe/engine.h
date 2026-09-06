#ifndef ENGINE_H
#define ENGINE_H

#include <stdint.h>

#define GRID1_MASK 0x000001FFU
#define GRID2_MASK 0x0003FE00U
#define GAME_DATA  0xFFFC0000U

#define BIT18      0x00200000U

#define CELL_1     0x00000001U
#define CELL_2     0x00000002U
#define CELL_3     0x00000004U
#define CELL_4     0x00000008U
#define CELL_5     0x00000010U
#define CELL_6     0x00000020U
#define CELL_7     0x00000040U
#define CELL_8     0x00000080U
#define CELL_9     0x00000100U

/**
 * ============================================================================
 * MOTOR ENGINE - TIC TAC TOE INTEGERS
 * ============================================================================
 * The entire game state is fully encapsulated within a single uint32_t register.
 *
 * BIT MAP MEMORY LAYOUT:
 * ----------------------------------------------------------------------------
 * Bits 00 to 08 : Player 1 grid layout (CELL_1 to CELL_9)
 * Bits 09 to 17 : Player 2 / AI grid layout (Shifted copy of CELL_1 to CELL_9)
 * Bit 18        : Game Status        (1 = Active playtime,   0 = Game Over)
 * Bit 19        : Active Player      (1 = Player 1,          0 = Player 2 / AI)
 * Bit 20        : Game Mode          (1 = Two players,       0 = Single player)
 * Bit 21        : Draw Status        (1 = Tie game,          0 = No draw)
 * Bit 22        : Input Validity     (1 = Successful move,   0 = Error/Occupied)
 * Bits 23 to 31 : Reserved / Unused
 * ============================================================================
 */

/**
 * @brief Software NOP (No Operation).
 * Safe-state function acting as a security buffer for unassigned inputs or index 0.
 */
static inline uint32_t do_nothing(uint32_t game){
    return game;
}

/**
 * @brief Branchless equality checker.
 * Compares two 32-bit registers without branching pipelines.
 * @return 1 if registers are strictly identical, 0 otherwise.
 */
static inline uint32_t is_equal(uint32_t a, uint32_t b){
    a ^= b;

    a |= a >> 1;
    a |= a >> 2;
    a |= a >> 4;
    a |= a >> 8;
    a |= a >> 16;

    return (~a & 1);
}

/**
 * @brief Toggles the active turn and physically swaps player grids.
 *
 * This is the core engine switch. Instead of using conditional branching to adapt
 * the winning logic to each player, this function mirrors the memory layout:
 * - Flips Bit 19 to switch the active player.
 * - Saves game metadata (Bits 18 to 31).
 * - Rotates Player 1 grid (Bits 0-8) into Player 2 slot (Bits 9-17) and vice-versa.
 */
static inline uint32_t update_game(uint32_t game){
    // 1. Toggle active player bit (Bit 19) via XOR inversion
    game ^= (1U << 19);
    // 2. Extract metadata, shift P2 grid down, shift P1 grid up, merge everything
    return (game & GAME_DATA) | ((game & GRID2_MASK) >> 9) | ((game & GRID1_MASK) << 9);
}

static inline uint32_t write_register(uint32_t success, uint32_t index, uint32_t game){
    // On écrit le pion seulement si success == 1
    game |= (success << index);
    // On nettoie d'abord le bit 22, puis on injecte le résultat
    game = (game & ~(1U << 22)) | (success << 22);
    // On stock le dernier valide coup joué
    return (game & ~(0xFU << 23)) | ((index & 0xF) << 23);
}

/**
 * @brief Evaluates board fullness to trigger a tie game.
 * Merges both player grids using a bitwise OR operation. If all 9 cells are marked
 * and the game status is active, switches Bit 21 (Draw) and shuts down Bit 18 (Game Over).
 */
static inline uint32_t check_draw(uint32_t game){
    // Combine P1 & P2 grids, mask the first 9 bits, compare to full board (0x1FF)
    uint32_t draw = is_equal(((game | (game >> 9)) & GRID1_MASK), GRID1_MASK);
    // If true and game is still running (Bit 18 is 1), flip Bit 21 to 1 (Draw)
    game ^= (draw & ((game >> 18) & 1U)) << 21;
    // If Bit 21 is active, toggle Bit 18 down to 0 to trigger Game Over
    game ^= (game & BIT18) >> 3;

    return game;
}

/**
 * @brief Evaluates winning conditions for Cell 1 (Top-Left).
 * Vectors checked: Row 1 (1-2-3), Col 1 (1-4-7), Diag 1 (1-5-9).
 *
 * Visual:   1 2 3
 *           4 5 .
 *           7 . 9
 */
static inline uint32_t check_cell_1(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_2) << 17) & ((game & CELL_3) << 16)) |
            (((game & CELL_1) << 18) & ((game & CELL_4) << 15) & ((game & CELL_7) << 12)) |
            (((game & CELL_1) << 18) & ((game & CELL_5) << 14) & ((game & CELL_9) << 10));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 2 (Top-Middle).
 * Vectors checked: Row 1 (1-2-3), Col 2 (2-5-8).
 *
 * Visual:   1 2 3
 *           . 5 .
 *           . 8 .
 */
static inline uint32_t check_cell_2(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_2) << 17) & ((game & CELL_3) << 16)) |
            (((game & CELL_2) << 17) & ((game & CELL_5) << 14) & ((game & CELL_8) << 11));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 3 (Top-Right).
 * Vectors checked: Row 1 (1-2-3), Col 3 (3-6-9), Diag 2 (3-5-7).
 *
 * Visual:   1 2 3
 *           . 5 6
 *           7 . 9
 */
static inline uint32_t check_cell_3(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_2) << 17) & ((game & CELL_3) << 16)) |
            (((game & CELL_3) << 16) & ((game & CELL_6) << 13) & ((game & CELL_9) << 10)) |
            (((game & CELL_3) << 16) & ((game & CELL_5) << 14) & ((game & CELL_7) << 12));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 4 (Middle-Left).
 * Vectors checked: Col 1 (1-4-7), Row 2 (4-5-6).
 *
 * Visual:   1 . .
 *           4 5 6
 *           7 . .
 */
static inline uint32_t check_cell_4(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_4) << 15) & ((game & CELL_7) << 12)) |
            (((game & CELL_4) << 15) & ((game & CELL_5) << 14) & ((game & CELL_6) << 13));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 5 (Center).
 * Vectors checked: Diag 1 (1-5-9), Col 2 (2-5-8), Diag 2 (3-5-7), Row 2 (4-5-6).
 *
 * Visual:   1 2 3
 *           4 5 6
 *           7 8 9
 */
static inline uint32_t check_cell_5(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_5) << 14) & ((game & CELL_9) << 10)) |
            (((game & CELL_2) << 17) & ((game & CELL_5) << 14) & ((game & CELL_8) << 11)) |
            (((game & CELL_3) << 16) & ((game & CELL_5) << 14) & ((game & CELL_7) << 12)) |
            (((game & CELL_4) << 15) & ((game & CELL_5) << 14) & ((game & CELL_6) << 13));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 6 (Middle-Right).
 * Vectors checked: Row 2 (4-5-6), Col 3 (3-6-9).
 *
 * Visual:   . . 3
 *           4 5 6
 *           . . 9
 */
static inline uint32_t check_cell_6(uint32_t game){
    game ^= (((game & CELL_3) << 16) & ((game & CELL_6) << 13) & ((game & CELL_9) << 10)) |
            (((game & CELL_4) << 15) & ((game & CELL_5) << 14) & ((game & CELL_6) << 13));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 7 (Bottom-Left).
 * Vectors checked: Col 1 (1-4-7), Diag 2 (3-5-7), Row 3 (7-8-9).
 *
 * Visual:   1 . 3
 *           4 5 .
 *           7 8 9
 */
static inline uint32_t check_cell_7(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_4) << 15) & ((game & CELL_7) << 12)) |
            (((game & CELL_3) << 16) & ((game & CELL_5) << 14) & ((game & CELL_7) << 12)) |
            (((game & CELL_7) << 12) & ((game & CELL_8) << 11) & ((game & CELL_9) << 10));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 8 (Bottom-Middle).
 * Vectors checked: Col 2 (2-5-8), Row 3 (7-8-9).
 *
 * Visual:   . 2 .
 *           . 5 .
 *           7 8 9
 */
static inline uint32_t check_cell_8(uint32_t game){
    game ^= (((game & CELL_2) << 17) & ((game & CELL_5) << 14) & ((game & CELL_8) << 11)) |
            (((game & CELL_7) << 12) & ((game & CELL_8) << 11) & ((game & CELL_9) << 10));
    return check_draw(game);
}

/**
 * @brief Evaluates winning conditions for Cell 9 (Bottom-Right).
 * Vectors checked: Diag 1 (1-5-9), Col 3 (3-6-9), Row 3 (7-8-9).
 *
 * Visual:   1 . 3
 *           . 5 6
 *           7 8 9
 */
static inline uint32_t check_cell_9(uint32_t game){
    game ^= (((game & CELL_1) << 18) & ((game & CELL_5) << 14) & ((game & CELL_9) << 10)) |
            (((game & CELL_3) << 16) & ((game & CELL_6) << 13) & ((game & CELL_9) << 10)) |
            (((game & CELL_7) << 12) & ((game & CELL_8) << 11) & ((game & CELL_9) << 10));
    return check_draw(game);
}

#endif
