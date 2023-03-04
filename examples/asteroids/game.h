#ifndef GAME_H
#define GAME_H

typedef struct
{
    Ship ship;
    Array_T bullets, asteroids;
    
} GameState;

void game_init(void);
void game_update_and_render(void);
void game_shutdown(void);

#endif //GAME_H
