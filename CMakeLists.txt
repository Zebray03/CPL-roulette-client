cmake_minimum_required(VERSION 3.30)
project(CPL_roulette_server C)

set(CMAKE_C_STANDARD 11)

set(CJSON_SOURCES
        lib/cJSON/cJSON.c
)

set(CJSON_HEADERS
        lib/cJSON/cJSON.h
)

add_executable(
        CPL_roulette_client
        include/ui/connect_waiting_page.h
        include/game.h
        include/ui/home_page.h
        include/main.h
        include/network_controller.h
        include/network_protocol.h
        include/player.h
        include/ui/ui_composition.h
        include/ui_controller.h

        src/ui/connect_waiting_page.c
        src/game.c
        src/ui/home_page.c
        src/main.c
        src/network_controller.c
        src/player.c
        src/ui/ui_composition.c
        src/ui_controller.c

        ${CJSON_SOURCES}
        ${CJSON_HEADERS}
)

link_libraries(ws2_32)

set(CSFML_DIR "${CMAKE_SOURCE_DIR}/lib/CSFML")

target_include_directories(CPL_roulette_client PRIVATE
        "${CSFML_DIR}/include"
        include
        lib/cJSON
)

target_link_directories(CPL_roulette_client PRIVATE
        "${CSFML_DIR}/lib/gcc"
)

target_link_libraries(
        CPL_roulette_client
        ws2_32
        csfml-graphics
        csfml-window
        csfml-system
)
