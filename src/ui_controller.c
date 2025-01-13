#include "../include/ui_controller.h"

#include <windows.h>

void createWindow(UIController* self, HINSTANCE hInstance) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = DefWindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = "GameClientWindow";
    RegisterClass(&wc);

    self->hwnd = CreateWindowEx(0, "GameClientWindow", "Game Client", WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL,
                                hInstance, NULL);

    self->txtOutput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | ES_MULTILINE | ES_READONLY,
                                   10, 10, 760, 400, self->hwnd, NULL, hInstance, NULL);

    self->txtInput = CreateWindow("EDIT", "", WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL,
                                  10, 420, 560, 30, self->hwnd, NULL, hInstance, NULL);

    self->btnSend = CreateWindow("BUTTON", "Send", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
                                 580, 420, 80, 30, self->hwnd, (HMENU)1, hInstance, NULL);
    ShowWindow(self->hwnd, SW_SHOWNORMAL);
    UpdateWindow(self->hwnd);
}

void updateUI(UIController* self, const char* message) {
    SetWindowText(self->txtOutput, message);
}