#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>

#include "cJSON.h"

#define ROUND_NUM 5

typedef struct {
    int hp;
    bool is_demon;
    int bullets[ROUND_NUM];  // 1 - 实弹，0 - 未知，-1 - 虚弹）
    int knife_num;
    int beer_num;
    int phone_num;
    bool knife_use;
    bool beer_use;
    bool phone_use;
} Player;

void assign_items(Player* player, const cJSON* item_assign_data);

bool use_knife(Player* player);

bool use_beer(Player* player);

bool use_phone(const Player* player);

void renew_player_state(Player* player, const cJSON* player_state_data);

#endif //PLAYER_H
