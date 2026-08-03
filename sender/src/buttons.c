#include <buttons.h>

volatile uint8_t button_flags = 0;

uint8_t get_buttons_flags(void) { 

    return button_flags;

}

int is_button_pressed(uint8_t button) {

    return (button_flags & (1 << button)) != 0;

}

void update_buttons_flags(void) {

    button_flags = (~(PIND) & ((1 << PD2) | (1 << PD3) | (1 << PD4))) >> 2;

}

void reset_buttons(void) {

    button_flags = 0;

}