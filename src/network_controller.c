#include "../include/network_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <SFML/System.h>

#define CONN_TIMEOUT 60
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

cJSON* receive_type_message_data(SOCKET socket, const NetMessageType type) {
    while (true) {
        cJSON* json_msg = receive_json_message(socket);
        cJSON* type_json = cJSON_GetObjectItem(json_msg, "type");
        if (type_json && string_to_type(type_json->valuestring) == type) {
            return cJSON_Parse(cJSON_PrintUnformatted(cJSON_GetObjectItem(json_msg, "data")));
        }
    }
}

//todo: 可能的解耦重构，build_item_use_data，add_data_type，send_json_message
void send_json_message(SOCKET socket, const cJSON* json) {
    char* json_string = cJSON_PrintUnformatted(json);
    send(socket, json_string, strlen(json_string), 0);
    free(json_string);
}

void send_type_json_message(SOCKET socket, const NetMessageType type, cJSON* data) {
    cJSON* msg = cJSON_CreateObject();
    cJSON_AddStringToObject(msg, "type", type_to_string(type));
    cJSON_AddItemToObject(msg, "data", data);
    send_json_message(socket, msg);
    cJSON_Delete(msg);
}

cJSON* build_acknowledge_data() {
    cJSON* data = cJSON_CreateObject();

    return data;
}

void acknowledge(SOCKET socket) {
    cJSON* acknowledge_data = build_acknowledge_data();
    send_type_json_message(socket, ACKNOWLEDGE, acknowledge_data);
}

cJSON* build_item_use_data(const char item[]) {
    cJSON* data = cJSON_CreateObject();

    cJSON_AddStringToObject(data, "item", item);

    return data;
}

cJSON* build_player_shoot_data(const bool is_attack) {
    cJSON* data = cJSON_CreateObject();

    cJSON_AddBoolToObject(data, "is_attack", is_attack);

    return data;
}
