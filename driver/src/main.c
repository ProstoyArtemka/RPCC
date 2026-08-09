#include "raylib.h"
#include "stdio.h"

#include <winapi.h>
#include <uart.h>
#include <graphics.h>

char port_state[64] = "Nothing connected yet";

void rpcc_connected(int port) {

    snprintf(port_state, sizeof(port_state), "RPCC connected to: COM%d", port);

}

int main(void) {

    start_ports_scan(rpcc_connected);

    SetTraceLogLevel(LOG_WARNING);
    InitWindow(1280, 720, "RPCC");
    SetTargetFPS(60);

    void* window = GetWindowHandle();
    add_connected_device_callback(window, rpcc_connected);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(RAYWHITE);

        draw_centered_text(port_state, 64, GetScreenWidth() / 2, GetScreenHeight() / 2, BLACK);

        EndDrawing();
    }


    remove_connected_device_callback(window);
    CloseWindow();
    
    return 0;
}