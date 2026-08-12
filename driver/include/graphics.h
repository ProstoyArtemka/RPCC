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

#define BUTTON_DEFAULT_BACKGROUND (Color) { 22, 22, 22, 255 }
#define BUTTON_DEFAULT_STROKE (Color) { 72, 72, 72, 255 }

#define BUTTON_HOVER_BACKGROUND (Color) { 44, 44, 44, 255 }
#define BUTTON_HOVER_STROKE (Color) { 92, 92, 92, 255 }

typedef struct {

    bool is_rpcc_connected;
    int rpcc_port;

} RPCCState;

typedef struct {

    RenderTexture2D screen_texture;
    Font font;
    Vector2 mouse_position;

    float delta;

    float connected_animation;
    float configuration_animation;
    float status_color_animation;

    int selected_button;

    char status_text[32];
    char configuration_title[64];

    bool is_configuring;

} GUIContext;

typedef struct {

    Rectangle rect;

    char *text;

    float hovering_progress;

    void (*callback) (GUIContext*);

} CharButton;

typedef struct {

    Rectangle rect;

    char* path;
    Texture2D texture;

    float hovering_progress;

    void (*callback) (GUIContext*);

} IconButton;

void load_gui(GUIContext *context);
void unload_gui(GUIContext *context);

void render_screen(GUIContext *context, RPCCState*state);

#endif