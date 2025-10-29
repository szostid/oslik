#include <random.h>
#include <stdio.h>
#include <string.h>
#include <tty.h>

tty_t tetris_tty;
bool should_stop = false;

#define TETRIS_WIDTH 10
#define TETRIS_HEIGHT 20

typedef struct
{
    int32_t x;
    int32_t y;
} vec2_t;

typedef struct
{
    vec2_t blocks[4];
    vec2_t offsets[4];
} piece_shape_t;

typedef struct
{
    piece_shape_t *shape;
    vec2_t position;
    uint32_t color;
    uint32_t angle;
} piece_t;

typedef struct
{
    int board[TETRIS_WIDTH * TETRIS_HEIGHT];
    piece_t falling_piece;
} board_t;

#define SHAPE_COUNT 6

static piece_shape_t shapes[] = {
    {
        // T shape
        // ███
        // .█.
        .blocks = {{0, 0}, {1, 0}, {-1, 0}, {0, 1}},
        .offsets = {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    },
    {
        // Z shape
        // ██.
        // .██
        .blocks = {{0, 0}, {-1, 0}, {1, 1}, {0, 1}},
        .offsets = {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    },
    {
        // J shape
        //  █
        //  █
        // ██
        .blocks = {{0, -1}, {-1, 1}, {0, 0}, {0, 1}},
        .offsets = {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    },
    {
        // L shape
        // █
        // █
        // ██
        .blocks = {{0, -1}, {1, 1}, {0, 0}, {0, 1}},
        .offsets = {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    },
    {
        // I shape
        // █
        // █
        // █
        // █
        .blocks = {{-1, 0}, {0, 0}, {1, 0}, {2, 0}},
        .offsets = {{0, 0}, {1, 0}, {1, 0}, {0, 0}},
    },
    {
        // O shape
        // ██
        // ██
        .blocks = {{0, 0}, {0, 1}, {1, 0}, {1, 1}},
        .offsets = {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
    },
};

#define COLOR_COUNT 6

typedef void (*piece_pos_callback_t)(vec2_t pos);

static int32_t colors[] = {
    TTY_COLOR_CYAN,  TTY_COLOR_BLUE,    TTY_COLOR_LIGHT_BROWN,
    TTY_COLOR_GREEN, TTY_COLOR_MAGENTA, TTY_COLOR_RED,
};

static board_t board;

inline vec2_t rotate_vec2(vec2_t vec, uint32_t count)
{
    int new_x = vec.x;
    int new_y = vec.y;

    for (size_t i = 0; i < count; i++)
    {
        int old_x = new_x;
        new_x = -new_y;
        new_y = old_x;
    }

    return (vec2_t){new_x, new_y};
}

inline vec2_t add_vec2(vec2_t lhs, vec2_t rhs)
{
    return (vec2_t){lhs.x + rhs.x, lhs.y + rhs.y};
}

inline vec2_t negate_vec2(vec2_t vec)
{
    return (vec2_t){-vec.x, -vec.y};
}

int get_board_spot_unchecked(board_t *board, vec2_t pos)
{
    return board->board[pos.y * TETRIS_WIDTH + pos.x];
}

bool is_board_spot_vacant(board_t *board, vec2_t pos)
{
    if (pos.x < 0 || pos.x >= TETRIS_WIDTH || pos.y >= TETRIS_HEIGHT)
    {
        return false;
    }

    return get_board_spot_unchecked(board, pos) == 0;
}

/// @brief Returns whether the piece is either out of the bounds
/// of the board, or collides with any elements on the board
bool does_piece_colilde(board_t *board, piece_t *piece)
{
    piece_shape_t *shape = piece->shape;
    uint32_t angle = piece->angle % 4;

    for (int i = 0; i < 4; i++)
    {
        vec2_t base_offset = shape->blocks[i];
        vec2_t offset_rotated = rotate_vec2(base_offset, angle);
        vec2_t offset = add_vec2(offset_rotated, shape->offsets[angle]);
        vec2_t block_position = add_vec2(piece->position, offset);

        if (!is_board_spot_vacant(board, block_position))
        {
            return true;
        }
    }

    return false;
}

bool does_falling_piece_collide_after_moving(board_t *board, vec2_t offset)
{
    vec2_t *position = &board->falling_piece.position;

    *position = add_vec2(*position, offset);

    bool does_collide = does_piece_colilde(board, &board->falling_piece);

    *position = add_vec2(*position, negate_vec2(offset));

    return does_collide;
}

bool does_falling_piece_collide_after_rotating(board_t *board)
{
    board->falling_piece.angle++;

    bool does_collide = does_piece_colilde(board, &board->falling_piece);

    board->falling_piece.angle--;

    return does_collide;
}

/// @brief Returns a random piece shape
piece_shape_t *random_shape()
{
    return &shapes[rand() % SHAPE_COUNT];
}

/// @brief Returns a random piece color
int32_t random_color()
{
    return colors[rand() % COLOR_COUNT];
}

/// @brief Spawns a new falling piece on the board
/// @returns Whether the user lost the game
bool spawn_falling_piece(board_t *board)
{
    piece_shape_t *shape = random_shape();
    int32_t color = random_color();

    piece_t piece = {shape, {5, 1}, color, 0};

    board->falling_piece = piece;

    return does_piece_colilde(board, &piece);
}

void solidify_falling_piece(board_t *board)
{
    piece_shape_t *shape = board->falling_piece.shape;
    uint32_t angle = board->falling_piece.angle % 4;

    for (int i = 0; i < 4; i++)
    {
        vec2_t base_offset = shape->blocks[i];
        vec2_t offset_rotated = rotate_vec2(base_offset, angle);
        vec2_t offset = add_vec2(offset_rotated, shape->offsets[angle]);
        vec2_t block_position = add_vec2(board->falling_piece.position, offset);

        board->board[block_position.y * TETRIS_WIDTH + block_position.x] =
            board->falling_piece.color;
    }
}

void draw_board(board_t *board)
{
    // blitting the solidified elements
    for (int y = 0; y < TETRIS_HEIGHT; y++)
    {
        for (int x = 0; x < TETRIS_WIDTH; x++)
        {
            int32_t spot = get_board_spot_unchecked(board, (vec2_t){x, y});

            if (spot == 0)
            {
                terminal_entry_color_t entry = {TTY_COLOR_DARK_GREY,
                                                TTY_COLOR_BLACK};
                tty_set_color(&tetris_tty, entry);
                tty_set_char_at(&tetris_tty, '.', x, y);
            }
            else
            {
                terminal_entry_color_t entry = {TTY_COLOR_WHITE, spot};
                tty_set_color(&tetris_tty, entry);
                tty_set_char_at(&tetris_tty, ' ', x, y);
            }
        }
    }

    // blitting the falling piece
    piece_t *falling_piece = &board->falling_piece;
    piece_shape_t *falling_piece_shape = board->falling_piece.shape;
    uint32_t angle = board->falling_piece.angle % 4;

    for (int i = 0; i < 4; i++)
    {
        vec2_t base_offset = falling_piece_shape->blocks[i];
        vec2_t offset_rotated = rotate_vec2(base_offset, angle);
        vec2_t offset =
            add_vec2(offset_rotated, falling_piece_shape->offsets[angle]);
        vec2_t block_position = add_vec2(falling_piece->position, offset);

        terminal_entry_color_t entry = {TTY_COLOR_WHITE, falling_piece->color};
        tty_set_color(&tetris_tty, entry);
        tty_set_char_at(&tetris_tty, ' ', block_position.x, block_position.y);
    }

    tty_flush(&tetris_tty);
}

void draw_lost_text()
{
    tty_set_color(&tetris_tty,
                  (terminal_entry_color_t){TTY_COLOR_WHITE, TTY_COLOR_RED});
    tty_set_char_at(&tetris_tty, 'L', 3, 10);
    tty_set_char_at(&tetris_tty, 'o', 4, 10);
    tty_set_char_at(&tetris_tty, 's', 5, 10);
    tty_set_char_at(&tetris_tty, 't', 6, 10);
    tty_flush(&tetris_tty);
}

void check_board_for_clearing(board_t *board)
{

    for (int y = 0; y < TETRIS_HEIGHT; y++)
    {
        bool full = true;
        for (int x = 0; x < TETRIS_WIDTH; x++)
        {
            if (is_board_spot_vacant(board, (vec2_t){x, y}))
            {
                full = false;
                break;
            }
        }
        if (!full)
            continue;

        size_t row_bytes = TETRIS_WIDTH * sizeof(int);

        for (int y2 = y; y2 > 0; y2--)
        {
            memmove(&board->board[TETRIS_WIDTH * y2],
                    &board->board[TETRIS_WIDTH * (y2 - 1)], row_bytes);
        }

        memset(&board->board[0], 0, row_bytes);

        y--;
    }
}

void input_handler(keys_t key, bool was_pressed)
{
    // should_stop = true;

    if (!was_pressed)
    {
        return;
    }

    switch (key)
    {
    case KB_A:
    case KB_Arrow_Left:
        if (!does_falling_piece_collide_after_moving(&board, (vec2_t){-1, 0}))
        {
            board.falling_piece.position =
                add_vec2(board.falling_piece.position, (vec2_t){-1, 0});
        }
        break;
    case KB_D:
    case KB_Arrow_Right:
        if (!does_falling_piece_collide_after_moving(&board, (vec2_t){1, 0}))
        {
            board.falling_piece.position =
                add_vec2(board.falling_piece.position, (vec2_t){1, 0});
        }
        break;
    case KB_S:
    case KB_Arrow_Down:
        if (!does_falling_piece_collide_after_moving(&board, (vec2_t){0, 1}))
        {
            board.falling_piece.position =
                add_vec2(board.falling_piece.position, (vec2_t){0, 1});
        }
        break;
    case KB_W:
    case KB_Arrow_Up:
        if (!does_falling_piece_collide_after_rotating(&board))
        {
            board.falling_piece.angle++;
        }
        break;
    case KB_Q:
    case KB_Esc:
        should_stop = true;
        return;
    default:
        return;
    }

    draw_board(&board);
}

#define CYCLES 1000000000

void run_tetris()
{
    tetris_tty.on_keypress = input_handler;
    tetris_tty.cursor_visible = false;
    should_stop = false;
    memset(&board, 0, sizeof(board_t));

    tty_initialize(&tetris_tty);

    set_active_tty(&tetris_tty);

    srand((uint32_t)rdtsc());

    spawn_falling_piece(&board);

    uint64_t last_frame_time = rdtsc();

    while (!should_stop)
    {
        uint64_t time = rdtsc();

        if (time - last_frame_time < CYCLES)
        {
            continue;
        }

        last_frame_time = time;

        if (does_falling_piece_collide_after_moving(&board, (vec2_t){0, 1}))
        { 
            solidify_falling_piece(&board);
            check_board_for_clearing(&board);
            bool did_loose = spawn_falling_piece(&board);

            if (did_loose)
            {
                draw_lost_text();

                while (true)
                {
                    uint64_t time = rdtsc();

                    if (time - last_frame_time > CYCLES * 2)
                    {
                        break;
                    }
                }

                break;
            }
        }
        else
        {
            board.falling_piece.position =
                add_vec2(board.falling_piece.position, (vec2_t){0, 1});
        }

        draw_board(&board);
    }

    set_active_tty(&kernel_tty);
}