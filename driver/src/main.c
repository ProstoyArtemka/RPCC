#include "raylib.h"
#include "stdio.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <winapi.h>
#include <uart.h>

#include <graphics.h>
#include <config.h>
#include <inputs.h>

RPCCState state = (RPCCState) {
    .is_rpcc_connected = false,
    .rpcc_port = -1
};

Config config = (Config) {

    .buttons = (ButtonConfig[]) {

        (ButtonConfig) { .action = NOTHING },
        (ButtonConfig) { .action = NOTHING },
        (ButtonConfig) { .action = NOTHING },

    }

};

GUIContext context = (GUIContext) {
    .connected_animation = 0.0,
    .configuration_animation = 0.0,
    .status_color_animation = 0.0,

    .is_configuring = false,

    .current_button = -1,
    .configuration_options = {NOTHING, NOTHING, NOTHING},
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

void init_config() {

    ConfigReadResult result = load_config(&config);
    if (result == CONFIG_READ_SUCCESS) {

        for (size_t i = 0; i < sizeof(context.configuration_options) / sizeof(ButtonAction); i++)
            context.configuration_options[i] = config.buttons[i].action;

        return;
    }

    if (save_config(&config) == CONFIG_SAVE_ERROR) exit(-1);
}

int main(void) {

    TraceLog(LOG_INFO, "Проверка UTF-8 в консоли: %s", "Привет");

    init_config();

    start_ports_scan(rpcc_connected);

    WINDOW_SUBCLASS_PARAMS *window_params = malloc(sizeof(WINDOW_SUBCLASS_PARAMS));
    init_window(window_params);
    
    load_gui(&context);

    while (1) {

        if (is_hidden) {

            keep_window_alive();

            continue;
        }

        if (IsKeyReleased(KEY_Q)) {
            state.is_rpcc_connected = !state.is_rpcc_connected;
        }

        process_inputs(&context, &state, &config, window);        
        render_screen(&context, &state);
    }

    remove_window_subclass(window);
    CloseWindow();

    unload_gui(&context);
    unload_config(&config);
    
    free(window_params);
    
    return 0;
}