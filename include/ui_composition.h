#ifndef UI_COMPOSITION_H
#define UI_COMPOSITION_H

#include <SFML/Graphics.h>

typedef enum {
    ACTION_NONE,
    ACTION_PVP,
    ACTION_PVE,
    ACTION_EXIT
} MenuAction;

typedef struct {
    sfRectangleShape* shape;
    sfText* text;
    sfBool is_hovered;
    MenuAction action;
} Button;

void init_button(Button* btn, const char* label, sfVector2f position, sfFont* font);

#endif //UI_COMPOSITION_H
