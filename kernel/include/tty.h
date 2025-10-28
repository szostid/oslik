/// Low-level utilities for terminal content manipulation
#ifndef TERMINAL_H
#define TERMINAL_H

#include <input.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define BUFFER_SIZE VGA_HEIGHT *VGA_WIDTH
// lowest row is for the scratchpad
#define TTY_HEIGHT (VGA_HEIGHT - 1)
#define SCRATCHPAD_WIDTH 78

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

/// @brief Callback called on a keypress.
typedef void (*keypress_callback_t)(keys_t key, bool was_pressed);

/// @brief A virtual terminal buffer.
///
/// All terminal operations will operate on virtual terminal buffers.
/// The terminal buffer will only flush on a call to `tty_flush`.
typedef struct
{
    uint16_t buffer[BUFFER_SIZE];
    size_t cursor_row;
    size_t cursor_col;
    terminal_entry_color_t color;
    keypress_callback_t on_keypress;
} tty_t;

/// @brief Initializes the terminal for usage
void tty_initialize(tty_t *tty);

/// @brief Clears the terminal, resets the cursor position and color.
void tty_clear(tty_t *tty, terminal_color_t background);

/// @brief Reads the terminal entry at `x, y`
///
/// @param x X position in terminal
/// @param y Y position in terminal
/// @return Entry at provided position
terminal_entry_t tty_read_at(tty_t *tty, size_t x, size_t y);

/// @brief Sets the entry at the provided position
///
/// @param entry Entry to set
/// @param x X position in terminal
/// @param y Y position in terminal
void tty_set_entry_at(tty_t *tty, terminal_entry_t entry, size_t x, size_t y);

/// @brief Sets the character at the provided position. The color will be the
/// tty's default color.
///
/// @param c Character to set
/// @param x X position in terminal
/// @param y Y position in terminal
void tty_set_char_at(tty_t *tty, char c, size_t x, size_t y);

/// @brief Moves the terminal cursor to the next character's position
void tty_next_char(tty_t *tty);

/// @brief Moves the terminal cursor to a new line
void tty_next_line(tty_t *tty);

/// @brief Scrolls the terminal up by one line
void tty_move_up(tty_t *tty);

/// @brief Inserts the provided entry at the cursor position
///
/// @param entry The entry to insert
void tty_put_entry(tty_t *tty, terminal_entry_t entry);

/// @brief Writes the provided `data` with the provided `size` to the terminal
///        at the cursor's position with the `color`
///
/// @param data Data to write to terminal
/// @param size Size of the data to write
/// @param color Color to write
void tty_write(tty_t *tty, const char *data, size_t size);

/// @brief Writes the provided null-terminated `data` to the terminal at
/// cursor's position with the `color`
///
/// @param data Data to write to terminal. Must be null-terminated.
/// @param color Color to write.
void tty_write_string(tty_t *tty, const char *data);

/// @brief Sets the color that the next `terminal_write_*` functions will use
///
/// @param color Color to write
void tty_set_color(tty_t *tty, terminal_entry_color_t color);

/// @brief If the provided tty is active, it will be flushed to the tty buffer.
/// Otherwise, this function has no effect.
void tty_flush(tty_t *tty);

void set_active_tty(tty_t *tty);

tty_t *get_active_tty();

/// @brief The kernel tty.
extern tty_t kernel_tty;

#endif