#ifndef MAIN_H
#define MAIN_H

typedef enum {
    MAIN_MENU,      // 主菜单
    CONNECTING,     // 连接服务器
    GAMING,           // 游戏状态（后续实现）
    EXIT            // 退出状态
} GlobalState;

extern GlobalState gs;

#endif //MAIN_H
