#include <inputs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphics.h>
#include <config.h>
#include <winapi.h>

void option_pressed(GUIContext *context, Config *config, ButtonAction action) {

    context->configuration_options[context->current_button] = action;
    
    ButtonConfig *button_config = &config->buttons[context->current_button];
    button_config->action = action;
    
    switch (action) {

        case OPEN_FILE: {

            OpenFileActionData *data = malloc(sizeof(OpenFileActionData));

            data->as_administrator = 0;
            data->path = malloc(1);
            strcpy(data->path, "");

            button_config->action_data = data;

            break;
        }

        case OPEN_LINK: {

            OpenLinkActionData *data = malloc(sizeof(OpenLinkActionData));

            data->link = malloc(1);
            strcpy(data->link, "");

            button_config->action_data = data;

            break; 

        }

        case SCRIPT: {

            ScriptActionData *data = malloc(sizeof(ScriptActionData));

            data->script = malloc(1);
            strcpy(data->script, "");

            button_config->action_data = data;

        }
    }

    save_config(config);
}

void load_button_option(GUIContext *context, Config *config) {

    ButtonConfig *button_config = &config->buttons[context->current_button];
    ButtonAction action = context->configuration_options[context->current_button];

    switch (action) {
    
        case (OPEN_FILE): {

            OpenFileActionData *data = (OpenFileActionData*) button_config->action_data;
            
            if (open_file_input.text != NULL) free(open_file_input.text);

            open_file_input.text = malloc((strlen(data->path) + 1) * sizeof(char));
            strcpy(open_file_input.text, data->path);

            break;
        }

    }

}

void button_selection_pressed(GUIContext *context, Config *config, void *button) {

    context->current_button = *((int*) button);
    context->is_configuring = true;

    load_button_option(context, config);
}

void exit_button_pressed(GUIContext *context, Config *config, void *data) { 
    
    context->connected_animation = 0.0f;
    context->is_configuring = false;

}

void as_administrator_checkbox_pressed(GUIContext *context, Config *config, void* data) {

    as_administrator_checkbox.checked = !as_administrator_checkbox.checked;

}

void press_button(GUIContext *context, Config *config, Rectangle rect, ButtonCallback callback, void *data) {

    if (is_mouse_hovers_rect(context, &rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        callback(context, config, data);
}

void press_select_buttons(GUIContext *context, Config *config) {

    for (int i = 0; i < sizeof(select_buttons) / sizeof(CharButton); i++) {
        CharButton *button = &select_buttons[i];

        press_button(context, config, button->rect, button_selection_pressed, &i);
    }

}

void press_configuration_options(GUIContext *context, Config *config) {

    for (int i = 0; i < BUTTON_ACTION_COUNT; i++) {
        ConfigurationOption *option = &options[i];

        if (is_mouse_hovers_rect(context, &option->rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            option_pressed(context, config, (ButtonAction) i);

    }

}

void press_app_path_input(GUIContext *context, Config *config, void* hwnd) {

    ButtonAction current_action = context->configuration_options[context->current_button];
    if (current_action != OPEN_FILE) return;

    if (is_mouse_hovers_rect(context, &open_file_input.rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        
        static char path[MAX_WINDOWS_PATH_LENGTH];

        if (!open_file_dialog(hwnd, path, MAX_WINDOWS_PATH_LENGTH)) return;

        OpenFileActionData *data = (OpenFileActionData*) config->buttons[context->current_button].action_data;

        if (data->path != NULL) free(data->path);
        data->path = malloc((strlen(path) + 1) * sizeof(char));
        strcpy(data->path, path);

        save_config(config);

        if (open_file_input.text != NULL) free(open_file_input.text);
        open_file_input.text = malloc((strlen(path) + 1) * sizeof(char));
        strcpy(open_file_input.text, path);
    }

}

void process_inputs(GUIContext *context, RPCCState *state, Config *config, void* hwnd) {

    if (context->is_configuring) {

        press_configuration_options(context, config);
        press_button(context, config, exit_button.rect, exit_button_pressed, 0);
        press_app_path_input(context, config, hwnd);
        press_button(context, config, as_administrator_checkbox.rect, as_administrator_checkbox_pressed, 0);

    }

    if (!context->is_configuring && state->is_rpcc_connected) {

        press_select_buttons(context, config);

    }

}