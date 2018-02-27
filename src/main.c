#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "plotter.h"


static double cube (double x)
{
    return x*x*x*0.001;
}


int main (int argc, char *argv[])
{
    plotter plotter;
    SDL_Event event;
    int frametime;
    int running = 1;
    double scale = 1;

    plotter_open(&plotter, "Plotter Demo", 800, 600, PLOTTER_DRAW_ORIGIN | PLOTTER_DRAW_AXES | PLOTTER_DRAW_GRID);

    while (running) {

        frametime = -SDL_GetTicks();

        while (SDL_PollEvent(&event) != 0) {
            switch (event.type) {
                case SDL_QUIT:
                    running = 0;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = 0;
                            break;
                        case SDLK_PLUS:
                        case SDLK_KP_PLUS:
                            scale *= 2;
                            plotter_set_scale(&plotter, scale);
                            break;
                        case SDLK_MINUS:
                        case SDLK_KP_MINUS:
                            scale /= 2;
                            plotter_set_scale(&plotter, scale);
                            break;
                        case SDLK_LEFT:
                            plotter_move_center(&plotter, -50, 0);
                            break;
                        case SDLK_RIGHT:
                            plotter_move_center(&plotter, 50, 0);
                            break;
                        case SDLK_UP:
                            plotter_move_center(&plotter, 0, 50);
                            break;
                        case SDLK_DOWN:
                            plotter_move_center(&plotter, 0, -50);
                            break;
                    }
                    break;
            }
        }

        if (!running) {
            break;
        }

        plotter_draw_begin(&plotter);
        plotter_set_color(&plotter, 255, 255, 0, 0);
        plotter_draw_func(&plotter, -400, 400, 1, cube, 0);
        plotter_draw_end(&plotter);

        frametime += SDL_GetTicks();
        if (frametime < 1000.0/24) {
            SDL_Delay(1000.0/24 - frametime);
        }
    }

    plotter_close(&plotter);
    return 0;
}
