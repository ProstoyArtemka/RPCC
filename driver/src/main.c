#include "raylib.h"
#include "stdio.h"

#include <string.h>
#include <stdlib.h>
#include <winapi.h>
#include <uart.h>
#include <graphics.h>

bool is_hidden = false;
char port_state[64] = "Nothing connected yet";
int rpcc_port = -1;

void* window;

void rpcc_connected(int port) {

    snprintf(port_state, sizeof(port_state), "RPCC connected to: COM%d", port);

    rpcc_port = port;

    update_tray_icon(window, true);
    show_tray_notification(window, "RPCC connected!", "Device connected and ready to work.");

}

void rpcc_disconnected(int port) {

    if (port != rpcc_port) return;

    update_tray_icon(window, false);
    strcpy(port_state, "Nothing connected yet");
    show_tray_notification(window, "RPCC disconnected!", "Device disconected.");

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

int main(void) {

    start_ports_scan(rpcc_connected);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1280, 720, "RPCC Driver");
    SetTargetFPS(60);

    Image icon = LoadImage("assets/icon.png");
    SetWindowIcon(icon);
    UnloadImage(icon);

    SetExitKey(KEY_NULL);

    window = GetWindowHandle();

    WINDOW_SUBCLASS_PARAMS *window_params = malloc(sizeof(WINDOW_SUBCLASS_PARAMS));
    window_params->device_connected_callback = rpcc_connected;
    window_params->device_disconnected_callback = rpcc_disconnected;
    window_params->icon_callback = tray_icon_pressed;
    window_params->close_callback = window_closed;

    add_window_subclass(window, window_params);
    add_tray_icon(window);

    while (1) {

        if (is_hidden) {

            keep_window_alive();

            continue;
        }

        BeginDrawing();

        ClearBackground(RAYWHITE);
        draw_centered_text(port_state, 64, GetScreenWidth() / 2, GetScreenHeight() / 2, BLACK);

        EndDrawing();
    }

    remove_window_subclass(window);
    CloseWindow();

    free(window_params);
    
    return 0;
}