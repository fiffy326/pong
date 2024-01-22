#include <SDL2/SDL.h>

static const char *win_title = "Pong Clone";
static const int win_w = 800;
static const int win_h = 600;
static const int fps = 60;
static int quit = 0;

SDL_Window *window;
SDL_Renderer *renderer;

struct Paddle {
    SDL_Rect box;
    int y_vel;
    int speed;
};

struct Ball {
    SDL_Rect box;
    int x_vel;
    int y_vel;
};

static SDL_Rect win_top, win_bottom, win_left, win_right;
static struct Paddle paddle[2];
static struct Ball ball;

void init(void)
{
    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
        exit(1);
    }

    /* Create the window */
    window = SDL_CreateWindow(win_title, SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, win_w, win_h,
                              SDL_WINDOW_OPENGL);
    if (window == NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
        exit(2);
    }

    /* Create the renderer */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) {
        printf("SDL_CreateRenderer error: %s\n", SDL_GetError());
        exit(3);
    }

    /* Initialize the left paddle */
    paddle[0].box.w = 20;
    paddle[0].box.h = 120;
    paddle[0].box.x = 0;
    paddle[0].box.y = (win_h / 2) - (paddle[0].box.h / 2);
    paddle[0].y_vel = 0;
    paddle[0].speed = 6;

    /* Initialize the right paddle */
    paddle[1].box.w = 20;
    paddle[1].box.h = 120;
    paddle[1].box.x = win_w - paddle[1].box.w;
    paddle[1].box.y = (win_h / 2) - (paddle[1].box.h / 2);
    paddle[1].y_vel = 0;
    paddle[1].speed = 6;

    /* Initialize the ball */
    ball.box.w = 8;
    ball.box.h = 8;
    ball.box.x = (win_w / 2) - (ball.box.w / 2);
    ball.box.y = (win_h / 2) - (ball.box.h / 2);
    ball.x_vel = 5;
    ball.y_vel = 3;

    /* Initialize the out of bounds barriers */
    win_top    = (SDL_Rect) { 0, -10, win_w, 10 };
    win_bottom = (SDL_Rect) { 0, win_h, win_w, 10 };
    win_left   = (SDL_Rect) { -10, 0, 10, win_h };
    win_right  = (SDL_Rect) { win_w, 0, 10, win_h };
}

void cleanup(void)
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void processKeyDown(SDL_Event *event)
{
    if (!event->key.repeat) {
        switch (event->key.keysym.sym) {
            case SDLK_w:
                paddle[0].y_vel = -paddle[0].speed;
                break;

            case SDLK_s:
                paddle[0].y_vel = paddle[0].speed;
                break;

            case SDLK_UP:
                paddle[1].y_vel = -paddle[1].speed;
                break;

            case SDLK_DOWN:
                paddle[1].y_vel = paddle[1].speed;
                break;
        }
    }
}

void processKeyUp(SDL_Event *event)
{
    if (!event->key.repeat) {
        switch (event->key.keysym.sym) {
            case SDLK_w:
                paddle[0].y_vel = 0;
                break;

            case SDLK_s:
                paddle[0].y_vel = 0;
                break;

            case SDLK_UP:
                paddle[1].y_vel = 0;
                break;

            case SDLK_DOWN:
                paddle[1].y_vel = 0;
                break;
        }
    }
}

void processInput(void)
{
    static SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            quit = 1;
            break;

        case SDL_KEYDOWN:
            processKeyDown(&event);
            break;

        case SDL_KEYUP:
            processKeyUp(&event);
            break;
        }
    }
}

void renderFrame(void)
{
    /* Draw background */
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);

    /* Draw paddles and ball */
    SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderFillRect(renderer, &paddle[0].box);
    SDL_RenderFillRect(renderer, &paddle[1].box);
    SDL_RenderFillRect(renderer, &ball.box);

    /* Render the frame */
    SDL_RenderPresent(renderer);
}

void updatePaddles(void)
{
    for (int i = 0; i < 2; i++) {
        /* Paddle hits the window's top edge */
        if (SDL_HasIntersection(&paddle[i].box, &win_top)) {
            paddle[i].y_vel = 0;
            paddle[i].box.y = 0;
            return;
        }

        /* Paddle hits the window's bottom edge */
        if (SDL_HasIntersection(&paddle[i].box, &win_bottom)) {
            paddle[i].y_vel = 0;
            paddle[i].box.y = win_h - paddle[i].box.h;
            return;
        }

        /* Paddle is within the window */
        paddle[i].box.y += paddle[i].y_vel;
    }
}

void updateBall(void)
{
    /* Ball hits the window's top edge */
    if (SDL_HasIntersection(&ball.box, &win_top)) {
        ball.y_vel *= -1;
    }

    /* Ball hits the window's bottom edge */
    if (SDL_HasIntersection(&ball.box, &win_bottom)) {
        ball.y_vel *= -1;
    }

    /* Ball hits the window's left edge */
    if (SDL_HasIntersection(&ball.box, &win_left)) {
        quit = 1;
    }

    /* Ball hits the window's right edge */
    if (SDL_HasIntersection(&ball.box, &win_right)) {
        quit = 1;
    }

    /* Ball hits a paddle */
    for (int i = 0; i < 2; i++) {
        if (SDL_HasIntersection(&ball.box, &paddle[i].box)) {
            ball.x_vel *= -1;
        }
    }

    /* Update the position */
    ball.box.x += ball.x_vel;
    ball.box.y += ball.y_vel;
}

void updateState(void)
{
    updatePaddles();
    updateBall();
}

int main(void)
{
    init();

    /* Game loop */
    while (!quit) {
        processInput();
        updateState();
        renderFrame();
        SDL_Delay(1000 / fps);
    }

    cleanup();
    return 0;
}
