#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <stdbool.h>

#include "ui_composition.h"

typedef enum {
    CONN_STATUS_TRYING,
    CONN_STATUS_SUCCESS,
    CONN_STATUS_FAILED
} ConnectionStatus;

MenuAction show_main_menu();

void show_waiting_ui();

void show_connection_result(const char* result_message);

bool show_retry_dialog();

#endif // UICONTROLLER_H
