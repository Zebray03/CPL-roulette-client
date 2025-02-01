#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#include "game.h"
#include "../lib/cJSON/cJSON.h"

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    SOCKET socket;
    struct sockaddr_in server_addr;
    Game pvp_game;
} NetworkController;

typedef enum {
    CONNECT_SUCCESS,
    CONNECT_FAIL_TIMEOUT,
    CONNECT_FAIL_SOCKET,
    CONNECT_FAIL_NET_ERROR,
} ConnectStatus;

typedef enum {
    MSG_SHOOT,
    MSG_USE_ITEM,
    MSG_GAME_STATE
} MessageType;

typedef struct {
    MessageType type;
    cJSON* data;
} GameMessage;

ConnectStatus connect_to_server(NetworkController* nc);

void send_message(NetworkController* nc, GameMessage* msg);

void receive_message(NetworkController* nc, GameMessage* msg);

void start_pvp_battle(NetworkController* nc);

#endif //NETWORKCONTROLLER_H
