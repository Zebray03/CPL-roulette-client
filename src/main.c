#include <stdio.h>

#include "network_controller.h"
#include "ui_controller.h"

int main() {
    MenuAction choice = show_main_menu();

    switch (choice) {
    case ACTION_PVP:
        printf("Attempting to connect server...\n");
        NetworkController nc;
        ConnectStatus status = connect_to_server(&nc);
        if (status == CONNECT_SUCCESS) {
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


    // NetworkController network_controller;
    // connect_to_server(&network_controller);
    //
    // while (true) {
    //     Game game;
    //     initialize_game(&game);
    //     while (!is_game_over(&game)) {
    //
    //     }
    // }

    return 0;
}
