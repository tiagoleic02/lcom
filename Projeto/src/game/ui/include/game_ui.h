#ifndef GAME_UI_H
#define GAME_UI_H

#include "../../core/include/game.h"

// Draws the Tetris grid
void draw_grid();

// Draws the backgroud of Tetris grid
void draw_grid_background();

// Draws the score, lines, and level information
void draw_game_infos();

// Draws the score box
void draw_score(void);

// Draws the lines box
void draw_lines(void);

// Draws the high scores
void draw_highscores(void);

// Draws the level box
void draw_level(void);

// Draws the next piece box
void draw_next_piece(const GameLogic* game);

#endif //GAME_UI_H
