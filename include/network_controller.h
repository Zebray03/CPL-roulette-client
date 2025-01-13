#ifndef NETWORKCONTROLLER_H
#define NETWORKCONTROLLER_H

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
    SOCKET socket;
    struct sockaddr_in server_addr;
} NetworkController;

void connectToServer(NetworkController* self);

void sendMessage(NetworkController* self, const char* message);

void receiveMessage(NetworkController* self, char* buffer);

#endif //NETWORKCONTROLLER_H
