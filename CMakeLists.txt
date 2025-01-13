cmake_minimum_required(VERSION 3.30)
project(CPL_roulette_client C)

set(CMAKE_C_STANDARD 11)

add_executable(CPL_roulette_client src/main.c
        src/network_controller.c
        include/network_controller.h
        src/game_controller.c
        include/game_controller.h
        src/ui_controller.c
        include/ui_controller.h)

link_libraries(ws2_32)
target_link_libraries(CPL_roulette_client ws2_32)
