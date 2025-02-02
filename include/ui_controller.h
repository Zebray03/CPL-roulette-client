#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include "ui/ui_composition.h"
#include <stdbool.h>
#include <SFML/System/Types.h>

MenuAction show_home_page();

void show_connect_waiting_page();

void show_connection_result(const char* result_message);

bool show_retry_dialog();

#endif // UICONTROLLER_H
