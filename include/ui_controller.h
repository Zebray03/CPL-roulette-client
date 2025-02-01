#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "game.h"
#include <SFML/Graphics.h>

typedef enum {
    ACTION_NONE,
    ACTION_PVP,
    ACTION_PVE,
    ACTION_EXIT
} MenuAction;

typedef enum {
    CONN_STATUS_TRYING,
    CONN_STATUS_SUCCESS,
    CONN_STATUS_FAILED
} ConnectionStatus;

typedef struct {
    sfRectangleShape* shape;
    sfText* text;
    sfBool is_hovered;
    MenuAction action;
} Button;

MenuAction show_main_menu(void);

typedef struct {
    sfCircleShape* spinner;
    sfText* status_text;
    sfThread* connect_thread;
    ConnectionStatus status;
    sfMutex* connection_status_mutex;
} LoadingScreen;

void show_loading_screen(LoadingScreen* screen, sfRenderWindow* window);
void update_loading_screen(LoadingScreen* screen, sfRenderWindow* window);
void destroy_loading_screen(LoadingScreen* screen);

#endif // UICONTROLLER_H
