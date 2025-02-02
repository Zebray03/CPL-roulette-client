#include "ui/home_page.h"
#include <stdio.h>
#include <SFML/Graphics.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

MenuAction home_page() {
    sfRenderWindow* window = sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Roulette - Main Page",
        sfTitlebar | sfClose,
        NULL
    );

    // 加载字体
    sfFont* font = sfFont_createFromFile("../assets/fonts/arial.ttf");
    if (!font) {
        fprintf(stderr, "Error loading font!\n");
        return ACTION_EXIT;
    }

    // 初始化按钮
    Button buttons[3];
    init_button(&buttons[0], "PVP Fight", (sfVector2f){300, 200}, font);
    init_button(&buttons[1], "PVE Fight", (sfVector2f){300, 300}, font);
    init_button(&buttons[2], "Exit", (sfVector2f){300, 400}, font);
    buttons[0].action = ACTION_PVP;
    buttons[1].action = ACTION_PVE;
    buttons[2].action = ACTION_EXIT;

    // 标题文字
    sfText* title = sfText_create();
    sfText_setString(title, "Roulette");
    sfText_setFont(title, font);
    sfText_setCharacterSize(title, 48);
    sfText_setFillColor(title, sfColor_fromRGB(200, 50, 50));
    sfFloatRect titleRect = sfText_getLocalBounds(title);
    sfText_setPosition(title, (sfVector2f){
                           (WINDOW_WIDTH - titleRect.width) / 2,
                           50
                       });

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        MenuAction selected_action = ACTION_NONE;

        // 处理事件
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
                selected_action = ACTION_EXIT;
            }

            // 鼠标移动检测
            if (event.type == sfEvtMouseMoved) {
                sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);
                for (int i = 0; i < 3; i++) {
                    sfFloatRect bounds = sfRectangleShape_getGlobalBounds(buttons[i].shape);
                    buttons[i].is_hovered = sfFloatRect_contains(&bounds, mousePos.x, mousePos.y);
                }
            }

            // 鼠标点击处理
            if (event.type == sfEvtMouseButtonPressed) {
                if (event.mouseButton.button == sfMouseLeft) {
                    sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);
                    for (int i = 0; i < 3; i++) {
                        sfFloatRect bounds = sfRectangleShape_getGlobalBounds(buttons[i].shape);
                        if (sfFloatRect_contains(&bounds, mousePos.x, mousePos.y)) {
                            selected_action = buttons[i].action;
                        }
                    }
                }
            }
        }

        // 更新按钮状态
        for (int i = 0; i < 3; i++) {
            if (buttons[i].is_hovered) {
                sfRectangleShape_setFillColor(buttons[i].shape, sfColor_fromRGB(100, 100, 100));
                sfText_setStyle(buttons[i].text, sfTextBold);
            }
            else {
                sfRectangleShape_setFillColor(buttons[i].shape, sfColor_fromRGB(70, 70, 70));
                sfText_setStyle(buttons[i].text, sfTextRegular);
            }
        }

        // 渲染
        sfRenderWindow_clear(window, sfColor_fromRGB(40, 40, 40));
        sfRenderWindow_drawText(window, title, NULL);

        for (int i = 0; i < 3; i++) {
            sfRenderWindow_drawRectangleShape(window, buttons[i].shape, NULL);
            sfRenderWindow_drawText(window, buttons[i].text, NULL);
        }

        sfRenderWindow_display(window);

        // 处理选择结果
        if (selected_action != ACTION_NONE) {
            // 清理资源
            for (int i = 0; i < 3; i++) {
                sfRectangleShape_destroy(buttons[i].shape);
                sfText_destroy(buttons[i].text);
            }
            sfText_destroy(title);
            sfFont_destroy(font);
            sfRenderWindow_destroy(window);
            return selected_action;
        }
    }

    // 默认退出
    sfFont_destroy(font);
    sfRenderWindow_destroy(window);
    return ACTION_EXIT;
}