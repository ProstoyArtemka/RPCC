#include <config.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ConfigReadResult read_button_config(FILE *file, ButtonConfig *button_config) {

    ButtonAction button_action = (ButtonAction) fgetc(file);

    button_config->action = button_action;
    button_config->action_data = NULL;

    switch (button_action) {

        case OPEN_FILE: {
            
            int as_administrator = fgetc(file);
            if (as_administrator == EOF) return CONFIG_READ_ERROR;

            int path_size;
            if (fread(&path_size, sizeof(int), 1, file) != 1) return CONFIG_READ_ERROR;

            char *path = malloc(sizeof(char) * (path_size + 1));
            path[path_size] = '\0';

            if (fread(path, sizeof(char), path_size, file) != path_size) {
                free(path);

                return CONFIG_READ_ERROR;
            }

            OpenFileActionData *data = malloc(sizeof(OpenFileActionData));

            data->as_administrator = as_administrator;
            data->path = path;

            button_config->action_data = (void*) data;

            break;
        }

        case OPEN_LINK: {

            int link_size;
            if (fread(&link_size, sizeof(int), 1, file) != 1) return CONFIG_READ_ERROR;

            char *link = malloc(sizeof(char) * (link_size + 1));
            link[link_size] = '\0';

            if (fread(link, sizeof(char), link_size, file) != link_size) {
                free(link);

                return CONFIG_READ_ERROR;
            }

            OpenLinkActionData *data = malloc(sizeof(OpenLinkActionData));

            data->link = link;

            button_config->action_data = (void*) data;

            break;
        }

        case SCRIPT: {

            int script_size;
            if (fread(&script_size, sizeof(int), 1, file) != 1) return CONFIG_READ_ERROR;

            char *script = malloc(sizeof(char) * (script_size + 1));
            script[script_size] = '\0';

            if (fread(script, sizeof(char), script_size, file) != script_size) {
                free(script);

                return CONFIG_READ_ERROR;
            }

            ScriptActionData *data = malloc(sizeof(ScriptActionData));

            data->script = script;

            button_config->action_data = (void*) data;

            break;
        }
    
    }

    return CONFIG_READ_SUCCESS;
}

ConfigReadResult load_config(Config *config) {

    FILE *file = fopen("./rpcc.config", "a+b");
    if (file == NULL) return CONFIG_READ_ERROR;

    if (fgetc(file) == EOF) {
        fclose(file);
    
        return CONFIG_IS_EMPTY;
    }

    rewind(file);

    for (size_t i = 0; i < sizeof(config->buttons) / sizeof(ButtonConfig); i++) {

        if (read_button_config(file, &config->buttons[i]) == CONFIG_READ_SUCCESS) continue;

        fclose(file);
        return CONFIG_READ_ERROR;
    }

    fclose(file);
    return CONFIG_READ_SUCCESS;
}

void save_button_config(FILE *file, ButtonConfig *button_config) {

    fputc(button_config->action, file);

    switch (button_config->action) {

        case OPEN_FILE: {

            OpenFileActionData *data = (OpenFileActionData*) button_config->action_data;

            fputc(data->as_administrator, file);

            int path_size = strlen(data->path);
            fwrite(&path_size, sizeof(int), 1, file);

            fwrite(data->path, sizeof(char), path_size, file);

            break;
        }

        case OPEN_LINK: {

            OpenLinkActionData *data = (OpenLinkActionData*) button_config->action_data;

            int link_size = strlen(data->link);
            fwrite(&link_size, sizeof(int), 1, file);

            fwrite(data->link, sizeof(char), link_size, file);

            break;
        }

        case SCRIPT: {

            ScriptActionData *data = (ScriptActionData*) button_config->action_data;

            int script_size = strlen(data->script);
            fwrite(&script_size, sizeof(int), 1, file);

            fwrite(data->script, sizeof(char), script_size, file);

            break;
        }
    }

}

ConfigSaveResult save_config(Config *config) {

    FILE *file = fopen("./rpcc.config", "wb");
    if (file == NULL) return CONFIG_SAVE_ERROR;

    for (size_t i = 0; i < sizeof(config->buttons) / sizeof(ButtonConfig); i++) {

        ButtonConfig *button_config = &config->buttons[i];

        save_button_config(file, button_config);
    }

    if (ferror(file)) {
        fclose(file);

        return CONFIG_SAVE_ERROR;
    }

    fclose(file);

    return CONFIG_SAVE_SUCCESS;

}

void unload_config(Config *config) {

    for (size_t i = 0; i < sizeof(config->buttons) / sizeof(ButtonConfig); i++) {

        ButtonConfig *button_config = &config->buttons[i];

        switch (button_config->action) {

            case OPEN_FILE: {

                OpenFileActionData *data = (OpenFileActionData*) button_config->action_data;
                
                if (data->path != NULL)
                    free(data->path);

                free(data);

                break;
            }

            case OPEN_LINK: {

                OpenLinkActionData *data = (OpenLinkActionData*) button_config->action_data;

                if (data->link != NULL)
                    free(data->link);
                
                free(data);

                break;
            }

            case SCRIPT: {

                ScriptActionData *data = (ScriptActionData*) button_config->action_data;

                if (data->script != NULL)
                    free(data->script);
                
                free(data);

                break;

            }
        }
    }
}