#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include "cJSON.h"

#define MAX_PACKET_SIZE 1024
#define DEFAULT_PORT 8080

typedef enum {
    JOIN_REQUEST,
    GAME_BEGIN,
    GAME_STATE,
    ITEM_ASSIGN,
    PLAYER_STATE,
    ITEM_USE,
    PLAYER_SHOOT,
    SHOOT_RESULT,
    GAME_OVER
} NetMessageType;

typedef enum {
    CONNECT_SUCCESS,
    CONNECT_FAIL_TIMEOUT,
    CONNECT_FAIL_WSA_ERROR,
    CONNECT_FAIL_SOCKET,
    CONNECT_FAIL_NET_ERROR,
    CONNECT_FAIL_INTERRUPT,
} ConnectStatus;

const char* type_to_string(NetMessageType type);

NetMessageType string_to_type(const char type_str[]);

#endif //NETWORK_PROTOCOL_H
