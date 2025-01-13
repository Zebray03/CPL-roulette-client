#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "network_controller.h"

#include <windows.h>

typedef struct game_controller {
    NetworkController netController;
    HWND hwnd;
    HWND txtOutput;
    HWND txtInput;
    HWND btnSend;
} GameController;

void initializeGameController(GameController* self);

void startGame(GameController* self);

void displayMessage(GameController* self, const char* message);

void playGame(GameController* self);

#endif //GAMECONTROLLER_H
