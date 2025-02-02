#ifndef CONNECT_WAITING_PAGE_H
#define CONNECT_WAITING_PAGE_H

#include <stdbool.h>

typedef enum {
    CONN_STATUS_TRYING,
    CONN_STATUS_SUCCESS,
    CONN_STATUS_FAILED
} ConnectionStatus;

void connect_waiting_page();

void connection_result(const char* result_message);

bool retry_dialog();

#endif //CONNECT_WAITING_PAGE_H
