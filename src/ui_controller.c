#include "ui_controller.h"
#include "network_controller.h"
#include <stdio.h>
#include <math.h>

#include "main.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

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

void show_waiting_ui() {
    // 创建窗口
    sfRenderWindow* window = sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Connect", sfTitlebar | sfClose, NULL);
    if (!window)
        return;

    // 创建多个小圆点组成的转圈动画
    const int num_dots = 8;  // 转圈的圆点数量
    sfCircleShape* dots[num_dots];  // 存储小圆点
    float radius = 50.0f;  // 圆环半径

    // 初始化小圆点
    for (int i = 0; i < num_dots; i++) {
        dots[i] = sfCircleShape_create();
        sfCircleShape_setRadius(dots[i], 10);  // 每个小圆点的大小
        sfCircleShape_setFillColor(dots[i], sfColor_fromRGB(255, 255, 255));  // 设置颜色为白色
    }

    // 创建状态文本
    sfFont* font = sfFont_createFromFile("../assets/fonts/arial.ttf");
    sfText* status_text = sfText_create();
    sfText_setFont(status_text, font);
    sfText_setCharacterSize(status_text, 24);
    sfText_setFillColor(status_text, sfColor_fromRGB(255, 255, 255));
    sfText_setString(status_text, "Connecting...");
    sfFloatRect text_bounds = sfText_getLocalBounds(status_text);
    sfText_setPosition(status_text, (sfVector2f){(WINDOW_WIDTH - text_bounds.width) / 2, (WINDOW_HEIGHT + 100) / 2});

    // 动画旋转速度
    float rotation_speed = 0.0005f;  // 旋转速度控制
    float rotation_angle = 0.0f;  // 当前旋转角度

    // 主动画循环
    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed) {
                sfRenderWindow_close(window);
                g_connection_interrupt = true;
                break;
            }
        }

        // 每帧旋转所有小圆点
        for (int i = 0; i < num_dots; i++) {
            // 计算当前圆点的角度
            float angle = rotation_angle + i * (2 * M_PI / num_dots);

            // 计算圆点的新位置
            float x = WINDOW_WIDTH / 2 + radius * cos(angle);
            float y = WINDOW_HEIGHT / 2 + radius * sin(angle);

            // 设置小圆点的位置
            sfCircleShape_setPosition(dots[i], (sfVector2f){x, y});
        }

        // 每帧增加旋转角度，控制旋转速度
        rotation_angle += rotation_speed;

        // 检查网络线程是否完成
        sfMutex_lock(connection_mutex);
        bool finished = g_connection_finished;
        sfMutex_unlock(connection_mutex);
        if (finished) {
            break;
        }

        // 渲染
        sfRenderWindow_clear(window, sfColor_fromRGB(40, 40, 40));  // 设置背景颜色
        for (int i = 0; i < num_dots; i++) {
            sfRenderWindow_drawCircleShape(window, dots[i], NULL);  // 绘制每个小圆点
        }
        sfRenderWindow_drawText(window, status_text, NULL);  // 绘制状态文本
        sfRenderWindow_display(window);  // 显示窗口内容
    }

    // 释放资源
    for (int i = 0; i < num_dots; i++) {
        sfCircleShape_destroy(dots[i]);  // 销毁小圆点
    }
    sfText_destroy(status_text);  // 销毁状态文本
    sfFont_destroy(font);  // 销毁字体
    sfRenderWindow_close(window);  // 关闭窗口
    sfRenderWindow_destroy(window);  // 销毁窗口
}

void show_connection_result(const char* result_message) {
    sfRenderWindow* window = sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Result: ", sfTitlebar | sfClose, NULL);
    if (!window)
        return;

    // 创建状态文本
    sfFont* font = sfFont_createFromFile("../assets/fonts/arial.ttf");
    sfText* status_text = sfText_create();
    sfText_setFont(status_text, font);
    sfText_setCharacterSize(status_text, 24);
    sfText_setFillColor(status_text, sfColor_fromRGB(255, 255, 255));
    sfText_setString(status_text, result_message);
    sfFloatRect textBounds = sfText_getLocalBounds(status_text);
    sfText_setPosition(status_text, (sfVector2f){(WINDOW_WIDTH - textBounds.width) / 2, (WINDOW_HEIGHT + 100) / 2});

    // 显示连接结果
    sfRenderWindow_clear(window, sfColor_fromRGB(40, 40, 40));
    sfRenderWindow_drawText(window, status_text, NULL);
    sfRenderWindow_display(window);

    // 等待用户关闭窗口
    sfEvent event;
    while (sfRenderWindow_pollEvent(window, &event)) {
        if (event.type == sfEvtClosed)
            sfRenderWindow_close(window);
    }

    // 释放资源
    sfText_destroy(status_text);
    sfFont_destroy(font);
    sfRenderWindow_close(window);
    sfRenderWindow_destroy(window);
}

bool show_retry_dialog() {
    sfRenderWindow* window = sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Timeout", sfTitlebar | sfClose, NULL);
    if (!window)
        return false;

    // 创建状态文本
    sfFont* font = sfFont_createFromFile("../assets/fonts/arial.ttf");
    sfText* status_text = sfText_create();
    sfText_setFont(status_text, font);
    sfText_setCharacterSize(status_text, 24);
    sfText_setFillColor(status_text, sfColor_fromRGB(255, 255, 255));
    sfText_setString(status_text, "Retry?");
    sfFloatRect textBounds = sfText_getLocalBounds(status_text);
    sfText_setPosition(status_text, (sfVector2f){(WINDOW_WIDTH - textBounds.width) / 2, (WINDOW_HEIGHT - 100) / 2});

    // 创建按钮区域：简单的区分“重试”和“退出”区域
    sfRectangleShape* retry_button = sfRectangleShape_create();
    sfRectangleShape_setSize(retry_button, (sfVector2f){200, 50});
    sfRectangleShape_setPosition(retry_button, (sfVector2f){(WINDOW_WIDTH - 200) / 2, (WINDOW_HEIGHT - 50) / 2 + 50});
    sfRectangleShape_setFillColor(retry_button, sfColor_fromRGB(0, 255, 0));

    sfRectangleShape* exit_button = sfRectangleShape_create();
    sfRectangleShape_setSize(exit_button, (sfVector2f){200, 50});
    sfRectangleShape_setPosition(exit_button, (sfVector2f){(WINDOW_WIDTH - 200) / 2, (WINDOW_HEIGHT - 50) / 2 + 120});
    sfRectangleShape_setFillColor(exit_button, sfColor_fromRGB(255, 0, 0));

    // 等待用户重试或退出
    bool retry = false;
    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);

            if (event.type == sfEvtMouseButtonPressed) {
                sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);

                // 判断点击是否在按钮区域内
                if (mousePos.x > (WINDOW_WIDTH - 200) / 2 && mousePos.x < (WINDOW_WIDTH + 200) / 2 &&
                    mousePos.y > (WINDOW_HEIGHT - 50) / 2 + 50 && mousePos.y < (WINDOW_HEIGHT + 50) / 2 + 50) {
                    retry = true;  // 重试
                    sfRenderWindow_close(window);
                }
                else if (mousePos.x > (WINDOW_WIDTH - 200) / 2 && mousePos.x < (WINDOW_WIDTH + 200) / 2 &&
                    mousePos.y > (WINDOW_HEIGHT - 50) / 2 + 120 && mousePos.y < (WINDOW_HEIGHT + 50) / 2 + 120) {
                    retry = false;  // 退出
                    sfRenderWindow_close(window);
                }
            }
        }

        // 渲染
        sfRenderWindow_clear(window, sfColor_fromRGB(40, 40, 40));
        sfRenderWindow_drawText(window, status_text, NULL);
        sfRenderWindow_drawRectangleShape(window, retry_button, NULL);
        sfRenderWindow_drawRectangleShape(window, exit_button, NULL);
        sfRenderWindow_display(window);
    }

    // 释放资源
    sfText_destroy(status_text);
    sfFont_destroy(font);
    sfRectangleShape_destroy(retry_button);
    sfRectangleShape_destroy(exit_button);
    sfRenderWindow_close(window);
    sfRenderWindow_destroy(window);

    return retry;
}
