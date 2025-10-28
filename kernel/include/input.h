#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

typedef void (*keypress_callback_t)();

void on_keypress(void);
void set_keypress_callback(keypress_callback_t callback);

#endif