#include "../include/game_controller.h"

#include <stdbool.h>
#include <stdio.h>

void initializeGameController(GameController* self) {
    // 初始化网络控制器
    connectToServer(&self->netController);
}

void startGame(GameController* self) {
    while (true) {
        char buffer[BUFFER_SIZE] = {0};
        receiveMessage(&self->netController, buffer);
        displayMessage(self, buffer);

        if (strstr(buffer, "wins") != NULL) {
            printf("Game over. Exiting...\n");
            break;
        }

        char command[BUFFER_SIZE] = {0};
        GetWindowText(self->txtInput, command, BUFFER_SIZE);
        sendMessage(&self->netController, command);
    }
}

void displayMessage(GameController* self, const char* message) {
    SetWindowText(self->txtOutput, message);
}

void playGame(GameController* self) {
    char buffer[BUFFER_SIZE] = {0};
    char command[BUFFER_SIZE] = {0};

    while (1) {
        // 接收服务器发送的状态更新
        memset(buffer, 0, BUFFER_SIZE);
        int recv_size = recv(self->netController.socket, buffer, BUFFER_SIZE, 0);
        if (recv_size == SOCKET_ERROR) {
            printf("Failed to receive data. Error Code: %d\n", WSAGetLastError());
            break;
        }
        buffer[recv_size] = '\0';

        // 打印服务器信息
        printf("%s\n", buffer);

        // 判断游戏是否结束
        if (strstr(buffer, "wins") != NULL) {
            printf("Game over. Exiting...\n");
            break;
        }

        // 显示服务器信息到UI
        displayMessage(self, buffer);

        // 获取玩家输入的指令
        printf("Enter your action (shoot [self/enemy], use [knife/beer/phone]): ");
        GetWindowText(self->txtInput, command, BUFFER_SIZE);
        if (strlen(command) == 0) {
            continue;  // 如果输入为空，跳过发送
        }

        // 将指令发送到服务器
        if (send(self->netController.socket, command, strlen(command), 0) == SOCKET_ERROR) {
            printf("Failed to send data. Error Code: %d\n", WSAGetLastError());
            break;
        }
    }
}