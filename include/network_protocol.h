#ifndef NETWORK_PROTOCOL_H
#define NETWORK_PROTOCOL_H

#define MAX_PACKET_SIZE 1024
#define DEFAULT_PORT 8080

// 网络消息类型
typedef enum {
    NET_JOIN_REQUEST,
    NET_GAME_STATE,
    NET_PLAYER_ACTION,
    NET_ITEM_USE,
    NET_GAME_OVER
} NetMessageType;

// 网络消息结构
typedef struct {
    NetMessageType type;
    char data[MAX_PACKET_SIZE];
} NetworkPacket;

#endif //NETWORK_PROTOCOL_H
