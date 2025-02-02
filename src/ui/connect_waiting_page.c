#include "ui/connect_waiting_page.h"
#include "ui/ui_composition.h"
#include "network_controller.h"
#include <stdio.h>
#include <math.h>
#include <SFML/System.h>
#include <SFML/Graphics.h>
#include <SFML/Window.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

void connect_waiting_page() {
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

void connection_result(const char* result_message) {
    sfRenderWindow* window = sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        "Connection Result: ", sfTitlebar | sfClose, NULL);
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
    sfText_setPosition(status_text, (sfVector2f){
                           (WINDOW_WIDTH - textBounds.width) / 2, (WINDOW_HEIGHT / 2 - 50)
                       });

    // **创建OK按钮**
    sfRectangleShape* ok_button = sfRectangleShape_create();
    sfRectangleShape_setSize(ok_button, (sfVector2f){120, 50});
    sfRectangleShape_setFillColor(ok_button, sfColor_fromRGB(100, 200, 100));
    sfRectangleShape_setPosition(ok_button, (sfVector2f){
                                     (WINDOW_WIDTH - 120) / 2,
                                     (WINDOW_HEIGHT / 2 + 20)
                                 });

    // **OK按钮上的文本**
    sfText* ok_text = sfText_create();
    sfText_setFont(ok_text, font);
    sfText_setCharacterSize(ok_text, 24);
    sfText_setFillColor(ok_text, sfBlack);
    sfText_setString(ok_text, "OK");

    // 让OK文本在按钮中居中
    sfFloatRect okBounds = sfText_getLocalBounds(ok_text);
    sfText_setPosition(ok_text, (sfVector2f){
                           (WINDOW_WIDTH - okBounds.width) / 2,
                           (WINDOW_HEIGHT / 2 + 30)
                       });

    bool running = true;
    while (running) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtMouseButtonPressed) {
                sfVector2i mousePos = sfMouse_getPositionRenderWindow(window);
                sfVector2f btnPos = sfRectangleShape_getPosition(ok_button);
                sfVector2f btnSize = sfRectangleShape_getSize(ok_button);

                // 判断鼠标是否点击在OK按钮范围内
                if (mousePos.x >= btnPos.x && mousePos.x <= btnPos.x + btnSize.x &&
                    mousePos.y >= btnPos.y && mousePos.y <= btnPos.y + btnSize.y) {
                    running = false;  // 关闭弹窗
                    }
            }
        }

        // **渲染窗口**
        sfRenderWindow_clear(window, sfColor_fromRGB(40, 40, 40));
        sfRenderWindow_drawText(window, status_text, NULL);
        sfRenderWindow_drawRectangleShape(window, ok_button, NULL);
        sfRenderWindow_drawText(window, ok_text, NULL);
        sfRenderWindow_display(window);
    }

    // 释放资源
    sfText_destroy(status_text);
    sfFont_destroy(font);
    sfRenderWindow_close(window);
    sfRenderWindow_destroy(window);
}

bool retry_dialog() {
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
