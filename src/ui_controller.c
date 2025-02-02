#include "ui_controller.h"
#include "ui/connect_waiting_page.h"
#include "ui/home_page.h"

MenuAction show_home_page() {
    return home_page();
}

void show_connect_waiting_page() {
    connect_waiting_page();
}

void show_connection_result(const char* result_message) {
    connection_result(result_message);
}

bool show_retry_dialog() {
    return retry_dialog();
}
