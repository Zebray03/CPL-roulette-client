#include <network_protocol.h>
#include <stdlib.h>
#include <string.h>

const char* type_to_string(NetMessageType type) {
    switch (type) {
        case ACKNOWLEDGE: return "ACKNOWLEDGE";
        case JOIN_REQUEST: return "JOIN_REQUEST";
        case GAME_BEGIN: return "GAME_BEGIN";
        case GAME_OVER: return "GAME_OVER";
        case GAME_STATE: return "GAME_STATE";
        case ITEM_ASSIGN: return "ITEM_ASSIGN";
        case ITEM_USE: return "ITEM_USE";
        case PHONE_RESULT: return "PHONE_RESULT";
        case PLAYER_SHOOT: return "PLAYER_SHOOT";
        case PLAYER_STATE: return "PLAYER_STATE";
        case SHOOT_RESULT: return "SHOOT_RESULT";
        default: exit(777);
    }
}

NetMessageType string_to_type(const char type_str[]) {
    if (strcmp(type_str, "ACKNOWLEDGE") == 0) return ACKNOWLEDGE;
    if (strcmp(type_str, "JOIN_REQUEST") == 0) return JOIN_REQUEST;
    if (strcmp(type_str, "GAME_BEGIN") == 0) return GAME_BEGIN;
    if (strcmp(type_str, "GAME_OVER") == 0) return GAME_OVER;
    if (strcmp(type_str, "GAME_STATE") == 0) return GAME_STATE;
    if (strcmp(type_str, "ITEM_ASSIGN") == 0) return ITEM_ASSIGN;
    if (strcmp(type_str, "ITEM_USE") == 0) return ITEM_USE;
    if (strcmp(type_str, "PHONE_RESULT") == 0) return PHONE_RESULT;
    if (strcmp(type_str, "PLAYER_SHOOT") == 0) return PLAYER_SHOOT;
    if (strcmp(type_str, "PLAYER_STATE") == 0) return PLAYER_STATE;
    if (strcmp(type_str, "SHOOT_RESULT") == 0) return SHOOT_RESULT;
    exit(777);
}
