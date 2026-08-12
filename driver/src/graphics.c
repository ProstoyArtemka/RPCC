#include <graphics.h>

#include "reasings.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

bool is_configuring(GUIContext *context) { return context->selected_button != -1; }

void set_configuration_title(GUIContext *context, char button) {
    
    snprintf(
        context->configuration_title,
        sizeof(context->configuration_title),
        "Configure button %c functionality",
        button
    );
}

void button_a_pressed(GUIContext *context) { context->selected_button = 0; set_configuration_title(context, 'A'); }
void button_b_pressed(GUIContext *context) { context->selected_button = 1; set_configuration_title(context, 'B'); }
void button_c_pressed(GUIContext *context) { context->selected_button = 2; set_configuration_title(context, 'C'); }

void exit_button_pressed(GUIContext *context) { context->selected_button = -1; context->connected_animation = 0.0f; }

void option_app_pressed(GUIContext *context) { context->selected_configuration_option[context->selected_button] = 0; }
void option_link_pressed(GUIContext *context) { context->selected_configuration_option[context->selected_button] = 1; }
void option_wrap_pressed(GUIContext *context) { context->selected_configuration_option[context->selected_button] = 2; }
void option_cmd_pressed(GUIContext *context) { context->selected_configuration_option[context->selected_button] = 3; }
void option_nothing_pressed(GUIContext *context) { context->selected_configuration_option[context->selected_button] = 4; }

CharButton select_buttons[] = {

    (CharButton) {
        .rect = (Rectangle) {
            .x = 417.,
            .y = 415.,
            .width = 250.,
            .height = 250
        },

        .text = "A",

        .hovering_progress = 0,

        .callback = button_a_pressed
    },

    (CharButton) {
        .rect = (Rectangle) {
            .x = 835.,
            .y = 415.,
            .width = 250.,
            .height = 250
        },


        .text = "B",

        .hovering_progress = 0,

        .callback = button_b_pressed
    },

    (CharButton) {
        .rect = (Rectangle) {
            .x = 1253.,
            .y = 415.,
            .width = 250.,
            .height = 250
        },
        
        .text = "C",

        .hovering_progress = 0,

        .callback = button_c_pressed
    }
};

IconButton exit_button = (IconButton) {

    .rect = (Rectangle) {
        .x = 1695.0,
        .y = 75.0,
        .width = 150,
        .height = 150
    },
    
    .path = "assets/icons/close.png",
    .texture = {0},

    .hovering_progress = 0.,

    .callback = exit_button_pressed
};

ConfigurationOption options[] = {

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 458.0,
            .y = 272.0,
            .width = 200.0,
            .height = 150.0
        },

        .path = "assets/icons/app.png",
        .texture = (Texture2D) {0},

        .hovering_progress = 0.0f,
        .press_progress = 0.0f,
        
        .callback = option_app_pressed

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 658.0,
            .y = 272.0,
            .width = 200.0,
            .height = 150.0
        },

        .path = "assets/icons/link.png",
        .texture = (Texture2D) {0},

        .hovering_progress = 0.0f,
        .press_progress = 0.0f,

        .callback = option_link_pressed

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 858.0,
            .y = 272.0,
            .width = 200.0,
            .height = 150.0
        },

        .path = "assets/icons/wrap.png",
        .texture = (Texture2D) {0},

        .hovering_progress = 0.0f,
        .press_progress = 0.0f,

        .callback = option_wrap_pressed

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 1058.0,
            .y = 272.0,
            .width = 200.0,
            .height = 150.0
        },

        .path = "assets/icons/cmd.png",
        .texture = (Texture2D) {0},

        .hovering_progress = 0.0f,
        .press_progress = 0.0f,

        .callback = option_cmd_pressed

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 1258.0,
            .y = 272.0,
            .width = 200.0,
            .height = 150.0
        },

        .path = "assets/icons/nothing.png",
        .texture = (Texture2D) {0},

        .hovering_progress = 0.0f,
        .press_progress = 1.0f,

        .callback = option_nothing_pressed

    },

};

char* get_status_text(RPCCState *state) {
    return state->is_rpcc_connected
        ? "RPCC connected"
        : "RPCC disconnected";
}

int clamp_int(int x, int low, int high) {
    if (x < low) return low;
    if (x > high) return high;

    return x;
}

void update_mouse_position(GUIContext *context) {
    Vector2 mouse = GetMousePosition();
    
    Vector2 virtual_mouse = {
        (mouse.x / (float) GetScreenWidth()) * (float) context->screen_texture.texture.width,
        (mouse.y / (float) GetScreenHeight()) * (float) context->screen_texture.texture.height
    };
    
    virtual_mouse.x = fmaxf(0.0f, fminf(virtual_mouse.x, (float) context->screen_texture.texture.width));
    virtual_mouse.y = fmaxf(0.0f, fminf(virtual_mouse.y, (float) context->screen_texture.texture.height));

    context->mouse_position = virtual_mouse;
}

bool is_mouse_hovers_rect(GUIContext *context, Rectangle *rectangle) {
    return CheckCollisionPointRec(context->mouse_position, *rectangle);
}

Color transform_color(float t, Color from, Color to) {

    return (Color) {
        from.r + (to.r - from.r) * t, 
        from.g + (to.g - from.g) * t,
        from.b + (to.b - from.b) * t,
        from.a + (to.a - from.a) * t
    };

}

void tick(bool add, float *value, float speed, float delta_time) {

    float delta = delta_time * speed;

    if (add) {
        *value = fminf(*value + delta, 1.0f);
    
        return;
    }

    *value = fmaxf(*value - delta, 0.0f);

}

void smooth_text(float ease, char* prefix, char* smooth, char* suffix, char* output, size_t output_size) {

    int smooth_len = strlen(smooth);
    int visible = (int) ((1.0 - ease) * smooth_len + 0.5);
    visible = clamp_int(visible, 0, smooth_len);

    snprintf(output, output_size, "%s%.*s%s", prefix, visible, smooth, suffix);

}

void expand_rectangle(Rectangle *rect, float amount) {

    rect->x -= amount;
    rect->y -= amount;
    rect->height += amount * 2.0f;
    rect->width += amount * 2.0f;

}


void draw_text(Font font, char *text, Vector2 position, float size, float spacing, Color color) {
    DrawTextEx(font, text, position, size, spacing, color);
}

void draw_default_status(GUIContext *context) {

    float ease = EaseCircInOut(context->connected_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float rect_x = 654.0 + (34.0 * ease);
    float rect_y = 75.0;
    float rect_width = 545.0 + (70. * invert_ease);
    float rect_height = 150.0;
    
    Rectangle status_bg_rectangle = (Rectangle) { rect_x, rect_y, rect_width, rect_height }; 

    Rectangle stroke_bg_rectangle = status_bg_rectangle;
    expand_rectangle(&stroke_bg_rectangle, 5.0f);

    Color status_bg_color = (Color) { 11 + (1.0f - context->status_color_animation), 11 + (22 * context->status_color_animation), 11, 255 };
    Color status_color = (Color) { 255 * (1.0f - context->status_color_animation), 255 * context->status_color_animation, 0, 255 };

    DrawRectangleRounded(stroke_bg_rectangle, 0.5f, 16, status_color);
    DrawRectangleRounded(status_bg_rectangle, 0.5f, 16, status_bg_color);

    float circle_x = rect_x + 75;
    float circle_y = rect_y + (rect_height / 4.0) + 37.5;

    DrawCircle(circle_x, circle_y, 37.5, status_color);

    float text_x = rect_x + 143.;
    float text_y = rect_y + (rect_height / 2.) - 27.;

    char animated_status_text[32];

    smooth_text(
        ease, 
        "RPCC ", 
        "dis", 
        "connected",
        animated_status_text,
        sizeof(animated_status_text)
    );

    draw_text(context->font, animated_status_text, (Vector2) { text_x, text_y }, 54, 1.5f, WHITE);
}

void draw_minimized_status(GUIContext *context, RPCCState *state) {
    
    float ease = EaseCircInOut(context->configuration_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float connected_ease = EaseCircInOut(context->connected_animation, 0.0f, 1.0f, 1.0f);

    float rect_x = 654.0 - (579.0 * ease) + (34.0 * connected_ease);
    float rect_y = 75.0;
    float rect_width = 150 + (395. * invert_ease);
    float rect_height = 150.0;

    if (!state->is_rpcc_connected && !is_configuring(context)) rect_width += (70. * invert_ease);

    Rectangle status_bg_rectangle = (Rectangle) { rect_x, rect_y, rect_width, rect_height }; 
    Rectangle stroke_bg_rectangle = (Rectangle) { rect_x - 5, rect_y - 5, rect_width + 10, rect_height + 10 };

    Color status_bg_color = (Color) { 11 + (1.0f - context->status_color_animation), 11 + (22 * context->status_color_animation), 11, 255 };
    Color status_color = (Color) { 255 * (1.0f - context->status_color_animation), 255 * context->status_color_animation, 0, 255 };

    DrawRectangleRounded(stroke_bg_rectangle, 0.5f, 16, status_color);
    DrawRectangleRounded(status_bg_rectangle, 0.5f, 16, status_bg_color);

    float circle_x = rect_x + 75;
    float circle_y = rect_y + (rect_height / 4.0) + 37.5;

    DrawCircle(circle_x, circle_y, 37.5, status_color);

    float text_x = rect_x + 143.;
    float text_y = rect_y + (rect_height / 2.) - 27.;

    char smoothed_text[32];
    smooth_text(ease, "", get_status_text(state), "", smoothed_text, sizeof(smoothed_text));

    draw_text(context->font, smoothed_text, (Vector2) { text_x, text_y }, 54, 1.5f, WHITE);
}

void draw_button_square(GUIContext *context, float x, float y, float size, float stroke_size, Color background, Color background_hover, Color stroke, Color stroke_hover, float *hovering_progress) {

    Rectangle rect = (Rectangle) { x, y, size, size };
    Rectangle stroke_rect = rect;
    expand_rectangle(&stroke_rect, stroke_size);

    bool is_mouse_hovering = is_mouse_hovers_rect(context, &rect);
    tick(is_mouse_hovering, hovering_progress, 10.0f, context->delta);

    Color background_color = transform_color(*hovering_progress, background, background_hover);
    Color stroke_color = transform_color(*hovering_progress, stroke, stroke_hover);

    DrawRectangleRounded(stroke_rect, 0.5f, 16, stroke_color);
    DrawRectangleRounded(rect, 0.5f, 16, background_color);

}

void draw_char_button(GUIContext *context, CharButton *button) {

    float ease = EaseCircInOut(context->connected_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float x = button->rect.x + (button->rect.width / 2.0 * invert_ease);
    float y = button->rect.y + (button->rect.width / 2.0 * invert_ease);
    float size = button->rect.width * ease;

    draw_button_square(context, x, y, size, 5.0, BUTTON_DEFAULT_BACKGROUND, BUTTON_HOVER_BACKGROUND, BUTTON_DEFAULT_STROKE, BUTTON_HOVER_STROKE, &button->hovering_progress);
    
    Vector2 text_size = MeasureTextEx(context->font, button->text, 96 * ease, 1.5);

    float text_x = x + (size / 2.0) - (text_size.x / 2.0);
    float text_y = y + (size / 2.0) - (text_size.y / 2.0);

    draw_text(context->font, button->text, (Vector2) { text_x, text_y }, 96 * ease, 1.5f, WHITE);
}

void draw_icon_button(GUIContext *context, IconButton *button) {

    float ease = EaseCircInOut(context->configuration_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float x = button->rect.x + ((button->rect.width / 2.0) * invert_ease);
    float y = button->rect.y + ((button->rect.width / 2.0) * invert_ease);
    float size = button->rect.width * ease;

    draw_button_square(context, x, y, size, 5.0, BUTTON_DEFAULT_BACKGROUND, BUTTON_HOVER_BACKGROUND, BUTTON_DEFAULT_STROKE, BUTTON_HOVER_STROKE, &button->hovering_progress);

    Rectangle source = (Rectangle){ 0.0f, 0.0f, (float)button->texture.width, (float)button->texture.height };

    float scaledWidth = (float) button->texture.width * ease;
    float scaledHeight = (float) button->texture.height * ease;

    float texture_x = button->rect.x + (button->rect.width - scaledWidth) / 2.0f;
    float texture_y = button->rect.y + (button->rect.height - scaledHeight) / 2.0f;

    Rectangle destination = (Rectangle){ texture_x, texture_y, scaledWidth, scaledHeight };
    Vector2 origin = (Vector2){ 0.0f, 0.0f };

    DrawTexturePro(button->texture, source, destination, origin, 0.0f, WHITE);
}

void press_button(GUIContext *context, Rectangle rect, void (*callback) (GUIContext*)) {

    if (is_mouse_hovers_rect(context, &rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        callback(context);

}

void draw_select_buttons(GUIContext *context) {

    for (int i = 0; i < sizeof(select_buttons) / sizeof(CharButton); i++) {
        CharButton *button = &select_buttons[i];
        
        draw_char_button(context, button);
        press_button(context, button->rect, button->callback);
    }
}

void draw_configuration_title(GUIContext *context) {

    float ease = EaseCircInOut((context->configuration_animation - 0.5f) * 2.0f, 0.0f, 1.0f, 1.0f);

    float font_size = 54 * ease;
    Vector2 text_size = MeasureTextEx(context->font, context->configuration_title, font_size, 1.5f);
    float text_x = (1920.0 / 2) - (text_size.x / 2);
    float text_y = 128;

    draw_text(context->font, context->configuration_title, (Vector2) {text_x, text_y}, font_size, 1.5f, WHITE);
}

void draw_exit_button(GUIContext *context) {

    float ease = EaseCircInOut(context->configuration_animation, 0.0f, 1.0f, 1.0f);

    draw_icon_button(context, &exit_button);

}

Color get_option_background_color(GUIContext *context, int index, float t) {

    return transform_color(
        t,
        OPTION_DEFAULT_BACKGROUND,
        OPTION_SELECTED_BACKGROUND
    );

}

Color get_option_stroke_color(GUIContext *context, int index, float t) { 

    return transform_color(
        t, 
        OPTION_DEFAULT_STROKE,
        OPTION_SELECTED_STROKE
    );

}

void draw_configuration_options(GUIContext *context) {

    float ease = EaseCircInOut(context->configuration_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = (1.0f - ease);

    float x = 458.0;
    float y = 272.0;

    // Left option

    Color left_option_background_color = get_option_background_color(context, 0, options[0].press_progress);
    Color left_option_stroke_color = get_option_stroke_color(context, 0, options[0].press_progress);

    Rectangle left_option_rect = (Rectangle) { x, y, 200.0, 150.0 * ease };
    Rectangle left_option_stroke_rect = left_option_rect;
    expand_rectangle(&left_option_stroke_rect, 5.0f);
    
    Rectangle left_option_square_rect = (Rectangle) {x + 150.0, y, 110.0, 150.0 * ease};
    Rectangle left_option_square_stroke_rect = left_option_square_rect;
    expand_rectangle(&left_option_square_stroke_rect, 5.0f);
    left_option_square_stroke_rect.x += 20;
    left_option_square_stroke_rect.width = 90.0f;

    DrawRectangleRounded(left_option_stroke_rect, 0.5f, 16, left_option_stroke_color);
    DrawRectangleRounded(left_option_rect, 0.5f, 16, left_option_background_color);

    DrawRectangleRec(left_option_square_stroke_rect, left_option_stroke_color);
    DrawRectangleRec(left_option_square_rect, left_option_background_color);

    // Right option

    Color right_option_background_color = get_option_background_color(context, 4, options[4].press_progress);
    Color right_option_stroke_color = get_option_stroke_color(context, 4, options[4].press_progress);

    Rectangle right_option_rect = (Rectangle) { x + (800.0), y, 200.0, 150.0 * ease };
    Rectangle right_option_stroke_rect = right_option_rect;
    expand_rectangle(&right_option_stroke_rect, 5.0f);
    
    Rectangle right_option_square_rect = (Rectangle) {x + (790.0), y, 110.0, 150.0 * ease};
    Rectangle right_option_square_stroke_rect = right_option_square_rect;
    expand_rectangle(&right_option_square_stroke_rect, 5.0f);
    right_option_square_stroke_rect.x += 20;
    right_option_square_stroke_rect.width = 20.0f;

    DrawRectangleRounded(right_option_stroke_rect, 0.5f, 16, right_option_stroke_color);
    DrawRectangleRounded(right_option_rect, 0.5f, 16, right_option_background_color);

    DrawRectangleRec(right_option_square_stroke_rect, right_option_stroke_color);
    DrawRectangleRec(right_option_square_rect, right_option_background_color);



    for (int i = 0; i < 5; i++) {

        if (i > 0 && i < 4) {

            Color option_background_color = get_option_background_color(context, i, options[i].press_progress);
            Color option_stroke_color = get_option_stroke_color(context, i, options[i].press_progress);

            float option_x = x + (i * 200);

            Rectangle background_center_rect = (Rectangle) { option_x, y, 200.0, 150.0 * ease };
            Rectangle stroke_center_rect = background_center_rect;
            expand_rectangle(&stroke_center_rect, 5.0f);

            DrawRectangleRec(stroke_center_rect, option_stroke_color);
            DrawRectangleRec(background_center_rect, option_background_color);

            Vector2 start = (Vector2) { x + (200.0 * i), y };
            Vector2 end = (Vector2) { x + (200.0 * i), y + (150.0 * ease) };
        
            DrawLineEx(start, end, 5, option_stroke_color);

        }

        ConfigurationOption *option = &options[i];
        Vector2 option_position = (Vector2) { option->rect.x + 50.0 + (50.0 * invert_ease), option->rect.y + 25.0 + (-25.0 * invert_ease) };

        tick(context->selected_configuration_option[context->selected_button] == i, &option->press_progress, 5.0f, context->delta);
        Color option_icon_color = transform_color(option->press_progress, WHITE, BLACK);

        DrawTextureEx(option->texture, option_position, 0.0f, ease, option_icon_color);
        
    }

}

void press_configuration_options(GUIContext *context) {

    for (int i = 0; i < 5; i++) {
        ConfigurationOption *option = &options[i];

        if (is_mouse_hovers_rect(context, &option->rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            option->callback(context);

    }

}

void draw_gui(GUIContext *context, RPCCState *state) {

    ClearBackground(BACKGROUND_DARK);

    if (is_configuring(context)) {

        draw_configuration_title(context);
        draw_minimized_status(context, state);

        draw_exit_button(context);
        press_button(context, exit_button.rect, exit_button.callback);

        draw_configuration_options(context);
        press_configuration_options(context);
    }

    if (!is_configuring(context) && context->configuration_animation > 0.0f) {

        draw_minimized_status(context, state);
        draw_configuration_title(context);

        draw_exit_button(context);

        draw_configuration_options(context);
    }

    if (!is_configuring(context) && context->configuration_animation == 0.0f) {

        draw_default_status(context);

    }

    if (context->connected_animation != 0) draw_select_buttons(context);

    tick(state->is_rpcc_connected && !is_configuring(context), &context->connected_animation, 1.0f, context->delta);
    tick(context->selected_button != -1, &context->configuration_animation, 1.0f, context->delta);
    tick(state->is_rpcc_connected, &context->status_color_animation, 1.0f, context->delta);

}



void load_gui(GUIContext *context) {

    context->screen_texture = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(context->screen_texture.texture, TEXTURE_FILTER_BILINEAR);

    context->font = LoadFontEx("assets/font.ttf", 96, NULL, 0);
    SetTextureFilter(context->font.texture, TEXTURE_FILTER_BILINEAR);

    exit_button.texture = LoadTexture(exit_button.path);
    SetTextureFilter(exit_button.texture, TEXTURE_FILTER_BILINEAR);

    size_t options_size = sizeof(options) / sizeof(ConfigurationOption);
    for (size_t i = 0; i < options_size; i++) {
        ConfigurationOption *option = &options[i];

        option->texture = LoadTexture(option->path);
        SetTextureFilter(option->texture, TEXTURE_FILTER_BILINEAR);
    }

}

void unload_gui(GUIContext *context) {

    UnloadRenderTexture(context->screen_texture);
    UnloadFont(context->font);

    UnloadTexture(exit_button.texture);

    size_t options_size = sizeof(options) / sizeof(ConfigurationOption);
    for (size_t i = 0; i < options_size; i++)
        UnloadTexture(options[i].texture);
}

void render_screen(GUIContext *context, RPCCState *state) {

    update_mouse_position(context);
    context->delta = GetFrameTime();

    BeginTextureMode(context->screen_texture);

    draw_gui(context, state);

    EndTextureMode();

    ClearBackground(BACKGROUND_DARK);

    BeginDrawing();

    Rectangle sourceRec = { 0.0f, 0.0f, (float) VIRTUAL_WIDTH, -(float)VIRTUAL_HEIGHT };

    Rectangle destRec = {
        0.0f,
        0.0f,
        (float) GetScreenWidth(),
        (float) GetScreenHeight()
    };

    DrawTexturePro(
        context->screen_texture.texture,
        sourceRec,
        destRec,
        (Vector2) {0, 0},
        0.0f, 
        WHITE
    );

    EndDrawing();
}