#ifndef INPUTS_H
#define INPUTS_H

#include <graphics.h>

typedef void (*ButtonCallback) (GUIContext *context, Config *config, void *data);

void process_inputs(GUIContext *context, RPCCState *state, Config *config, void* hwnd);

#endif