#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <SFML/System.h>
#include "game.h"
#include "network_protocol.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080

typedef struct {
    SOCKET socket;
    struct sockaddr_in server_addr;
    Game pvp_game;
} NetworkController;

extern sfMutex* connection_mutex;
extern volatile bool g_connection_finished;
extern volatile ConnectStatus g_connection_result;
extern volatile bool g_connection_interrupt;

ConnectStatus connect_to_server(NetworkController* nc);

void wait_for_ready_message(SOCKET socket);

void start_pvp_battle(NetworkController* nc);

cJSON* receive_json_message(SOCKET socket);

void send_game_begin_message(SOCKET socket);

void send_game_state_message(SOCKET socket, bool turn, int round, int real_bullet_num);

void send_item_assign_message(SOCKET socket, int knife_num, int beer_num, int phone_num);

void send_player_state_message(SOCKET socket, Player* player);

void send_item_use_message(SOCKET socket, const char item[]);

void send_player_shoot_message(SOCKET socket, bool is_attack);

void send_shoot_result_message(SOCKET socket, bool is_attack, int damage);

void send_game_over_message(SOCKET socket, bool is_winner);

#endif //NETWORKCONTROLLER_H
