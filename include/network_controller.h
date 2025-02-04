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

cJSON* receive_json_message(SOCKET socket);

cJSON* receive_type_message_data(SOCKET socket, NetMessageType type);

void send_type_json_message(SOCKET socket, NetMessageType type, cJSON* data);

void acknowledge(SOCKET socket);

cJSON* build_item_use_data(const char item[]);

cJSON* build_player_shoot_data(bool is_attack);

#endif //NETWORKCONTROLLER_H
