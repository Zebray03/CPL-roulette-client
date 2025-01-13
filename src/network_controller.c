#include "../include/network_controller.h"

#include <stdio.h>
#include <stdlib.h>

void connectToServer(NetworkController* self) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        exit(1);
    }

    if ((self->socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        WSACleanup();
        exit(1);
    }

    self->server_addr.sin_family = AF_INET;
    self->server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    self->server_addr.sin_port = htons(PORT);

    if (connect(self->socket, (struct sockaddr*)&(self->server_addr), sizeof(self->server_addr)) == SOCKET_ERROR) {
        printf("Connection to server failed. Error Code: %d\n", WSAGetLastError());
        closesocket(self->socket);
        WSACleanup();
        exit(1);
    }

    printf("Connected to server at %s:%d\n", SERVER_IP, PORT);
}

void sendMessage(NetworkController* self, const char* message) {
    if (send(self->socket, message, strlen(message), 0) == SOCKET_ERROR) {
        printf("Failed to send data. Error Code: %d\n", WSAGetLastError());
    }
}

void receiveMessage(NetworkController* self, char* buffer) {
    int recv_size = recv(self->socket, buffer, BUFFER_SIZE, 0);
    if (recv_size == SOCKET_ERROR) {
        printf("Failed to receive data. Error Code: %d\n", WSAGetLastError());
    }
    buffer[recv_size] = '\0';
}
