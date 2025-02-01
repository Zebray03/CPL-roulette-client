#include "../include/network_controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>

#define CONN_TIMEOUT 60
#define RETRY_INTERVAL 2

ConnectStatus connect_to_server(NetworkController* nc) {
    WSADATA wsaData;
    time_t start_time = time(NULL);
    int try_count = 0;

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        return CONNECT_FAIL_SOCKET;
    }

    while (difftime(time(NULL), start_time) < CONN_TIMEOUT) {
        printf("%d\n", ++try_count);
        if ((nc->socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
            printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
            continue;
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

            if (connect_error != 0) {  // 连接最终失败
                printf("Connection failed. Error Code: %d\n", connect_error);
                closesocket(nc->socket);
                WSACleanup();
                return CONNECT_FAIL_NET_ERROR;
            }
        }

        // 恢复阻塞模式
        mode = 0;
        ioctlsocket(nc->socket, FIONBIO, &mode);

        MessageBox(NULL, "Success", "state", MB_OK);
        printf("Connected to %s:%d\n", SERVER_IP, PORT);
        return CONNECT_SUCCESS;
    }

    // 超时处理
    int ret = MessageBox(NULL, "Timeout. Retry?", "Connect fail.", MB_YESNO);
    if (ret == IDYES) {
        return connect_to_server(nc); // 递归重试
    }

    WSACleanup();
    return CONNECT_FAIL_TIMEOUT;
}

void send_message(NetworkController* nc, GameMessage* msg) {
    char* json_str = cJSON_PrintUnformatted(msg->data);
    if (send(nc->socket, json_str, strlen(json_str), 0) == SOCKET_ERROR) {
        printf("Failed to send data. Error Code: %d\n", WSAGetLastError());
    }
    free(json_str);
}

void receive_message(NetworkController* nc, GameMessage* msg) {
    char buffer[BUFFER_SIZE];
    int recv_size = recv(nc->socket, buffer, BUFFER_SIZE - 1, 0);
    if (recv_size == SOCKET_ERROR) {
        printf("Failed to receive data. Error Code: %d\n", WSAGetLastError());
    }

    msg->data = cJSON_Parse(buffer);
    if (!msg->data) {
        printf("JSON Error: %s\n", buffer);
        return;
    }

    cJSON* type_obj = cJSON_GetObjectItem(msg->data, "type");
    msg->type = type_obj->valueint;
    buffer[recv_size] = '\0';
}

void start_pvp_battle(NetworkController* nc) {
}