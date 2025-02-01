#include "ui_controller.h"

#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
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

MenuAction show_main_menu() {
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
        (WINDOW_WIDTH - titleRect.width)/2,
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
            } else {
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

void show_loading_screen(LoadingScreen* screen, sfRenderWindow* window) {

    // 初始化转圈动画
    screen->spinner = sfCircleShape_create();
    sfCircleShape_setRadius(screen->spinner, 30);
    sfCircleShape_setFillColor(screen->spinner, sfTransparent);
    sfCircleShape_setOutlineThickness(screen->spinner, 3);
    sfCircleShape_setOutlineColor(screen->spinner, sfWhite);
    sfCircleShape_setOrigin(screen->spinner, (sfVector2f){30, 30});
    sfCircleShape_setPosition(screen->spinner, (sfVector2f){400, 300});

    // 状态文字
    screen->status_text = sfText_create();
    sfFont* font = sfFont_createFromFile("../assets/fonts/arial.ttf");
    sfText_setFont(screen->status_text, font); // 复用主菜单字体
    sfText_setCharacterSize(screen->status_text, 24);
    sfText_setFillColor(screen->status_text, sfWhite);
    sfText_setPosition(screen->status_text, (sfVector2f){300, 400});

    // 状态同步
    screen->connection_status_mutex = sfMutex_create();
    screen->status = CONN_STATUS_TRYING;
}

void update_loading_screen(LoadingScreen* screen, sfRenderWindow* window) {
    static float angle = 0;
    angle += 2.5f; // 控制旋转速度

    // 更新转圈动画
    sfCircleShape_setRotation(screen->spinner, angle);

    // 更新状态文字
    sfMutex_lock(screen->connection_status_mutex);
    const char* status_msg = "";
    switch (screen->status) {
        case CONN_STATUS_TRYING:
            status_msg = "Connecting...";
            break;
        case CONN_STATUS_SUCCESS:
            status_msg = "Success";
            break;
        case CONN_STATUS_FAILED:
            status_msg = "Fail";
            break;
    }
    sfMutex_unlock(screen->connection_status_mutex);
    sfText_setString(screen->status_text, status_msg);

    // 居中文字
    sfFloatRect text_rect = sfText_getLocalBounds(screen->status_text);
    sfText_setOrigin(screen->status_text, (sfVector2f){text_rect.width/2, 0});
    sfText_setPosition(screen->status_text, (sfVector2f){400, 400});

    // 渲染
    sfRenderWindow_clear(window, sfColor_fromRGB(40, 40, 40));
    sfRenderWindow_drawCircleShape(window, screen->spinner, NULL);
    sfRenderWindow_drawText(window, screen->status_text, NULL);
    sfRenderWindow_display(window);
}

void destroy_loading_screen(LoadingScreen* screen) {
    sfCircleShape_destroy(screen->spinner);
    sfText_destroy(screen->status_text);
    sfMutex_destroy(screen->connection_status_mutex);
}
