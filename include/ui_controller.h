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

#endif // UICONTROLLER_H
