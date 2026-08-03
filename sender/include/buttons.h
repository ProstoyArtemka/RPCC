#include <avr/io.h>

#define BUTTON_A 0
#define BUTTON_B 1
#define BUTTON_C 2

uint8_t get_buttons_flags(void);
int is_button_pressed(uint8_t button);

void update_buttons_flags(void);
void reset_buttons(void);