#include "include/game.h"
#include "../ui/include/game_ui.h"
#include "../states/include/state.h"
#include "../ui/include/pause_ui.h"
#include "../ui/include/render.h"
#include "../objects/include/score.h"
#include "../core/include/tetris.h"
#include "../ui/include/font.h"

int mouse_action_cooldown = 0;

// ===== INICIALIZAÇÃO =====
void game_logic_init(GameLogic* game) {
    if (!game) return;

    game->drop_timer = 0;
    game->current_drop_speed = DROP_SPEED_INITIAL;
    game->game_over = false;
    game->paused = false;
    game->pause_option = 0;

    tetris_init();

    // Gerar as próximas 3 peças
    for (int i = 0; i < 3; i++) {
        piece_init(&game->next_pieces[i], random_piece_type(), 3, 0);
    }

    // Spawnar a primeira peça
    game_logic_spawn_piece(game);
}

// ===== UPDATE LOOP =====
void game_logic_update(GameLogic* game) {
    if (!game || game->game_over || game->paused) return;

    if (mouse_action_cooldown > 0)
        mouse_action_cooldown--;

    game->drop_timer++;
    if (game->drop_timer >= game->current_drop_speed) {
        game->drop_timer = 0;
        game_logic_drop_piece(game);
        needs_redraw = true;
    }
}

// ===== MOVIMENTO DAS PEÇAS =====
bool game_logic_move_piece(GameLogic* game, const int deltaX, const int deltaY) {
    if (!game || game->game_over || game->paused) return false;

    Piece* piece = &game->current_piece;

    if (piece_fits(piece, piece->x + deltaX, piece->y + deltaY)) {
        piece->x += deltaX;
        piece->y += deltaY;
        return true;
    }
    return false;
}

void game_logic_rotate_piece(GameLogic* game, const bool clockwise) {
    if (!game || game->game_over) return;

    Piece* piece = &game->current_piece;
    const int old_rotation = piece->rotation;

    piece_rotate(piece, clockwise);

    if (!piece_fits(piece, piece->x, piece->y)) {
        // Revert position if doesnt fit
        piece->rotation = old_rotation;
        piece_update_shape(piece);
    }
}

void game_logic_drop_piece(GameLogic* game) {
    if (!game || game->game_over || game->paused) return;

    if (!game_logic_move_piece(game, 0, 1)) {
        game_logic_fix_piece(game);

        const int lines = game_logic_clear_lines(game);
        if (lines > 0) {
            GameScore* score = tetris_get_score();
            scoreAddLines(score, lines);
            game_logic_update_speed(game);
        }
        game_logic_spawn_piece(game);
    }
}

Piece game_logic_calculate_ghost_piece(const GameLogic* game) {
    if (!game || game->game_over || game->paused) {
        return game->current_piece;
    }
    Piece ghost = game->current_piece;
    while (piece_fits(&ghost, ghost.x, ghost.y + 1)) {
        ghost.y++;
    }
    return ghost;
}

// ===== GESTÃO DA GRELHA =====
void game_logic_fix_piece(GameLogic* game) {
    if (!game) return;
    fix_piece_to_grid(&game->current_piece);
}

int game_logic_clear_lines(GameLogic* game) {
    if (!game) return 0;
    game_logic_update_speed(game);
    return clear_full_lines();
}

void game_logic_spawn_piece(GameLogic* game) {
    if (!game) return;

    // A peça atual passa a ser a primeira próxima peça
    game->current_piece = game->next_pieces[0];
    game->current_piece.x = 3;
    game->current_piece.y = 0;

    // "Shift" as peças: a 2ª vira 1ª, a 3ª vira 2ª
    game->next_pieces[0] = game->next_pieces[1];
    game->next_pieces[1] = game->next_pieces[2];

    // Gerar nova terceira peça
    piece_init(&game->next_pieces[2], random_piece_type(), 3, 0);

    if (!piece_fits(&game->current_piece, game->current_piece.x, game->current_piece.y)) {
        game->game_over = true;
    }
}

// ===== SISTEMA DE PAUSE =====
void game_logic_toggle_pause(GameLogic* game) {
    if (!game || game->game_over) return;

    game->paused = !game->paused;
    if (game->paused) {
        game->pause_option = 0;
    }
    needs_redraw = true;
}

void game_logic_handle_pause_input(GameLogic* game, const InputEvent event) {
    if (!game || !game->paused) return;

    switch (event.action) {
        case INPUT_UP:
        case INPUT_DOWN:
            game->pause_option = (game->pause_option + 1) % 2;
            needs_redraw = true;
            break;
        case INPUT_ENTER:
            if (game->pause_option == 0) {
                game->paused = false;  // Back to Game
            } else {
                game->game_over = true;  // Back to Menu
            }
            needs_redraw = true;
            break;
        case INPUT_ESCAPE:
        case INPUT_Q:
            game->paused = false;
            needs_redraw = true;
            break;
        default:
            break;
    }
}

bool game_logic_is_paused(const GameLogic* game) {
    return game && game->paused;
}

// ===== INPUT HANDLING =====
void game_logic_handle_input(GameLogic* game, const InputEvent event) {
    if (!game || game->game_over || !event.pressed) return;

    // Se pausado, usar handler específico
    if (game->paused) {
        game_logic_handle_pause_input(game, event);
        return;
    }

    // Verificar teclas de pause
    if (event.action == INPUT_ESCAPE || event.action == INPUT_Q) {
        game_logic_toggle_pause(game);
        return;
    }

    // Ignore mouse events if in cooldown
    if (event.type == INPUT_TYPE_MOUSE && mouse_action_cooldown > 0) return;


    bool moved = false;
    switch (event.action) {
        case INPUT_LEFT:
            moved = game_logic_move_piece(game, -1, 0);
            break;
        case INPUT_RIGHT:
            moved = game_logic_move_piece(game, 1, 0);
            break;
        case INPUT_DOWN:
            moved = game_logic_move_piece(game, 0, 1);
            break;
        case INPUT_ROTATE_LEFT:
            game_logic_rotate_piece(game, false);
            if (event.type == INPUT_TYPE_MOUSE)
                return;
            break;
        case INPUT_ROTATE_RIGHT:
            game_logic_rotate_piece(game, true);
            if (event.type == INPUT_TYPE_MOUSE)
                return;
            break;
        case INPUT_DROP:
            // Fast drop
            while (game_logic_move_piece(game, 0, 1)) {}
            game_logic_drop_piece(game);
            needs_redraw = true;
            if (event.type == INPUT_TYPE_MOUSE)
                return;
            break;
        default:
            break;
    }

    if (moved) {
        needs_redraw = true;
    }
}

// ===== RENDERIZAÇÃO =====
void game_logic_render(const GameLogic* game) {
    if (!game) return;

    vg_clear_screen(COLOR_BACKGROUND);
    draw_moldure();
    draw_grid_background();

    extern int grid[GRID_ROWS][GRID_COLS];
    draw_grid_contents(grid);
    draw_game_infos(game);

    if (!game->game_over) {
        const Piece ghost = game_logic_calculate_ghost_piece(game);
        if (ghost.y != game->current_piece.y) {
            draw_ghost_piece(&ghost);
        }
        draw_current_piece(&game->current_piece);
    }

    draw_grid();

    if (game_logic_is_paused(game)) {
        pause_ui_draw(game);
    }
}

// ===== UTILIDADES =====
bool game_logic_is_game_over(const GameLogic* game) {
    return game ? game->game_over : true;
}

void game_logic_update_speed(GameLogic* game) {
    if (!game) return;

                // NOVAS TECLAS WA
    const GameScore* score = tetris_get_score();
    const int speed_reduction = (score->level - 1) * 5;
    game->current_drop_speed = DROP_SPEED_INITIAL - speed_reduction;

    if (game->current_drop_speed < DROP_SPEED_FAST) {
        game->current_drop_speed = DROP_SPEED_FAST;
    }
    if (game->current_drop_speed > DROP_SPEED_INITIAL) {
        game->current_drop_speed = DROP_SPEED_INITIAL;
    }
}
