#ifndef GAME_H
#define GAME_H

#include "player.h"
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

typedef struct {
    Player player;
    bool turn;
    int rival_hp;
    int rival_knife_num;
    int rival_beer_num;
    int rival_phone_num;
    int round;
    int real_bullet_num;
} Game;

void start_pvp_battle(Game* game, SOCKET socket);

#endif //GAME_H
