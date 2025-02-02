#include "../include/network_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <SFML/System.h>

#define CONN_TIMEOUT 6
#define RETRY_INTERVAL 2

sfMutex* connection_mutex;
volatile bool g_connection_finished;
volatile ConnectStatus g_connection_result;
volatile bool g_connection_interrupt;

ConnectStatus execute_connection(NetworkController* nc) {
    WSADATA wsaData;
    time_t start_time = time(NULL);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        return CONNECT_FAIL_WSA_ERROR;
    }

    while (difftime(time(NULL), start_time) < CONN_TIMEOUT) {
        if (g_connection_interrupt) {
            return CONNECT_FAIL_INTERRUPT;
        }

        if ((nc->socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            return CONNECT_FAIL_SOCKET;
        }

        // 非阻塞模式
        unsigned long mode = 1;
        if (ioctlsocket(nc->socket, FIONBIO, &mode) != 0) {
            closesocket(nc->socket);
            continue;
        }

        nc->server_addr.sin_family = AF_INET;
        nc->server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
        nc->server_addr.sin_port = htons(PORT);

        if (connect(nc->socket, (struct sockaddr*)&nc->server_addr,
                    sizeof(nc->server_addr)) == SOCKET_ERROR) {
            int err = WSAGetLastError();

            if (err != WSAEWOULDBLOCK) {
                closesocket(nc->socket);
                continue;
            }

            // 超时检测
            fd_set set;
            FD_ZERO(&set);
            FD_SET(nc->socket, &set);
            struct timeval timeout = {RETRY_INTERVAL, 0};

            if (select(0, NULL, &set, NULL, &timeout) <= 0) {
                closesocket(nc->socket);
                continue;
            }

            int connect_error = 0;
            int error_len = sizeof(connect_error);
            if (getsockopt(nc->socket, SOL_SOCKET, SO_ERROR, (char*)&connect_error, &error_len) != 0) {
                closesocket(nc->socket);
                continue;
            }

            if (connect_error != 0) {  // 连接失败
                printf("Connection failed. Error Code: %d\n", connect_error);
                closesocket(nc->socket);
                WSACleanup();
                return CONNECT_FAIL_NET_ERROR;
            }
        }

        // 恢复阻塞模式
        mode = 0;
        ioctlsocket(nc->socket, FIONBIO, &mode);
        return CONNECT_SUCCESS;
    }

    // 超时
    WSACleanup();
    return CONNECT_FAIL_TIMEOUT;
}

void network_connection_thread(NetworkController* nc) {
    ConnectStatus result = execute_connection(nc);

    // 临界区访问
    sfMutex_lock(connection_mutex);
    g_connection_result = result;
    g_connection_finished = true;
    sfMutex_unlock(connection_mutex);
}

ConnectStatus connect_to_server(NetworkController* nc) {
    sfThread* sf_thread = sfThread_create(network_connection_thread, nc);
    sfThread_launch(sf_thread);

    // 临界区访问
    while (true) {
        sfMutex_lock(connection_mutex);
        bool finished = g_connection_finished;
        sfMutex_unlock(connection_mutex);
        if (finished) {
            break;
        }
        sfSleep(sfMilliseconds(50));
    }

    ConnectStatus status = g_connection_result;
    sfThread_destroy(sf_thread);

    return status;
}

void wait_for_ready_message(SOCKET socket)
{
    cJSON* json_msg = NULL;
    while (true) {
        json_msg = receive_json_message(socket);
        cJSON* type_item = cJSON_GetObjectItem(json_msg, "type");
        if (type_item != NULL && type_item->valueint == GAME_BEGIN) {
            cJSON_Delete(json_msg);
            break;
        }
        cJSON_Delete(json_msg);
        Sleep(50);
    }
}

void start_pvp_battle(NetworkController* nc) {
    cJSON* item_assign_json=receive_json_message(nc->socket);
    //todo
}

void send_json_message(SOCKET socket, const cJSON* json) {
    char* json_string = cJSON_PrintUnformatted(json);
    send(socket, json_string, strlen(json_string), 0);
    free(json_string);
}

cJSON* receive_json_message(SOCKET socket) {
    char buffer[MAX_PACKET_SIZE];
    int recv_size = recv(socket, buffer, sizeof(buffer) - 1, 0);
    buffer[recv_size] = '\0';

    cJSON* json_message = cJSON_Parse(buffer);
    if (!json_message) {
        exit(1);
    }

    return json_message;
}

void send_game_begin_message(SOCKET socket) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(GAME_BEGIN));
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_game_state_message(SOCKET socket, const bool turn, const int round, const int real_bullet_num) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(GAME_STATE));
    cJSON_AddBoolToObject(msg, "turn", turn);
    cJSON_AddNumberToObject(msg, "round", round);
    cJSON_AddNumberToObject(msg, "real_bullet_num", real_bullet_num);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_item_assign_message(SOCKET socket, const int knife_num, const int beer_num, const int phone_num) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(ITEM_ASSIGN));
    cJSON_AddNumberToObject(msg, "knife_num", knife_num);
    cJSON_AddNumberToObject(msg, "beer_num", beer_num);
    cJSON_AddNumberToObject(msg, "phone_num", phone_num);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_player_state_message(SOCKET socket, Player* player) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(PLAYER_STATE));
    cJSON_AddNumberToObject(msg, "hp", player->hp);
    cJSON* json_bullet = cJSON_CreateArray();
    for (int i = 0; i < ROUND_NUM; i++) {
        cJSON_AddItemToArray(json_bullet, cJSON_CreateNumber(player->bullets[i]));
    }
    cJSON_AddItemToObject(msg, "bullets", json_bullet);
    cJSON_AddNumberToObject(msg, "knife_num", player->knife_num);
    cJSON_AddNumberToObject(msg, "beer_num", player->beer_num);
    cJSON_AddNumberToObject(msg, "phone_num", player->phone_num);
    cJSON_AddBoolToObject(msg, "knife_use", player->knife_use);
    cJSON_AddBoolToObject(msg, "beer_use", player->beer_use);
    cJSON_AddBoolToObject(msg, "phone_use", player->phone_use);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_item_use_message(SOCKET socket, const char item[]) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(ITEM_USE));
    cJSON_AddStringToObject(msg, "item", item);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_player_shoot_message(SOCKET socket, const bool is_attack) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(PLAYER_SHOOT));
    cJSON_AddBoolToObject(msg, "is_attack", is_attack);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_shoot_result_message(SOCKET socket, bool is_attack, int damage) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(SHOOT_RESULT));
    cJSON_AddBoolToObject(msg, "is_attack", is_attack);
    cJSON_AddNumberToObject(msg, "damage", damage);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

void send_game_over_message(SOCKET socket, const bool is_winner) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(GAME_OVER));
    cJSON_AddBoolToObject(msg, "is_winner", is_winner);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}
