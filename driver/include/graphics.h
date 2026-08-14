#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "raylib.h"

#include <config.h>

#define BASE_WIDTH 1280
#define BASE_HEIGHT 720

#define VIRTUAL_WIDTH 1920
#define VIRTUAL_HEIGHT 1080

#define TRANSPARENT (Color) { 0, 0, 0, 0 }

#define BACKGROUND_DARK (Color) { 0, 0, 0, 255 }
#define BACKGROUND (Color) { 0, 0, 0, 255 }
#define BACKGROUND_LIGHT (Color) { 22, 22, 22, 255 }

#define STATUS_BAR_CONNECTED (Color) { 11, 33, 11, 255 }
#define STATUS_BAR_DISCONNECTED (Color) { 33, 11, 11, 255 }

#define BUTTON_DEFAULT_BACKGROUND (Color) { 0, 0, 0, 255 }
#define BUTTON_DEFAULT_STROKE (Color) { 72, 72, 72, 255 }

#define BUTTON_HOVER_BACKGROUND (Color) { 22, 22, 22, 255 }
#define BUTTON_HOVER_STROKE (Color) { 92, 92, 92, 255 }

#define OPTION_DEFAULT_BACKGROUND (Color) { 0, 0, 0, 255 }
#define OPTION_DEFAULT_STROKE (Color) { 72, 72, 72, 255 }

#define OPTION_HOVER_BACKGROUND (Color)  { 22, 22, 22, 255 }
#define OPTION_HOVER_STROKE (Color) { 72, 72, 72, 255 }

#define OPTION_SELECTED_BACKGROUND (Color) { 255, 255, 255, 255 }
#define OPTION_SELECTED_STROKE (Color) { 72, 72, 72, 255 }

#define CHECKBOX_DEFAULT_BACKGROUND (Color) { 0, 0, 0, 255 }
#define CHECKBOX_HOVER_BACKGROUND (Color) { 22, 22, 22, 255 }
#define CHECKBOX_PRESSED_BACKGROUND (Color) { 255, 255, 255, 255 } 

#define CHECKBOX_DEFAULT_STROKE (Color) { 72, 72, 72, 255 }

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

    int current_button;
    ButtonAction configuration_options[3];

    char status_text[32];

    bool is_configuring;

} GUIContext;

typedef struct {

    Rectangle rect;

    char *text;

    float hovering_progress;

} CharButton;

typedef struct {

    Rectangle rect;

    char* path;
    Texture2D texture;

} Icon;

typedef struct {

    Rectangle rect;

    Icon icon;

    float hovering_progress;

} IconButton;

typedef struct {

    Rectangle rect;

    Icon icon;

    float hovering_progress;
    float press_progress;

} ConfigurationOption;

typedef struct {

    Rectangle rect;

    char *text;

    float hovering_progress;

} TextLineInput;

typedef struct {

    Rectangle rect;

    bool checked;

    float hovering_progress;
    float pressed_progress;
    
} Checkbox;


extern CharButton select_buttons[3];
extern IconButton exit_button;
extern ConfigurationOption options[5];
extern TextLineInput open_file_input;
extern Checkbox as_administrator_checkbox;

bool is_mouse_hovers_rect(GUIContext *context, Rectangle *rectangle);

void load_gui(GUIContext *context);
void unload_gui(GUIContext *context);

void render_screen(GUIContext *context, RPCCState*state);

#endif