#include <graphics.h>

#include "reasings.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

float connected_animation = 0;
char status_text[] = "RPC disconnected"; 

int clamp_int(int x, int low, int high) {
    if (x < low) return low;
    if (x > high) return high;

    return x;
}

void draw_text(Font font, char *text, Vector2 position, float size, float spacing, Color color) {
    DrawTextEx(font, text, position, size * 1.2f, spacing, color);
}

void animate_connection(bool connected) {

    if (connected) {
        connected_animation += GetFrameTime();
        connected_animation = fminf(connected_animation, 1.0f);

        return;
    }

    connected_animation -= GetFrameTime();
    connected_animation = fmaxf(connected_animation, 0.0f);

}

void smooth_text(float ease, char* prefix, char* smooth, char* suffix, char* output) {

    int smooth_len = strlen(smooth);
    int visible = (int) ((1.0 - ease) * smooth_len + 0.5);
    visible = clamp_int(visible, 0, smooth_len);

    snprintf(output, 32, "%s%.*s%s", prefix, visible, smooth, suffix);

}

void draw_status(Font *font) {

    float ease = EaseCircInOut(connected_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float rect_x = 600.0 + (50.0 * ease);
    float rect_y = 75.0 + (390.0 * invert_ease);
    float rect_width = 620 + (70. * invert_ease);
    float rect_height = 150.0;
    
    Rectangle status_bg_rectangle = (Rectangle) { rect_x, rect_y, rect_width, rect_height }; 
    Color status_bg_color = (Color) { 11 + (22 * invert_ease), 11 + (22 * ease), 11, 255 };

    Rectangle stroke_bg_rectangle = (Rectangle) { rect_x - 5, rect_y - 5, rect_width + 10, rect_height + 10 };

    DrawRectangleRounded(stroke_bg_rectangle, 0.5f, 16, (Color) { 255 * invert_ease, 255 * ease, 0, 255 });
    DrawRectangleRounded(status_bg_rectangle, 0.5f, 16, status_bg_color);

    float circle_x = rect_x + 75;
    float circle_y = rect_y + (rect_height / 4.0) + 37.5;

    DrawCircle(circle_x, circle_y, 37.5, (Color) {255 * invert_ease, 255 * ease, 0, 255});

    float text_x = 743 + (50 * ease);
    float text_y = 118 + (391 * invert_ease);

    smooth_text(ease, "RPCC ", "dis", "connected", status_text);

    draw_text(*font, status_text, (Vector2) { text_x, text_y }, 54, 1.5f, WHITE);
}

void draw_button(Font *font, char *text, int index) {
    float ease = EaseCircInOut(connected_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float rect_x = 417.0 + (418.0 * index) + (125.0 * invert_ease);
    float rect_y = 415.0 + (125.0 * invert_ease);
    float rect_size = 250.0 * ease;

    Rectangle rect = (Rectangle) { rect_x, rect_y, rect_size, rect_size };
    Rectangle stroke_rect = (Rectangle) { rect_x - 5, rect_y - 5, rect_size + 10, rect_size + 10 };

    DrawRectangleRounded(stroke_rect, 0.5f, 16, BUTTON_DEFAULT_STROKE);
    DrawRectangleRounded(rect, 0.5f, 16, BUTTON_DEFAULT_BACKGROUND);
    
    Vector2 text_size = MeasureTextEx(*font, text, 96 * ease, 1.5);

    float text_x = rect_x + (rect_size / 2.0) - (text_size.x / 2.0);
    float text_y = rect_y + (rect_size / 2.0) - (text_size.y / 2.0);

    draw_text(*font, text, (Vector2) { text_x, text_y }, 96 * ease, 1.5f, WHITE);

}

void draw_buttons(Font *font) {

    draw_button(font, "A", 0);
    draw_button(font, "B", 1);
    draw_button(font, "C", 2);

}

void draw_gui(RPCC_STATE *state, Font *font) {

    ClearBackground(BACKGROUND_DARK);

    if (connected_animation != 0) draw_buttons(font);
    draw_status(font);

    animate_connection(state->is_rpcc_connected);

}