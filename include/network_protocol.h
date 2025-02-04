#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

#define MAX_PACKET_SIZE 1024
#define DEFAULT_PORT 8080

typedef enum {
    ACKNOWLEDGE,
    JOIN_REQUEST,
    GAME_BEGIN,
    GAME_OVER,
    GAME_STATE,
    ITEM_ASSIGN,
    ITEM_USE,
    PHONE_RESULT,
    PLAYER_SHOOT,
    PLAYER_STATE,
    SHOOT_RESULT,
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
