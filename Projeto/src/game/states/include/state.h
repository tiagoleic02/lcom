#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include "../../core/include/input.h"

typedef enum {
    STATE_MENU,
    STATE_INSTRUCTIONS,
    STATE_GAME,
    STATE_HIGHSCORE,
    STATE_GAME_OVER,
    STATE_QUIT
} GameStateType;

static bool needs_redraw = true;

// Interface para estados
typedef struct GameState GameState;
struct GameState {
    GameStateType type;
    void (*init)(GameState* state);
    void (*cleanup)(GameState* state);
    void (*handle_input)(GameState* state, InputEvent event);
    void (*update)(GameState* state);
    void (*render)(GameState* state);
    GameStateType (*get_next_state)(GameState* state);
    void* data;
};

// State manager
void state_manager_init(void);
void state_manager_set_state(GameStateType type);
void state_manager_handle_input(InputEvent event);
void state_manager_update(void);
void state_manager_render(void);
bool state_manager_should_quit(void);


#endif

