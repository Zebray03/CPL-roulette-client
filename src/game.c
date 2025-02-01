#include "../include/game.h"

#include <stdio.h>

void initialize_game(Game* game) {
    initialize_player(&game->gambler, "赌徒");
    initialize_player(&game->demon, "恶魔");
    game->round = 1;
}

void shoot(Game* game) {
    // cJSON* root = cJSON_CreateObject();
    // cJSON_AddNumberToObject(root, "type", MSG_SHOOT);
    // cJSON_AddStringToObject(root, "shooter", game->current_player->name);
    // cJSON_AddBoolToObject(root, "target_self", shoot_self);
}

bool is_game_over(Game* game) {
    if (game->gambler.hp <= 0) {
        printf("游戏结束！%s 胜利！\n", game->demon.name);
        return true;
    }
    if (game->demon.hp <= 0) {
        printf("游戏结束！%s 胜利！\n", game->gambler.name);
        return true;
    }
    return false;
}
