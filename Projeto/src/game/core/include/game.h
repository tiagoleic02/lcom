#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include "../../objects/include/piece.h"
#include "input.h"

extern int mouse_action_cooldown;

typedef struct {
    Piece current_piece;
    Piece next_pieces[3]; // Array to hold next pieces
    int drop_timer;
    int current_drop_speed;
    bool game_over;
    bool paused;
    int pause_option;
} GameLogic;

// Pure game logic - sem input ou rendering
void game_logic_init(GameLogic* game);
void game_logic_update(GameLogic* game);
bool game_logic_move_piece(GameLogic* game, int deltaX, int deltaY);
void game_logic_rotate_piece(GameLogic* game, bool clockwise);
void game_logic_drop_piece(GameLogic* game);
Piece game_logic_calculate_ghost_piece(const GameLogic* game);
void game_logic_fix_piece(GameLogic* game);
int game_logic_clear_lines(GameLogic* game);
void game_logic_spawn_piece(GameLogic* game);
bool game_logic_is_game_over(const GameLogic* game);
void game_logic_handle_input(GameLogic* game, InputEvent event);
void game_logic_render(const GameLogic* game);
void game_logic_update_speed(GameLogic* game);
void game_logic_toggle_pause(GameLogic* game);
void game_logic_handle_pause_input(GameLogic* game, InputEvent event);
bool game_logic_is_paused(const GameLogic* game);

#endif
