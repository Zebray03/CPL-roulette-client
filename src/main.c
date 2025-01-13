#include "../include/network_controller.h"
#include "../include/game_controller.h"
#include "../include/ui_controller.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {  // 处理发送按钮的点击事件
            GameController* controller = (GameController*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
            startGame(controller);
        }
        break;
    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    // 初始化UI控制器
    UIController uiController = {0};
    createWindow(&uiController, hInstance);

    // 初始化游戏控制器和网络控制器
    GameController gameController = {0};
    initializeGameController(&gameController);
    SetWindowLongPtr(uiController.hwnd, GWLP_USERDATA, (LONG_PTR)&gameController);

    // 开始游戏
    startGame(&gameController);

    closesocket(gameController.netController.socket);
    WSACleanup();
    return 0;
}
