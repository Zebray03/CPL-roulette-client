#include <stdio.h>
#include "main.h"
#include "network_controller.h"
#include "ui_controller.h"

GlobalState gs = MAIN_MENU;

int main() {
    while (gs != EXIT) {
        NetworkController nc;

        switch (gs) {
        case MAIN_MENU: {
            MenuAction choice = show_home_page();
            switch (choice) {
            case ACTION_PVP:
                gs = CONNECTING;
                break;
            case ACTION_PVE:
                printf("Begin PVE Fight\n");
                break;
            case ACTION_EXIT:
                gs = EXIT;
                break;
            default:
                break;
            }
            break;
        }
        case CONNECTING: {
            connection_mutex = sfMutex_create();
            g_connection_finished = false;
            g_connection_result = CONNECT_FAIL_TIMEOUT;
            g_connection_interrupt = false;
            sfThread* waiting_ui_thread = sfThread_create(show_connect_waiting_page, NULL);
            sfThread_launch(waiting_ui_thread);

            ConnectStatus status = connect_to_server(&nc);
            sfThread_wait(waiting_ui_thread);

            switch (status) {
            case CONNECT_SUCCESS: {
                show_connection_result("Success");
                gs = GAMING;
                break;
            }
            case CONNECT_FAIL_INTERRUPT: {
                gs = MAIN_MENU;
                break;
            }
            case CONNECT_FAIL_TIMEOUT: {
                bool retry = show_retry_dialog();
                if (!retry) {
                    show_connection_result("Fail");
                    gs = MAIN_MENU;
                }
                break;
            }
            default:
                gs = MAIN_MENU;
                break;
            }
            sfThread_destroy(waiting_ui_thread);
            connection_mutex = NULL;
            break;
        }
        case GAMING: {
            wait_for_ready_message(nc.socket);
            printf("Begin PVP Fight\n");
            start_pvp_battle(&nc);
            break;
        }
        case EXIT: {}
        }
    }
    return 0;
}
