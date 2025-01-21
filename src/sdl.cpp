#include <stdio.h>
#include <SDL2/SDL.h>

#define WIDTH 1000
#define HEIGHT 800
#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define RAY_LENGHT 100
#define RAY_NUM 360

struct Circle
{
    int x;
    int y;
    int radius;
};

struct Ray
{
    float x_s, y_s;
    float angle;
    float x_e, y_e;
};

void FillCircle(SDL_Surface *surface, struct Circle circle, Uint32 color)
{
    for (int y = -circle.radius; y <= circle.radius; y++)
    {
        for (int x = -circle.radius; x <= circle.radius; x++)
        {
            if (x * x + y * y <= circle.radius * circle.radius)
            {
                int pixelX = circle.x + x;
                int pixelY = circle.y + y;
                SDL_Rect pixel = (SDL_Rect){pixelX, pixelY, 1, 1};
                if (pixelX >= 0 && pixelX < surface->w && pixelY >= 0 && pixelY < surface->h)
                {
                    SDL_FillRect(surface, &pixel, color);
                }
            }
        }
    }
}

void CreateRays(struct Circle circle, struct Circle shadow_circle, struct Ray rays[RAY_NUM])
{
    for (int i = 0; i < RAY_NUM; i++)
    {
        rays[i].x_s = circle.x;
        rays[i].y_s = circle.y;
        rays[i].angle = i * 360 / RAY_NUM;
        rays[i].x_e = circle.x + circle.radius * SDL_cos(rays[i].angle) * RAY_LENGHT;
        rays[i].y_e = circle.y + circle.radius * SDL_sin(rays[i].angle) * RAY_LENGHT;
    }

    return;
}

void CheckIntersect(struct Ray rays[RAY_NUM], struct Circle shadow_circle)
{
    for (int i = 0; i < RAY_NUM; i++)
    {
        float x1 = rays[i].x_s;
        float y1 = rays[i].y_s;
        float x2 = rays[i].x_e;
        float y2 = rays[i].y_e;

        float x3 = shadow_circle.x;
        float y3 = shadow_circle.y;
        float r = shadow_circle.radius;

        float dx = x2 - x1;
        float dy = y2 - y1;

        float A = dx * dx + dy * dy;
        float B = 2 * (dx * (x1 - x3) + dy * (y1 - y3));
        float C = (x1 - x3) * (x1 - x3) + (y1 - y3) * (y1 - y3) - r * r;

        float delta = B * B - 4 * A * C;

        if (delta >= 0)
        {
            float t1 = (-B - SDL_sqrt(delta)) / (2 * A);
            float t2 = (-B + SDL_sqrt(delta)) / (2 * A);

            if (t1 >= 0 && t1 <= 1)
            {
                rays[i].x_e = x1 + t1 * dx;
                rays[i].y_e = y1 + t1 * dy;
            }
            else if (t2 >= 0 && t2 <= 1)
            {
                rays[i].x_e = x1 + t2 * dx;
                rays[i].y_e = y1 + t2 * dy;
            }
        }
    }
}

void DrawRays(SDL_Renderer *renderer, struct Ray rays[RAY_NUM])
{
    for (int i = 0; i < RAY_NUM; i++)
    {
        SDL_RenderDrawLine(renderer, rays[i].x_s, rays[i].y_s, rays[i].x_e, rays[i].y_e);
    }
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window *window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);

    // SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_Surface *surface = SDL_GetWindowSurface(window);

    SDL_Renderer *renderer = SDL_CreateSoftwareRenderer(surface);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);                                                                                                                                                                        // Set the color used for drawing operations.


    struct Circle circle = {500, 500, 100};
    struct Circle shadow_circle = {700, 500, 140};
    struct Circle shadow_circle2 = {200, 200, 40};
    struct Circle shadow_circle3 = {250, 600, 40};

    int sim_running = 1;
    SDL_Event event;

    while (sim_running)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                sim_running = 0;
                break;

            case SDL_MOUSEMOTION:
                if (event.motion.state & SDL_BUTTON_LMASK)
                {
                    circle.x = event.motion.x;
                    circle.y = event.motion.y;
                }
                if (event.motion.state & SDL_BUTTON_RMASK)
                {
                    shadow_circle.x = event.motion.x;
                    shadow_circle.y = event.motion.y;
                }
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT)
                {
                    circle.x = event.button.x;
                    circle.y = event.button.y;
                }
                if (event.button.button == SDL_BUTTON_RIGHT)
                {
                    shadow_circle.x = event.button.x;
                    shadow_circle.y = event.button.y;
                }
                break;

            default:
                break;
            }
        }
        SDL_RenderClear(renderer);

        circle.x = SDL_max(0, circle.x);
        circle.x = SDL_min(WIDTH, circle.x);
        circle.y = SDL_max(0, circle.y);
        circle.y = SDL_min(HEIGHT, circle.y);

        SDL_FillRect(surface, NULL, COLOR_BLACK);
        FillCircle(surface, circle, COLOR_WHITE);
        struct Ray rays[RAY_NUM];
        CreateRays(circle, shadow_circle, rays);
        CheckIntersect(rays, shadow_circle);
        CheckIntersect(rays, shadow_circle2);
        CheckIntersect(rays, shadow_circle3);
        DrawRays(renderer, rays);
        FillCircle(surface, shadow_circle, 0x111111ff);
        FillCircle(surface, shadow_circle2, 0x111111ff);
        FillCircle(surface, shadow_circle3, 0x111111ff);

        SDL_RenderPresent(renderer);
        SDL_UpdateWindowSurface(window);
        SDL_PumpEvents();
        SDL_Delay(5);
    }

    SDL_DestroyWindow(window);
    SDL_Quit();
}