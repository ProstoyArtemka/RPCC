#include "raylib.h"
#include "stdio.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <winapi.h>
#include <uart.h>
#include <graphics.h>

RPCCState state = (RPCCState) {
    .is_rpcc_connected = false,
    .rpcc_port = -1
};

bool is_hidden = false;
void* window;

void rpcc_connected(int port) {

    update_tray_icon(window, true);
    show_tray_notification(window, "RPCC connected!", "Device connected and ready to work.");

    state.rpcc_port = port;
    state.is_rpcc_connected = true;

}

void rpcc_disconnected(int port) {

    if (port != state.rpcc_port) return;

    update_tray_icon(window, false);
    show_tray_notification(window, "RPCC disconnected!", "Device disconected.");

    state.rpcc_port = -1;
    state.is_rpcc_connected = false;
}

void tray_icon_pressed() {

    is_hidden = false;

    RestoreWindow();
    ClearWindowState(FLAG_WINDOW_HIDDEN);

}

void window_closed() {

    SetWindowState(FLAG_WINDOW_HIDDEN);

    is_hidden = true;

}

void init_window(WINDOW_SUBCLASS_PARAMS *window_params) {

    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(BASE_WIDTH, BASE_HEIGHT, "RPCC Driver");
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);

    Image icon = LoadImage("assets/icons/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    window = GetWindowHandle();

    window_params->device_connected_callback = rpcc_connected;
    window_params->device_disconnected_callback = rpcc_disconnected;
    window_params->icon_callback = tray_icon_pressed;
    window_params->close_callback = window_closed;

    add_window_subclass(window, window_params);
    add_tray_icon(window);

}

int main(void) {

    start_ports_scan(rpcc_connected);

    WINDOW_SUBCLASS_PARAMS *window_params = malloc(sizeof(WINDOW_SUBCLASS_PARAMS));
    init_window(window_params);

    GUIContext context = (GUIContext) {
        .connected_animation = 0.0,
        .configuration_animation = 0.0,
        .status_color_animation = 0.0,

        .is_configuring = false,

        .selected_button = -1,
        .selected_configuration_option = 4
    };

    load_gui(&context);

    while (1) {

        if (is_hidden) {

            keep_window_alive();

            continue;
        }

        if (IsKeyReleased(KEY_Q)) {
            state.is_rpcc_connected = !state.is_rpcc_connected;
        }

        render_screen(&context, &state);
    }

    remove_window_subclass(window);
    CloseWindow();

    unload_gui(&context);
    
    free(window_params);
    
    return 0;
}