#include "../include/game.h"
#include <stdio.h>

#include "network_controller.h"
#include "network_protocol.h"

void renew_game_state(Game* game, const cJSON* game_state_data) {
    game->turn = cJSON_GetObjectItem(game_state_data, "turn")->valueint;
    game->rival_hp = cJSON_GetObjectItem(game_state_data, "rival_hp")->valueint;
    game->rival_knife_num = cJSON_GetObjectItem(game_state_data, "rival_knife_num")->valueint;
    game->rival_beer_num = cJSON_GetObjectItem(game_state_data, "rival_beer_num")->valueint;
    game->rival_phone_num = cJSON_GetObjectItem(game_state_data, "rival_phone_num")->valueint;
    game->round = cJSON_GetObjectItem(game_state_data, "round")->valueint;
    game->real_bullet_num = cJSON_GetObjectItem(game_state_data, "real_bullet_num")->valueint;
}

void resolve_shoot_result(Game* game, const cJSON* shoot_result_data, const bool is_source) {
    bool is_attack = cJSON_GetObjectItem(shoot_result_data, "is_attack")->valueint;
    int damage = cJSON_GetObjectItem(shoot_result_data, "damage")->valueint;

    if (damage > 0) {
        game->player.bullets[game->round - 1] = 1;
    }
    else {
        game->player.bullets[game->round - 1] = -1;
    }

    if (is_source) {
        if (is_attack) {
            game->turn = !game->turn;
            if (damage > 0) {
                game->rival_hp -= damage;
                printf("hit rival %d\n", damage);
            }
            else {
                printf("terrible click\n");
            }
        }
        else {
            if (damage > 0) {
                game->turn = !game->turn;
                game->player.hp -= damage;
                printf("suicide %d\n", damage);
            }
            else {
                printf("good click\n");
            }
        }
    }
    else {
        if (is_attack) {
            game->turn = !game->turn;
            if (damage > 0) {
                game->player.hp -= damage;
                printf("hit you %d\n", damage);
            }
            else {
                printf("good click\n");
            }
        }
        else {
            if (damage > 0) {
                game->turn = !game->turn;
                game->rival_hp -= damage;
                printf("rival suicide %d\n", damage);
            }
            else {
                printf("terrible click\n");
            }
        }
    }

    game->round++;
}

bool is_game_over(Game* game) {
    return game->player.hp <= 0 || game->rival_hp <= 0;
}

void start_pvp_battle(Game* game, SOCKET socket) {
    setbuf(stdout,NULL);

    // ReSharper disable CppJoinDeclarationAndAssignment
    // ReSharper disable CppTooWideScope

    cJSON* game_begin_data;
    cJSON* game_over_data;
    cJSON* game_state_data;
    cJSON* item_assign_data;
    cJSON* item_use_data;
    cJSON* phone_result_data;
    cJSON* player_shoot_data;
    cJSON* player_state_data;
    cJSON* shoot_result_data;

    Player* player = &game->player;

    //todo: 暂时使用命令行

    // 就绪同步
    {
        game_begin_data = receive_type_message_data(socket, GAME_BEGIN);
        cJSON_Delete(game_begin_data);

        acknowledge(socket);
    }

    //todo: game begins

    /* 获得初始玩家状态
     * - hp 3
     * - 身份 demon 或 gambler
     * - 子弹视野 全未知
     * - 道具数量 全0
     * - 使用情况 全false
     */
    {
        player_state_data = receive_type_message_data(socket, PLAYER_STATE);
        renew_player_state(player, player_state_data);
        cJSON_Delete(player_state_data);

        acknowledge(socket);
    }

    // 获得初始游戏状态
    {
        game_state_data = receive_type_message_data(socket, GAME_STATE);
        renew_game_state(game, game_state_data);
        cJSON_Delete(game_state_data);

        acknowledge(socket);
    }

    //todo: show initial state

    // 每局
    while (!is_game_over(game)) {
        // 拿到道具
        {
            item_assign_data = receive_type_message_data(socket, ITEM_ASSIGN);
            assign_items(player, item_assign_data);
            cJSON_Delete(item_assign_data);

            acknowledge(socket);
        }

        //todo: show items

        // 每回合
        for (game->round; game->round <= ROUND_NUM; game->round++) {
            /*
             * 同步游戏状态
             * - 当前持枪者
             * - 当前回合数
             * - 当局实弹数
             */
            {
                game_state_data = receive_type_message_data(socket, GAME_STATE);
                renew_game_state(game, game_state_data);
                cJSON_Delete(game_state_data);

                acknowledge(socket);
            }

            //todo: show game state

            // 如果当前持枪
            if (game->turn) {
                printf("Your turn\n");
                int opr_node; // 0 - 开枪；1 - 使用道具
                do {
                    printf("Input opr-node:\n");
                    scanf("%d", &opr_node);
                    if (opr_node == 0) {
                        int is_attack;  // 0 - 朝自己开枪；1 - 开枪
                        printf("Input is_attack:\n");
                        scanf("%d", &is_attack);

                        player_shoot_data = build_player_shoot_data(is_attack);
                        send_type_json_message(socket, PLAYER_SHOOT, player_shoot_data);
                        receive_json_message(socket);

                        shoot_result_data = receive_type_message_data(socket, SHOOT_RESULT);
                        acknowledge(socket);

                        // 结算开枪结果
                        resolve_shoot_result(game, shoot_result_data,true);
                        cJSON_Delete(shoot_result_data);
                    }
                    else if (opr_node == 1) {
                        int item_int; // 1 - 🔪；2 - 🍺；3 - 📱
                        printf("Input item_int:\n");
                        scanf("%d", &item_int);

                        bool use_success = false;
                        // 不合法的道具使用在客户端拦截，不发送任何消息

                        char item[10];
                        switch (item_int) {
                            case 1: {
                                strcpy(item, "KNIFE");
                                use_success = use_knife(player);
                                break;
                            }
                            case 2: {
                                strcpy(item, "BEER");
                                use_success = use_beer(player);
                                break;
                            }
                            case 3: {
                                strcpy(item, "PHONE");
                                use_success = use_phone(player);
                                break;
                            }
                            default: break;
                        }

                        if (use_success) {
                            {
                                item_use_data = build_item_use_data(item);
                                send_type_json_message(socket, ITEM_USE, item_use_data);
                                receive_json_message(socket);
                            }

                            if (item_int == 3) {
                                // 使用手机
                                phone_result_data = receive_type_message_data(socket, PHONE_RESULT);
                                acknowledge(socket);

                                int index = cJSON_GetObjectItem(phone_result_data, "index")->valueint;
                                bool is_true = cJSON_GetObjectItem(phone_result_data, "is_true")->valueint;
                                printf("The %dth bullet is %s.\n", index, is_true ? "true" : "false");
                            }

                            player_state_data = receive_type_message_data(socket, PLAYER_STATE);
                            acknowledge(socket);
                            renew_player_state(player, player_state_data);
                        }
                        else {
                            printf("Invalid item\n");
                        }
                    }
                    else {
                        exit(555);
                    }
                }
                while (opr_node == 1);
            }
            // 否则
            else {
                // 等待对手行动
                printf("Waiting...\n");
                cJSON* recv_json;
                NetMessageType recv_type;
                do {
                    recv_json = receive_json_message(socket);
                    acknowledge(socket);

                    recv_type = string_to_type(cJSON_GetObjectItem(recv_json, "type")->valuestring);
                    cJSON* recv_data = cJSON_Parse(cJSON_PrintUnformatted(cJSON_GetObjectItem(recv_json, "data")));
                    switch (recv_type) {
                        case ITEM_USE: {
                            printf("Item use: %s\n", cJSON_GetObjectItem(recv_data, "item")->valuestring);
                            cJSON_Delete(recv_data);

                            game_state_data = receive_type_message_data(socket, GAME_STATE);
                            acknowledge(socket);

                            renew_game_state(game, game_state_data);
                            break;
                        }
                        case SHOOT_RESULT: {
                            // 结算开枪结果
                            shoot_result_data = recv_data;
                            resolve_shoot_result(game, shoot_result_data,false);
                            cJSON_Delete(shoot_result_data);

                            break;
                        }
                        default: exit(777);
                    }
                }
                while (recv_type == ITEM_USE);
            }

            // 如果游戏结束
            if (is_game_over(game)) {
                // 接收游戏结果
                {
                    game_over_data = receive_type_message_data(socket, GAME_OVER);
                    acknowledge(socket);

                    bool is_winner = cJSON_GetObjectItem(game_over_data, "is_winner")->valueint;
                    if (is_winner) {
                        printf("You win.\n");
                    }
                    else {
                        printf("You lose.\n");
                    }
                }
            }
        }
    }
}
