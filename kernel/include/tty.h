/// Low-level utilities for terminal content manipulation
#ifndef TERMINAL_H
#define TERMINAL_H

#include <stddef.h>
#include <stdint.h>

typedef enum
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} terminal_color_t;

/// @brief A color of an entry in the terminal's buffer
typedef struct
{
    terminal_color_t foreground;
    terminal_color_t background;
} terminal_entry_color_t;

/// @brief An entry in the terminal's buffer.
typedef struct
{
    uint8_t character;
    terminal_entry_color_t color;
} terminal_entry_t;

/// @brief Initializes the terminal for usage
void terminal_initialize(void);

/// @brief Clears the terminal, resets the cursor position and color.
void terminal_clear(terminal_color_t background);

/// @brief Reads the terminal entry at `x, y`
///
/// @param x X position in terminal
/// @param y Y position in terminal
/// @return Entry at provided position
terminal_entry_t terminal_read_at(size_t x, size_t y);

/// @brief Sets the entry at the provided position
///
/// @param entry Entry to set
/// @param x X position in terminal
/// @param y Y position in terminal
void terminal_set_at(terminal_entry_t entry, size_t x, size_t y);

/// @brief Moves the terminal cursor to the next character's position
void terminal_next_char(void);

/// @brief Moves the terminal cursor to a new line
void terminal_next_line(void);

/// @brief Scrolls the terminal up by one line
void terminal_move_up(void);

/// @brief Inserts the provided entry at the cursor position
///
/// @param entry The entry to insert
void terminal_put_entry(terminal_entry_t entry);

/// @brief Writes the provided `data` with the provided `size` to the terminal
///        at the cursor's position with the `color`
///
/// @param data Data to write to terminal
/// @param size Size of the data to write
/// @param color Color to write
void terminal_write(const char *data, size_t size);

/// @brief Writes the provided null-terminated `data` to the terminal at
/// cursor's position with the `color`
///
/// @param data Data to write to terminal. Must be null-terminated.
/// @param color Color to write.
void terminal_write_string(const char *data);

/// @brief Sets the color that the next `terminal_write_*` functions will use
///
/// @param color Color to write
void terminal_set_color(terminal_entry_color_t color);

#endif