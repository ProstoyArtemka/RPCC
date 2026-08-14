#include <graphics.h>

#include "reasings.h"

#include <math.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <config.h>
#include <inputs.h>

Icon checkbox_icon = (Icon) {
  
    .rect = (Rectangle) {
        .x = 0.0f,
        .y = 0.0f,
        .height = 75.0f,
        .width = 75.0f
    },
    
    .path = "assets/icons/check.png"

};

CharButton select_buttons[] = {

    (CharButton) {
        .rect = (Rectangle) {
            .x = 417.,
            .y = 415.,
            .width = 250.,
            .height = 250
        },

        .text = "A",

        .hovering_progress = 0
    },

    (CharButton) {
        .rect = (Rectangle) {
            .x = 835.,
            .y = 415.,
            .width = 250.,
            .height = 250
        },


        .text = "B",

        .hovering_progress = 0
    },

    (CharButton) {
        .rect = (Rectangle) {
            .x = 1253.,
            .y = 415.,
            .width = 250.,
            .height = 250
        },
        
        .text = "C",

        .hovering_progress = 0
    }
};

IconButton exit_button = (IconButton) {

    .rect = (Rectangle) {
        .x = 1695.0,
        .y = 75.0,
        .width = 150,
        .height = 150
    },
    
    .icon = (Icon) {
        .rect = (Rectangle) {
            .x = 0,
            .y = 0,
            .width = 100.0f,
            .height = 100.0f
        },

        .path = "assets/icons/close.png"
    },

    .hovering_progress = 0.0f
};

ConfigurationOption options[] = {

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 458.0,
            .y = 75.0,
            .width = 200.0,
            .height = 150.0
        },

        .icon = (Icon) {
            .path = "assets/icons/file.png",
        },

        .hovering_progress = 0.0f,
        .press_progress = 0.0f

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 658.0,
            .y = 75.0,
            .width = 200.0,
            .height = 150.0
        },

        .icon = (Icon) {
            .path = "assets/icons/link.png",
        },

        .hovering_progress = 0.0f,
        .press_progress = 0.0f

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 858.0,
            .y = 75.0,
            .width = 200.0,
            .height = 150.0
        },

        .icon = (Icon) {
            .path = "assets/icons/wrap.png",
        },

        .hovering_progress = 0.0f,
        .press_progress = 0.0f

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 1058.0,
            .y = 75.0,
            .width = 200.0,
            .height = 150.0
        },

        .icon = (Icon) {
            .path = "assets/icons/cmd.png",
        },

        .hovering_progress = 0.0f,
        .press_progress = 0.0f

    },

    (ConfigurationOption) {

        .rect = (Rectangle) {
            .x = 1258.0,
            .y = 75.0,
            .width = 200.0,
            .height = 150.0
        },

        .icon = (Icon) {
            .path = "assets/icons/nothing.png",
        },
 
        .hovering_progress = 1.0f,
        .press_progress = 1.0f

    },

};

TextLineInput open_file_input = (TextLineInput) {

    .rect = (Rectangle) {
        .x = 297.0f,
        .y = 449.0f,

        .width = 1325.0f,
        .height = 100.0f
    },

    .hovering_progress = 0.0f,

};
Icon open_file_icon = (Icon) {

    .rect = (Rectangle) {
        .x = 0,
        .y = 0,
        .width = 75.0f,
        .height = 75.0f
    },

    .path = "assets/icons/add.png"

};
Checkbox as_administrator_checkbox = (Checkbox) {

    .rect = (Rectangle) {
        .x = 400.0f,
        .y = 600.0f,
        .width = 100.0f,
        .height = 100.0f
    },

    .hovering_progress = 0.0f,
    .pressed_progress = 0.0f,
    .checked = false,

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

    if (!state->is_rpcc_connected && !context->is_configuring) rect_width += (70. * invert_ease);

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

void draw_button_square(GUIContext *context, Vector2 position, Vector2 size, float stroke_size, Color background, Color background_hover, Color stroke, Color stroke_hover, float *hovering_progress) {

    Rectangle rect = (Rectangle) { position.x, position.y, size.x, size.y };
    Rectangle stroke_rect = rect;
    expand_rectangle(&stroke_rect, stroke_size);

    bool is_mouse_hovering = is_mouse_hovers_rect(context, &rect);
    tick(is_mouse_hovering, hovering_progress, 10.0f, context->delta);

    Color background_color = transform_color(*hovering_progress, background, background_hover);
    Color stroke_color = transform_color(*hovering_progress, stroke, stroke_hover);

    DrawRectangleRounded(stroke_rect, 0.5f, 16, stroke_color);
    DrawRectangleRounded(rect, 0.5f, 16, background_color);

}

void draw_text_line_input(GUIContext *context, TextLineInput *input, Vector2 position, Vector2 size, float stroke_size, float font_size, float spacing, Color background, Color background_hover, Color stroke, Color stroke_hover, Color text_color, float *hovering_progress) {

    Rectangle input_rect = (Rectangle) {
        .x = input->rect.x + position.x,
        .y = input->rect.y + position.y,
        .width = input->rect.width * size.x,
        .height = input->rect.height * size.y
    };

    Rectangle stroke_rect = input_rect;
    expand_rectangle(&stroke_rect, stroke_size);

    bool is_mouse_hovering = is_mouse_hovers_rect(context, &input_rect);
    tick(is_mouse_hovering, hovering_progress, 10.0f, context->delta);

    Color background_color = transform_color(*hovering_progress, background, background_hover);
    Color stroke_color = transform_color(*hovering_progress, stroke, stroke_hover);

    DrawRectangleRounded(stroke_rect, 0.5f, 16, stroke_color);
    DrawRectangleRounded(input_rect, 0.5f, 16, background_color);


    float current_font_size = font_size * size.y;
    Vector2 text_size = MeasureTextEx(context->font, input->text, current_font_size, spacing);
    Vector2 dots_size = MeasureTextEx(context->font, "...", current_font_size, 1.5f);
    
    float max_width = input->rect.width - 250.0f;
    bool is_larger_than_box = text_size.x >= max_width;


    char *display_text = input->text;
    if (is_larger_than_box) {
        
        while (*display_text != '\0' && text_size.x > max_width) {
            display_text++;

            text_size = MeasureTextEx(context->font, display_text, current_font_size, spacing);
        }

        Vector2 dots_position = (Vector2) {
            input_rect.x + (input_rect.width / 2.0f) - (text_size.x / 2.0f), 
            input_rect.y + (input_rect.height / 2.0f) - (dots_size.y / 2.0f)
        };

        draw_text(context->font, "...", dots_position, current_font_size, spacing, text_color);
    }

    Vector2 text_position = (Vector2) { 
        input_rect.x + (input_rect.width / 2.0f) - (text_size.x / 2.0f) + (is_larger_than_box ? dots_size.x : 0), 
        input_rect.y + (input_rect.height / 2.0f) - (text_size.y / 2.0f)
    };


    draw_text(context->font, display_text, text_position, current_font_size, spacing, text_color);
}

void draw_char_button(GUIContext *context, CharButton *button) {

    float ease = EaseCircInOut(context->connected_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float x = button->rect.x + (button->rect.width / 2.0 * invert_ease);
    float y = button->rect.y + (button->rect.width / 2.0 * invert_ease);
    float size = button->rect.width * ease;

    draw_button_square(context, (Vector2) { x, y }, (Vector2) { size, size }, 5.0, BUTTON_DEFAULT_BACKGROUND, BUTTON_HOVER_BACKGROUND, BUTTON_DEFAULT_STROKE, BUTTON_HOVER_STROKE, &button->hovering_progress);
    
    Vector2 text_size = MeasureTextEx(context->font, button->text, 96 * ease, 1.5);

    float text_x = x + (size / 2.0) - (text_size.x / 2.0);
    float text_y = y + (size / 2.0) - (text_size.y / 2.0);

    draw_text(context->font, button->text, (Vector2) { text_x, text_y }, 96 * ease, 1.5f, WHITE);
}

void draw_icon(GUIContext *context, Icon *icon, Vector2 position, Vector2 size, Color color) {

    Vector2 icon_position = (Vector2) { position.x + (icon->rect.x), position.y + (icon->rect.y) };

    Rectangle source = { 0.0f, 0.0f, (float) icon->texture.width, (float) icon->texture.height};
    Rectangle destination = { icon_position.x, icon_position.y, icon->texture.width * size.x, icon->texture.height * size.y };

    Vector2 origin = { 0.0f, 0.0f };

    DrawTexturePro(icon->texture, source, destination, origin, 0.0f, color);
}

void draw_icon_button(GUIContext *context, IconButton *button) {

    float ease = EaseCircInOut(context->configuration_animation, 0.0f, 1.0f, 1.0f);
    float invert_ease = 1.0 - ease;

    float x = button->rect.x + ((button->rect.width / 2.0) * invert_ease);
    float y = button->rect.y + ((button->rect.height / 2.0) * invert_ease);
    float size = button->rect.width * ease;

    draw_button_square(context, (Vector2) { x, y }, (Vector2) { size, size }, 5.0, BUTTON_DEFAULT_BACKGROUND, BUTTON_HOVER_BACKGROUND, BUTTON_DEFAULT_STROKE, BUTTON_HOVER_STROKE, &button->hovering_progress);

    Vector2 icon_position = (Vector2) { x + (button->icon.texture.width / 4.0f) * ease, y + (button->icon.texture.height / 4.0f) * ease };
    draw_icon(context, &button->icon, icon_position, (Vector2) { ease, ease }, WHITE);
}

void draw_checkbox(GUIContext *context, Checkbox *checkbox, Vector2 size) {

    float ease = EaseCircInOut(checkbox->pressed_progress, 0.0f, 1.0f, 1.0f);
    float invert_ease = (1.0f - ease);

    Color background_color = (checkbox->checked ? CHECKBOX_PRESSED_BACKGROUND : CHECKBOX_DEFAULT_BACKGROUND );
    Color hover_background_color = (checkbox->checked ? CHECKBOX_PRESSED_BACKGROUND : CHECKBOX_HOVER_BACKGROUND );

    tick(checkbox->checked, &checkbox->pressed_progress, 5.0f, context->delta);
    tick(is_mouse_hovers_rect(context, &checkbox->rect), &checkbox->hovering_progress, 5.0f, context->delta);

    Vector2 button_position = (Vector2) { checkbox->rect.x, checkbox->rect.y };
    Vector2 button_size = (Vector2) { checkbox->rect.width * size.x, checkbox->rect.height * size.y };

    draw_button_square(
        context, 
        button_position,
        button_size,
        5.0f, 
        background_color, 
        hover_background_color, 
        CHECKBOX_DEFAULT_STROKE, 
        CHECKBOX_DEFAULT_STROKE,
        &checkbox->hovering_progress
    );

    Color icon_color = transform_color(ease, TRANSPARENT, BLACK);

    Vector2 icon_size = (Vector2) { 1.0f, 1.0f };
    Vector2 icon_position = (Vector2) { 
        checkbox->rect.x + 15.5f, 
        checkbox->rect.y + 12.5f
    };

    draw_icon(context, &checkbox_icon, icon_position, icon_size, icon_color);
}



void draw_select_buttons(GUIContext *context) {

    for (int i = 0; i < sizeof(select_buttons) / sizeof(CharButton); i++) {
        CharButton *button = &select_buttons[i];
        
        draw_char_button(context, button);
    }
}

void draw_exit_button(GUIContext *context) {

    float ease = EaseCircInOut(context->configuration_animation, 0.0f, 1.0f, 1.0f);

    draw_icon_button(context, &exit_button);

}

Color get_option_background_color(GUIContext *context, ConfigurationOption *option) {

    if (option->press_progress > 0.0f)     
        return transform_color(option->press_progress, OPTION_HOVER_BACKGROUND, OPTION_SELECTED_BACKGROUND);

    return transform_color(option->hovering_progress, OPTION_DEFAULT_BACKGROUND, OPTION_HOVER_BACKGROUND);

}

Color get_option_stroke_color(GUIContext *context, ConfigurationOption *option) { 

    if (option->press_progress > 0.0f)
        return transform_color(option->press_progress, OPTION_HOVER_STROKE, OPTION_SELECTED_STROKE);

    return transform_color(option->hovering_progress, OPTION_DEFAULT_STROKE, OPTION_HOVER_STROKE);

}

void draw_configuration_options(GUIContext *context) {

    float ease = EaseCircInOut(fmaxf(0.0f, (context->configuration_animation - 0.5f) * 2.0f), 0.0f, 1.0f, 1.0f);
    if (ease == 0.0f) return;

    float invert_ease = (1.0f - ease);
    float x = 458.0;
    float y = 75.0;

    // Left option

    Color left_option_background_color = get_option_background_color(context, &options[0]);
    Color left_option_stroke_color = get_option_stroke_color(context, &options[0]);

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

    Color right_option_background_color = get_option_background_color(context, &options[4]);
    Color right_option_stroke_color = get_option_stroke_color(context, &options[4]);

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

        ConfigurationOption *option = &options[i];

        bool is_hovered = is_mouse_hovers_rect(context, &option->rect);
        bool is_pressed = context->configuration_options[context->current_button] == i;

        tick(is_hovered, &option->hovering_progress, 5.0f, context->delta);
        tick(is_pressed, &option->press_progress, 5.0f, context->delta);

        if (i > 0 && i < 4) {

            Color option_background_color = get_option_background_color(context, option);
            Color option_stroke_color = get_option_stroke_color(context, option);

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

        Vector2 option_position = (Vector2) { option->rect.x + 50.0, option->rect.y + 25.0 * ease };
        Color option_icon_color = transform_color(option->press_progress, WHITE, BLACK);

        draw_icon(context, &option->icon, option_position, (Vector2) { 1, ease }, option_icon_color);
        
    }

}

void draw_file_open_settings(GUIContext *context) {

    float ease = EaseCircInOut(fmaxf(0.0f, (context->configuration_animation - 0.5f) * 2.0f), 0.0f, 1.0f, 1.0f);
    float invert_ease = (1.0f - ease);

    if (ease == 0) return;

    char title[] = "Select path to your file";
    Vector2 title_size = MeasureTextEx(context->font, title, 54.0f * ease, 1.5f);
    float title_x = (1920.0f / 2.0f) - (title_size.x / 2.0f);
    float title_y = 376.0f;

    draw_text(context->font, title, (Vector2) { title_x, title_y }, 54.0f * ease, 1.5f, WHITE);

    Vector2 position = (Vector2) {0.0f, 0.0f};
    Vector2 size = (Vector2) {1, ease};

    draw_text_line_input(
        context,
        &open_file_input,
        position,
        size,
        5.0f,
        54.0f,
        1.5f,
        BUTTON_DEFAULT_BACKGROUND,
        BUTTON_HOVER_BACKGROUND,
        BUTTON_DEFAULT_STROKE,
        BUTTON_HOVER_STROKE,
        BUTTON_DEFAULT_STROKE,
        &open_file_input.hovering_progress
    );

    Vector2 icon_position = (Vector2) { open_file_input.rect.x + 15.0f, open_file_input.rect.y + 15.0f * ease };
    draw_icon(context, &open_file_icon, icon_position, (Vector2) { 1, ease }, WHITE);

    Vector2 checkbox_size = (Vector2) { 1.0f, ease };
    draw_checkbox(context, &as_administrator_checkbox, checkbox_size);

    char as_administrator_text[64];
    smooth_text(
        invert_ease,
        "",
        "Run as administrator",
        "",
        as_administrator_text,
        sizeof(as_administrator_text)
    );

    Vector2 text_position = (Vector2) { as_administrator_checkbox.rect.x + as_administrator_checkbox.rect.width + 27.0f, as_administrator_checkbox.rect.y + title_size.y / 2.0f };
    draw_text(context->font, as_administrator_text, text_position, 54.0f, 1.5f, WHITE);
}

void draw_configuration_setttings(GUIContext *context) {

    ButtonAction action = context->configuration_options[context->current_button];

    switch (action) {
        
        case OPEN_FILE: {

            draw_file_open_settings(context);

            break;
        }

    }

}

void draw_gui(GUIContext *context, RPCCState *state) {

    ClearBackground(BACKGROUND_DARK);

    if (context->is_configuring) {

        draw_minimized_status(context, state);

        draw_exit_button(context);

        draw_configuration_options(context);
        draw_configuration_setttings(context);
    }

    if (!context->is_configuring && context->configuration_animation > 0.0f) {

        draw_minimized_status(context, state);

        draw_exit_button(context);

        draw_configuration_options(context);
        draw_configuration_setttings(context);
    }

    if (!context->is_configuring && context->configuration_animation == 0.0f) {

        draw_default_status(context);

    }

    if (context->connected_animation != 0) draw_select_buttons(context);

    tick(state->is_rpcc_connected && !context->is_configuring, &context->connected_animation, 1.0f, context->delta);
    tick(context->is_configuring, &context->configuration_animation, 1.0f, context->delta);
    tick(state->is_rpcc_connected, &context->status_color_animation, 1.0f, context->delta);

}

void load_texture(Texture2D *texture, char* path) {

    *texture = LoadTexture(path);
    SetTextureFilter(*texture, TEXTURE_FILTER_BILINEAR);

}

void load_gui(GUIContext *context) {

    context->screen_texture = LoadRenderTexture(VIRTUAL_WIDTH, VIRTUAL_HEIGHT);
    SetTextureFilter(context->screen_texture.texture, TEXTURE_FILTER_BILINEAR);

    int codepoints[1300];
    for (int i = 0; i < 1300; i++) codepoints[i] = i;

    context->font = LoadFontEx("assets/font.ttf", 96, codepoints, 1300);
    SetTextureFilter(context->font.texture, TEXTURE_FILTER_BILINEAR);

    load_texture(&exit_button.icon.texture, exit_button.icon.path);

    size_t options_size = sizeof(options) / sizeof(ConfigurationOption);
    for (size_t i = 0; i < options_size; i++) {
        ConfigurationOption *option = &options[i];

        load_texture(&option->icon.texture, option->icon.path);
    }

    open_file_input.text = malloc(sizeof(char) * 256);
    strcpy(open_file_input.text, "Path to app");

    load_texture(&open_file_icon.texture, open_file_icon.path);
    load_texture(&checkbox_icon.texture, checkbox_icon.path);
}

void unload_gui(GUIContext *context) {

    UnloadRenderTexture(context->screen_texture);
    UnloadFont(context->font);

    UnloadTexture(exit_button.icon.texture);

    size_t options_size = sizeof(options) / sizeof(ConfigurationOption);
    for (size_t i = 0; i < options_size; i++)
        UnloadTexture(options[i].icon.texture);

    free(open_file_input.text);

    UnloadTexture(open_file_icon.texture);
    UnloadTexture(checkbox_icon.texture);
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