#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

#define ADJUST_IMPLEMENTATION
#include "../../adjust.h"   // the adjust library.

#include <stdlib.h>
#include <math.h>

int SDL_RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius);

int SDL_RenderFillCircle(SDL_Renderer *renderer, int x, int y, int radius);

;
/**
 * This examples make used of the 'adjust.h' single header file library to do hot reloading int SDL3.
 * The same example as the raylib one.
 *
 * Hope you all will enjoy.
 */
int main(int argc, char **argv) {
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to initialize the sdl3 library. Error: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    adjust_init();

    int window_width = 800;
    int window_height = 600;

    SDL_Window *window;
    SDL_Renderer *renderer;
    if (!SDL_CreateWindowAndRenderer(
        "Bouncing ball [sdl3]",
        window_width,
        window_height,
        SDL_WINDOW_RESIZABLE,
        &window,
        &renderer
    ))
    {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Failed to create the window or the renderer. Error: %s", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    bool is_running = true;
    SDL_Event event;

    struct
    {
        float x;
        float y;
    } ball_position = { window_width / 2, window_height / 2 };
    struct
    {
        float x_speed;
        float y_speed;
    } ball_speed = { 5, 4 };

    // float ball_radius = 50.0f;
    // float gravity = 0.9f;
    ADJUST_CONST_INT(ball_radius, 10);
    ADJUST_CONST_FLOAT(gravity, 0.6f);

    while (is_running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE)
                {
                    is_running = false;
                }
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED)
            {
                is_running = false;
            }
        }

        adjust_update();

        ball_position.x += ball_speed.x_speed;
        ball_position.y += ball_speed.y_speed;

        if (gravity < 9.4f)
            ball_speed.y_speed += gravity;

        // check wall collisions with bottom wall
        if (ball_position.x >= (window_width - ball_radius) || ball_position.x <= ball_radius)
        {
            ball_speed.x_speed *= -1.0f;
        }
        if (ball_position.y >= (window_height - ball_radius) || ball_position.y <= ball_radius)
        {
            ball_speed.y_speed *= -0.95f;
        }

        SDL_GetWindowSize(window, &window_width, &window_height);
        SDL_SetRenderDrawColor(renderer, 155.0f, 155.0f, 205.0f, 255);
        SDL_RenderClear(renderer);

        // render a circle
        SDL_SetRenderDrawColor(renderer, 255.0f, 0.0f, 0.0f, 255);
        SDL_RenderFillCircle(renderer, ball_position.x, ball_position.y, ball_radius);

        SDL_RenderPresent(renderer);

        SDL_Delay(16);
    }

    adjust_cleanup();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

int
SDL_RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status |= SDL_RenderPoint(renderer, x + offsetx, y + offsety);
        status |= SDL_RenderPoint(renderer, x + offsety, y + offsetx);
        status |= SDL_RenderPoint(renderer, x - offsetx, y + offsety);
        status |= SDL_RenderPoint(renderer, x - offsety, y + offsetx);
        status |= SDL_RenderPoint(renderer, x + offsetx, y - offsety);
        status |= SDL_RenderPoint(renderer, x + offsety, y - offsetx);
        status |= SDL_RenderPoint(renderer, x - offsetx, y - offsety);
        status |= SDL_RenderPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

int SDL_RenderFillCircle(SDL_Renderer *renderer, int x, int y, int radius)
{
    if (!renderer || radius <= 0) {
        return -1;
    }
    
    const int point_number = 360;
    SDL_Vertex *vertices = malloc(sizeof(SDL_Vertex) * (point_number + 1));
    if (!vertices) {
        return -1;
    }
    
    float g = ADJUST_FLOAT(5.0f);
    float r = ADJUST_FLOAT(5.0f);
    SDL_FColor color = {r, g, 0.0f, 255};
    
    vertices[0].position.x = (float)x;
    vertices[0].position.y = (float)y;
    vertices[0].color = color;
    
    for (int i = 0; i < point_number; i++) {
        float angle = (float)i * (2.0f * (float)M_PI) / (float)point_number;
        vertices[i + 1].position.x = (float)x + cosf(angle) * (float)radius;
        vertices[i + 1].position.y = (float)y + sinf(angle) * (float)radius;
        vertices[i + 1].color = color;
    }
    
    int *indices = malloc(sizeof(int) * point_number * 3);
    if (!indices) {
        free(vertices);
        return -1;
    }
    
    for (int i = 0; i < point_number; i++) {
        indices[i * 3] = 0; // Center
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i + 1) % point_number + 1;
    }
    
    int result = SDL_RenderGeometry(renderer, NULL, vertices, point_number + 1, 
                                   indices, point_number * 3);
    
    free(vertices);
    free(indices);
    return result;
}
