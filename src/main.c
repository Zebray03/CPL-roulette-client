#include <stdio.h>

#include "network_controller.h"
#include "ui_controller.h"

int main() {
    MenuAction choice = show_main_menu();

    switch (choice) {
    case ACTION_PVP:
        sfRenderWindow* loading_window = sfRenderWindow_create(
            (sfVideoMode){800, 600, 32},
            "Connecting...",
            sfNone,
            NULL
        );
        LoadingScreen screen;
        show_loading_screen(&screen, loading_window);

        NetworkController nc;
        // 启动连接线程
        sfThread* thread = sfThread_create(connect_to_server, &nc);
        sfThread_launch(thread);

        // UI更新循环
        while (sfRenderWindow_isOpen(loading_window)) {
            sfEvent event;
            while (sfRenderWindow_pollEvent(loading_window, &event)) {
                if (event.type == sfEvtClosed) {
                    sfRenderWindow_close(loading_window);
                }
            }

            update_loading_screen(&screen, loading_window);

            // 检查连接状态
            if (nc.connect_status != CONN_STATUS_TRYING) {
                sfRenderWindow_close(loading_window);
            }
        }

        // 清理
        destroy_loading_screen(&screen);
        sfRenderWindow_destroy(loading_window);

        if (nc.connect_status == CONN_STATUS_SUCCESS) {
            start_pvp_battle(&nc);
        }
        else {
            printf("Fail.\n");
        }

        break;
    case ACTION_PVE:
        printf("Begin PVE Fight\n");
        break;
    case ACTION_EXIT:
        printf("Exit\n");
        break;
    default:
        fprintf(stderr, "Error\n");
    }

    return 0;
}
