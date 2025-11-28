#include <random.h>
#include <stdio.h>
#include <string.h>
#include <tty.h>

#define PADDLE_HEIGHT 5

#define BALL_SPEED 1.0

#define FRAME_START_X 0
#define FRAME_START_Y 0

#define FRAME_SIZE_X VGA_WIDTH
#define FRAME_SIZE_Y VGA_HEIGHT

#define FRAME_END_X (FRAME_START_X + FRAME_SIZE_X - 1)
#define FRAME_END_Y (FRAME_START_Y + FRAME_SIZE_Y - 1)

#define WALL_TOP ((float)FRAME_START_Y + 1.0f)
#define WALL_BOTTOM ((float)FRAME_END_Y - 1.0f)
#define LEFT_BOUND ((float)FRAME_START_X + 2.0f)
#define RIGHT_BOUND ((float)FRAME_END_X - 2.0f)

#define FRAME_CENTER fvec2((float)FRAME_SIZE_X / 2.0, (float)FRAME_SIZE_Y / 2.0)

#define CYCLES 100000000

typedef struct
{
    int32_t x;
    int32_t y;
} ivec2_t;

static inline ivec2_t ivec2(int x, int y)
{
    ivec2_t v = {x, y};
    return v;
}

typedef struct
{
    float x;
    float y;
} fvec2_t;

static inline fvec2_t fvec2(float x, float y)
{
    fvec2_t v = {x, y};
    return v;
}

static inline ivec2_t quantize_from_fvec2(fvec2_t vec)
{
    return ivec2((int)vec.x, (int)vec.y);
}

typedef struct
{
    int verticalPosition;
    int velocity;
    bool isLeft;
} paddle_t;

typedef struct
{
    fvec2_t position;
    fvec2_t velocity;

    bool isPaused;
} ball_t;

typedef struct
{
    paddle_t left_paddle;
    paddle_t right_paddle;
    ball_t ball;
    uint64_t time;
} pong_game_t;

tty_t pong_tty;
pong_game_t game;
bool should_stop_pong = false;

static inline bool inside(float y, float top, float bottom)
{
    return y >= top && y <= bottom;
}

void reset_ball(pong_game_t *game)
{
    ball_t *ball = &game->ball;

    bool goesRight = rand() % 2 == 0;

    ball->velocity = fvec2(goesRight ? BALL_SPEED : -BALL_SPEED, 0.0);
    ball->position = FRAME_CENTER;
    ball->isPaused = true;
}

void update_game(pong_game_t *game, float dt)
{
    ball_t *ball = &game->ball;
    paddle_t *left_paddle = &game->left_paddle;
    paddle_t *right_paddle = &game->right_paddle;

    if (ball->isPaused)
    {
        return;
    }

    ball->position = fvec2(ball->position.x + ball->velocity.x * dt,
                           ball->position.y + ball->velocity.y * dt);

    if (ball->position.y <= WALL_TOP)
    {
        ball->position.y = WALL_TOP;
        ball->velocity.y = 1.0;
    }

    if (ball->position.y >= WALL_BOTTOM)
    {
        ball->position.y = WALL_BOTTOM;
        ball->velocity.y = -1.0;
    }

    if (ball->position.x <= LEFT_BOUND)
    {
        if (inside(ball->position.y, left_paddle->verticalPosition,
                   left_paddle->verticalPosition + PADDLE_HEIGHT))
        {
            ball->position.x = LEFT_BOUND;
            ball->velocity.x = BALL_SPEED;
            ball->velocity.y = (float)left_paddle->velocity;
        }
        else
        {
            reset_ball(game);
        }
    }

    if (ball->position.x >= RIGHT_BOUND)
    {
        if (inside(ball->position.y, right_paddle->verticalPosition,
                   right_paddle->verticalPosition + PADDLE_HEIGHT))
        {
            ball->position.x = RIGHT_BOUND;
            ball->velocity.x = -BALL_SPEED;
            ball->velocity.y = (float)right_paddle->velocity;
        }
        else
        {
            reset_ball(game);
        }
    }

    left_paddle->verticalPosition += left_paddle->velocity;
    right_paddle->verticalPosition += right_paddle->velocity;
}

void pong_input_handler(keys_t key, bool was_pressed, void *data)
{
    if (!was_pressed)
    {
        switch (key)
        {
        case KB_S:
        case KB_W:
            game.left_paddle.velocity = 0;
            break;
        case KB_Arrow_Down:
        case KB_Arrow_Up:
            game.right_paddle.velocity = 0;
            break;

        default:
            break;
        }

        return;
    }

    switch (key)
    {
    case KB_Space:
        game.ball.isPaused = false;
        return;
    case KB_S:
        game.left_paddle.velocity = 1;
        return;
    case KB_W:
        game.left_paddle.velocity = -1;
        return;
    case KB_Arrow_Down:
        game.right_paddle.velocity = 1;
        return;
    case KB_Arrow_Up:
        game.right_paddle.velocity = -1;
        return;
    case KB_Q:
    case KB_Esc:
        should_stop_pong = true;
        return;
    default:
        return;
    }
}

void setup_game(pong_game_t *game)
{
    int default_paddle_pos = (FRAME_SIZE_Y - PADDLE_HEIGHT) / 2;

    paddle_t left_paddle = {
        .velocity = 0,
        .isLeft = true,
        .verticalPosition = default_paddle_pos,
    };

    game->left_paddle = left_paddle;

    paddle_t right_paddle = {
        .velocity = 0,
        .isLeft = false,
        .verticalPosition = default_paddle_pos,
    };

    game->right_paddle = right_paddle;

    game->time = rdtsc();

    reset_ball(game);
}

void draw_frame()
{
    terminal_entry_color_t color = {
        .background = TTY_COLOR_BLACK,
        .foreground = TTY_COLOR_DARK_GREY,
    };

    tty_set_color(&pong_tty, color);

    // bottom and top frame
    for (int x = 0; x < FRAME_SIZE_X; x++)
    {
        tty_set_char_at(&pong_tty, '#', x, 0);
        tty_set_char_at(&pong_tty, '#', x, FRAME_END_Y);
    }

    // left and right frame
    for (int y = 0; y < FRAME_SIZE_Y; y++)
    {
        tty_set_char_at(&pong_tty, '#', 0, y);
        tty_set_char_at(&pong_tty, '#', FRAME_END_X, y);
    }
}

void draw_ball(ball_t *ball)
{
    terminal_entry_t entry = {
        .character = 'o',
        .color =
            {
                .background = TTY_COLOR_BLACK,
                .foreground = TTY_COLOR_WHITE,
            },
    };

    ivec2_t pos = quantize_from_fvec2(ball->position);
    tty_set_entry_at(&pong_tty, entry, pos.x, pos.y);
}

void draw_paddle(paddle_t *paddle)
{
    terminal_entry_color_t color = {
        .background = TTY_COLOR_BLACK,
        .foreground = paddle->isLeft ? TTY_COLOR_BLUE : TTY_COLOR_RED,
    };

    tty_set_color(&pong_tty, color);

    int horizontal_pos = paddle->isLeft ? FRAME_START_X + 2 : FRAME_END_X - 2;

    for (int y = 0; y < PADDLE_HEIGHT; y++)
    {
        tty_set_char_at(&pong_tty, '@', horizontal_pos,
                        y + paddle->verticalPosition);
    }
}

void run_pong()
{
    tty_set_keypress_callback(&pong_tty, pong_input_handler, NULL);
    pong_tty.cursor_visible = false;

    tty_initialize(&pong_tty);
    set_active_tty(&pong_tty);

    srand((uint32_t)rdtsc());

    setup_game(&game);

    uint64_t last_frame_time = rdtsc();

    while (!should_stop_pong)
    {
        uint64_t time = rdtsc();

        if (time - last_frame_time < CYCLES)
        {
            continue;
        }

        uint64_t elapsed = time - last_frame_time;
        float dt = (float)(elapsed) / (float)CYCLES;

        last_frame_time = time;

        tty_clear(&pong_tty, TTY_COLOR_BLACK);

        update_game(&game, dt);

        draw_frame();
        draw_ball(&game.ball);
        draw_paddle(&game.left_paddle);
        draw_paddle(&game.right_paddle);

        tty_flush(&pong_tty);
    }

    set_active_tty(&kernel_tty);
}
