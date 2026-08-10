#include "raylib.h"
#include "stdio.h"

#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <winapi.h>
#include <uart.h>
#include <graphics.h>

RPCC_STATE *state;

bool is_hidden = false;
void* window;

void rpcc_connected(int port) {

    update_tray_icon(window, true);
    show_tray_notification(window, "RPCC connected!", "Device connected and ready to work.");

    state->rpcc_port = port;
    state->is_rpcc_connected = true;

}

void rpcc_disconnected(int port) {

    if (port != state->rpcc_port) return;

    update_tray_icon(window, false);
    show_tray_notification(window, "RPCC disconnected!", "Device disconected.");

    state->rpcc_port = -1;
    state->is_rpcc_connected = false;
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

    Image icon = LoadImage("assets/icon.png");
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

    state = malloc(sizeof(RPCC_STATE));
    state->is_rpcc_connected = false;
    state->rpcc_port = -1;

    start_ports_scan(rpcc_connected);

    WINDOW_SUBCLASS_PARAMS *window_params = malloc(sizeof(WINDOW_SUBCLASS_PARAMS));
    init_window(window_params);

    Font font = LoadFontEx("assets/inter.ttf", 96, NULL, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);

    RenderTexture2D target = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(target.texture, TEXTURE_FILTER_BILINEAR);

    while (1) {

        if (is_hidden) {

            keep_window_alive();

            continue;
        }

        // if (IsKeyPressed(KEY_Q)) {
        //     state->is_rpcc_connected = !state->is_rpcc_connected;
        // }

        BeginTextureMode(target);

        draw_gui(state, &font);

        EndTextureMode();

        ClearBackground(BACKGROUND_DARK);

        BeginDrawing();

        float scale = fminf((float) GetScreenWidth() / VIRTUAL_HEIGHT, (float) GetScreenHeight() / VIRTUAL_HEIGHT);
        Rectangle sourceRec = { 0.0f, 0.0f, (float) VIRTUAL_WIDTH, -(float)VIRTUAL_HEIGHT };
        Rectangle destRec = {
            ((float) GetScreenWidth() - ((float) VIRTUAL_WIDTH * scale)),
            ((float) GetScreenHeight() - ((float) VIRTUAL_HEIGHT * scale)),
            (float) VIRTUAL_WIDTH * scale,
            (float) VIRTUAL_HEIGHT * scale
        };

        DrawTexturePro(
            target.texture,
            sourceRec,
            destRec,
            (Vector2) {0, 0},
            0.0f, 
            WHITE
        );

        EndDrawing();
    }

    remove_window_subclass(window);
    CloseWindow();

    UnloadFont(font);
    UnloadRenderTexture(target);

    free(window_params);
    free(state);
    
    return 0;
}