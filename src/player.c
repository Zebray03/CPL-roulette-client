#include "../include/player.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HP_MAX 3

void assign_items(Player* player, const cJSON* item_assign_data) {
    int knife_num = cJSON_GetObjectItem(item_assign_data, "knife_num")->valueint;
    int beer_num = cJSON_GetObjectItem(item_assign_data, "beer_num")->valueint;
    int phone_num = cJSON_GetObjectItem(item_assign_data, "phone_num")->valueint;

    player->knife_num += knife_num;
    player->beer_num += beer_num;
    player->phone_num += phone_num;
}

bool use_knife(Player* player) {
    bool use_success;
    if (player->knife_num == 0) {
        printf("don't have knife\n");
        use_success = false;
    }
    else if (player->knife_use) {
        printf("have used knife already\n");
        use_success = false;
    }
    else {
        player->knife_num--;
        player->knife_use = true;
        use_success = true;
    }
    return use_success;
}

bool use_beer(Player* player) {
    bool use_success;
    if (player->beer_num == 0) {
        printf("don't have beer\n");
        use_success = false;
    }
    else if (player->beer_use) {
        printf("have used beer already\n");
        use_success = false;
    }
    else {
        player->hp += 2;
        if (player->hp > HP_MAX) {
            player->hp = HP_MAX;
        }
        player->beer_num--;
        player->beer_use = true;
        use_success = true;
    }
    return use_success;
}

bool use_phone(const Player* player) {
    bool use_success;
    if (player->phone_num == 0) {
        printf("don't have phone\n");
        use_success = false;
    }
    else if (player->phone_use) {
        printf("have used phone already\n");
        use_success = false;
    }
    else {
        int unknown_num = 0;
        for (int i = 0; i < ROUND_NUM; i++) {
            if (player->bullets[i] == 0) {
                unknown_num++;
            }
        }
        if (unknown_num == 0) {
            printf("don't have unknown one\n");
            use_success = false;
        }
        else {
            use_success = true;
        }
    }
    return use_success;
}

void renew_player_state(Player* player, const cJSON* player_state_data) {
    player->hp = cJSON_GetObjectItem(player_state_data, "hp")->valueint;
    player->is_demon = cJSON_GetObjectItem(player_state_data, "is_demon")->valueint;
    cJSON* bullets = cJSON_GetObjectItem(player_state_data, "bullets");
    for (int i = 0; i < ROUND_NUM; i++) {
        player->bullets[i] = cJSON_GetArrayItem(bullets, i)->valueint;
    }
    player->knife_num = cJSON_GetObjectItem(player_state_data, "knife_num")->valueint;
    player->beer_num = cJSON_GetObjectItem(player_state_data, "beer_num")->valueint;
    player->phone_num = cJSON_GetObjectItem(player_state_data, "phone_num")->valueint;
    player->knife_use = cJSON_GetObjectItem(player_state_data, "knife_use")->valueint;
    player->beer_use = cJSON_GetObjectItem(player_state_data, "beer_use")->valueint;
    player->phone_use = cJSON_GetObjectItem(player_state_data, "phone_use")->valueint;
}
