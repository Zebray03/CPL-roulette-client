#ifndef GAME_H
#define GAME_H

#include "player.h"

typedef struct {
    Player gambler;
    Player demon;
    int round;
} Game;

void initialize_game(Game* game);

//void shoot(Game* game, NetworkController* nc);

bool is_game_over(Game* game);

#endif //GAME_H
