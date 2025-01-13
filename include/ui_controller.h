#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "game_controller.h"

typedef struct {
    HWND hwnd;
    HWND txtOutput;
    HWND txtInput;
    HWND btnSend;
} UIController;

void createWindow(UIController* self, HINSTANCE hInstance);

void updateUI(UIController* self, const char* message);

#endif //UICONTROLLER_H
