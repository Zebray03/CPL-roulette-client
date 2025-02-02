#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#define ROUND_NUM 5

typedef struct {
    int hp;
    int bullets[ROUND_NUM];  // 1 - 实弹，0 - 未知，-1 - 虚弹）
    int knife_num;
    int beer_num;
    int phone_num;
    bool knife_use;
    bool beer_use;
    bool phone_use;
    bool is_gambler;
    bool is_my_turn;
} Player;

void initialize_player(Player* player);

void assign_items(Player* player, int knife_num, int beer_num, int phone_num);

void use_knife(Player* player);

void use_beer(Player* player);

void use_phone(Player* player);

// void shoot(Player* source, Player* target,
//            SOCKET current_socket, char current_buffer[],
//            SOCKET opponent_socket, char opponent_buffer[]);

void display_player_info(Player* player);

#endif //PLAYER_H
