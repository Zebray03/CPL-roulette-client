#include "../include/ui_composition.h"

#define BUTTON_WIDTH 200
#define BUTTON_HEIGHT 60

void init_button(Button* btn, const char* label, sfVector2f position, sfFont* font) {
    // 按钮形状
    btn->shape = sfRectangleShape_create();
    sfRectangleShape_setSize(btn->shape, (sfVector2f){BUTTON_WIDTH, BUTTON_HEIGHT});
    sfRectangleShape_setFillColor(btn->shape, sfColor_fromRGB(70, 70, 70));
    sfRectangleShape_setOutlineThickness(btn->shape, 2);
    sfRectangleShape_setOutlineColor(btn->shape, sfWhite);
    sfRectangleShape_setPosition(btn->shape, position);

    // 按钮文字
    btn->text = sfText_create();
    sfText_setString(btn->text, label);
    sfText_setFont(btn->text, font);
    sfText_setCharacterSize(btn->text, 24);
    sfText_setFillColor(btn->text, sfWhite);

    // 居中文字
    sfFloatRect textRect = sfText_getLocalBounds(btn->text);
    sfVector2f textPos = {
        position.x + (BUTTON_WIDTH - textRect.width) / 2,
        position.y + (BUTTON_HEIGHT - textRect.height) / 2 - 5
    };
    sfText_setPosition(btn->text, textPos);

    btn->is_hovered = sfFalse;
}
