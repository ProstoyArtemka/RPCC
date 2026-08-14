#ifndef CONFIG_H
#define CONFIG_H

#define CONFIG_PATH "./rpcc.config"

typedef enum {

    CONFIG_READ_SUCCESS,
    CONFIG_IS_EMPTY,
    CONFIG_READ_ERROR

} ConfigReadResult;

typedef enum {

    CONFIG_SAVE_SUCCESS,
    CONFIG_SAVE_ERROR

} ConfigSaveResult;

typedef enum {

    OPEN_FILE,
    OPEN_LINK,
    WRAP_WINDOWS,
    SCRIPT,
    NOTHING,

    BUTTON_ACTION_COUNT

} ButtonAction;

typedef struct {

    char* path;
    int as_administrator;

} OpenFileActionData;

typedef struct {

    char* link;

} OpenLinkActionData;

typedef struct {

    char *script;

} ScriptActionData;

typedef struct {

    ButtonAction action;

    void *action_data;

} ButtonConfig;

typedef struct {

    ButtonConfig buttons[3];

} Config;

ConfigReadResult load_config(Config *config);
ConfigSaveResult save_config(Config *config);

void unload_config(Config *config);

#endif