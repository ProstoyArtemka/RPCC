#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"

#define BASE_WIDTH 1280
#define BASE_HEIGHT 720

#define VIRTUAL_WIDTH 1920
#define VIRTUAL_HEIGHT 1080

#define BACKGROUND_DARK (Color) { 3, 3, 3, 255 }
#define BACKGROUND (Color) { 11, 11, 11, 255 }
#define BACKGROUND_LIGHT (Color) { 22, 22, 22, 255 }

#define STATUS_BAR_CONNECTED (Color) { 11, 33, 11, 255 }
#define STATUS_BAR_DISCONNECTED (Color) { 33, 11, 11, 255 }

#define BUTTON_DEFAULT_BACKGROUND (Color) { 33, 33, 33, 255 }
#define BUTTON_DEFAULT_STROKE (Color) { 72, 72, 72, 255 }

typedef struct {

    bool is_rpcc_connected;
    int rpcc_port;

} RPCC_STATE;

void draw_gui(RPCC_STATE *state, Font *font);

#endif