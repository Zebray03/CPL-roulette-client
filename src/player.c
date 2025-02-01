#include "../include/player.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HP_MAX 3

void initialize_player(Player* player, const char* name) {
    strcpy(player->name, name);
    player->hp = HP_MAX;
    for (int i = 0; i < ROUND_NUM; i++) {
        player->bullet_view[i] = 0;
    }
    player->knife_num = 0;
    player->beer_num = 0;
    player->phone_num = 0;
    player->knife_use = false;
    player->beer_use = false;
    player->phone_use = false;
    //todo：找服务器拿当前game的所有属性
    if (player->is_gambler) {
        player->is_my_turn = false;
    }
    else {
        player->is_my_turn = true;
    }
}

void assign_items(Player* player, int assigned_knife_num, int assigned_beer_num, int assigned_phone_num) {
    player->knife_num += assigned_knife_num;
    player->beer_num += assigned_beer_num;
    player->phone_num += assigned_phone_num;
}

void use_knife(Player* player) {
    if (player->knife_num == 0) {
        printf("don't have knife\n");
    }
    else if (player->knife_use) {
        printf("have used knife already\n");
    }
    else {
        player->knife_num--;
        player->knife_use = true;
        //todo:在此处调用服务器端use_knife(Player* player)
    }
}

void use_beer(Player* player) {
    if (player->beer_num == 0) {
        printf("don't have beer\n");
    }
    else if (player->beer_use) {
        printf("have used beer already\n");
    }
    else {
        player->hp += 2;
        if (player->hp > HP_MAX) {
            player->hp = HP_MAX;
        }
        player->beer_num--;
        player->beer_use = true;
        //todo:在此处调用服务器端use_beer(Player* player)
    }
}

// 这里“任意一发”理解为每一发子弹
void use_phone(Player* player) {
    if (player->phone_num == 0) {
        printf("don't have phone\n");
    }
    else if (player->phone_use) {
        printf("have used phone already\n");
    }
    else {
        //todo
    }
}

// void shoot(Player* source, Player* target,
//            SOCKET current_socket, char current_buffer[],
//            SOCKET opponent_socket, char opponent_buffer[]) {
//
// }

void display_player_info(Player* player) {
    printf("玩家: %s\n", player->name);
    printf("血量: %d\n", player->hp);
}
