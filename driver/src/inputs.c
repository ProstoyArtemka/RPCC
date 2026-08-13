#include <inputs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <graphics.h>
#include <config.h>

void option_pressed(GUIContext *context, Config *config, ButtonAction action) {

    context->selected_configuration_option[context->selected_button] = action;
    
    ButtonConfig *button_config = &config->buttons[context->selected_button];
    button_config->action = action;
    
    switch (action) {

        case OPEN_APP: {

            OpenAppActionData *data = malloc(sizeof(OpenAppActionData));

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

void button_selection_pressed(GUIContext *context, void *button) {

    context->selected_button = *((int*) button);
    context->is_configuring = true;

}

void exit_button_pressed(GUIContext *context, void *data) { 
    
    context->selected_button = -1; 
    context->connected_animation = 0.0f;
    context->is_configuring = false;

}



void press_button(GUIContext *context, Rectangle rect, ButtonCallback callback, void *data) {

    if (is_mouse_hovers_rect(context, &rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        callback(context, data);
}

void press_select_buttons(GUIContext *context) {

    for (int i = 0; i < sizeof(select_buttons) / sizeof(CharButton); i++) {
        CharButton *button = &select_buttons[i];

        press_button(context, button->rect, button_selection_pressed, &i);
    }

}

void press_configuration_options(GUIContext *context, Config *config) {

    for (int i = 0; i < BUTTON_ACTION_COUNT; i++) {
        ConfigurationOption *option = &options[i];

        if (is_mouse_hovers_rect(context, &option->rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
            option_pressed(context, config, (ButtonAction) i);

    }

}



void process_inputs(GUIContext *context, RPCCState *state, Config *config) {


    
    if (!context->is_configuring && state->is_rpcc_connected) {

        press_select_buttons(context);

    }

    if (context->is_configuring) {

        press_configuration_options(context, config);
        press_button(context, exit_button.rect, exit_button_pressed, 0);

    }

}