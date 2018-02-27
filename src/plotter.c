#include <math.h>

#include "plotter.h"


static double to_window_x (plotter *plotter,
                           double x);

static double to_window_y (plotter *plotter,
                           double y);

static int to_pixel (double v);


int plotter_open (plotter *plotter,
                  const char *title,
                  int width,
                  int height,
                  unsigned int options)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        return -1;
    }
    plotter->window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if (plotter->window == NULL) {
        return -2;
    }
    plotter->renderer = SDL_CreateRenderer(plotter->window, -1, 0);
    if (plotter->renderer == NULL) {
        return -3;
    }
    plotter->width = width;
    plotter->height = height;
    plotter->bgcolor[0] = 0;
    plotter->bgcolor[1] = 0;
    plotter->bgcolor[2] = 0;
    plotter->bgcolor[3] = 0;
    plotter->color[0] = 255;
    plotter->color[1] = 255;
    plotter->color[2] = 255;
    plotter->color[3] = 0;
    plotter->cx = 0;
    plotter->cy = 0;
    plotter->scale = 1;
    plotter->grid_step = 1;
    plotter->options = options;
    return 0;
}

int plotter_close (plotter *plotter)
{
    SDL_DestroyRenderer(plotter->renderer);
    SDL_DestroyWindow(plotter->window);
    SDL_Quit();
    return 0;
}

void plotter_set_bgcolor (plotter *plotter,
                          unsigned char r,
                          unsigned char g,
                          unsigned char b,
                          unsigned char a)
{
    plotter->bgcolor[0] = r;
    plotter->bgcolor[1] = g;
    plotter->bgcolor[2] = b;
    plotter->bgcolor[3] = a;
}

void plotter_set_color (plotter *plotter,
                        unsigned char r,
                        unsigned char g,
                        unsigned char b,
                        unsigned char a)
{
    plotter->color[0] = r;
    plotter->color[1] = g;
    plotter->color[2] = b;
    plotter->color[3] = a;
}

void plotter_set_center (plotter *plotter,
                         double cx,
                         double cy)
{
    plotter->cx = cx;
    plotter->cy = cy;
}

void plotter_set_scale (plotter *plotter,
                        double scale)
{
    plotter->scale = scale;
}

void plotter_set_grid_step (plotter *plotter,
                            double step)
{
    plotter->grid_step = step;
}

void plotter_move_center (plotter *plotter,
                          double dpx,
                          double dpy)
{
    plotter->cx += dpx / plotter->scale;
    plotter->cy += dpy / plotter->scale;
}

int plotter_draw_begin (plotter *plotter)
{
    SDL_Rect rect;
    double x, y;
    int i;

    SDL_SetRenderDrawColor(plotter->renderer, plotter->bgcolor[0], plotter->bgcolor[1], plotter->bgcolor[2], plotter->bgcolor[3]);
    SDL_RenderClear(plotter->renderer);

    if (plotter->options & PLOTTER_DRAW_GRID) {
        //TODO sta roba ha senso?
        double dist = plotter->scale * plotter->grid_step;
        while (dist < 10) {
            dist *= 10;
        }
        while (dist/10 > 10) {
            dist /= 10;
        }
        //vertical lines, positive x
        x = to_window_x(plotter, 0) + dist;
        i = 1;
        while (x <= plotter->width) {
            if (i % 10 == 0) {
                SDL_SetRenderDrawColor(plotter->renderer, 159, 159, 159, 0);
            } else {
                SDL_SetRenderDrawColor(plotter->renderer, 127, 127, 127, 0);
            }
            SDL_RenderDrawLine(plotter->renderer, to_pixel(x), 0, to_pixel(x), plotter->height);
            x += dist;
            i++;
        }
        //vertical lines, negative x
        x = to_window_x(plotter, 0) - dist;
        i = 1;
        while (x >= 0) {
            if (i % 10 == 0) {
                SDL_SetRenderDrawColor(plotter->renderer, 159, 159, 159, 0);
            } else {
                SDL_SetRenderDrawColor(plotter->renderer, 127, 127, 127, 0);
            }
            SDL_RenderDrawLine(plotter->renderer, to_pixel(x), 0, to_pixel(x), plotter->height);
            x -= dist;
            i++;
        }
        //horizontal lines, positive y
        y = to_window_y(plotter, 0) + dist;
        i = 1;
        while (y <= plotter->height) {
            if (i % 10 == 0) {
                SDL_SetRenderDrawColor(plotter->renderer, 159, 159, 159, 0);
            } else {
                SDL_SetRenderDrawColor(plotter->renderer, 127, 127, 127, 0);
            }
            SDL_RenderDrawLine(plotter->renderer, 0, to_pixel(y), plotter->width, to_pixel(y));
            y += dist;
            i++;
        }
        //horizontal lines, negative y
        y = to_window_y(plotter, 0) - dist;
        i = 1;
        while (y >= 0) {
            if (i % 10 == 0) {
                SDL_SetRenderDrawColor(plotter->renderer, 159, 159, 159, 0);
            } else {
                SDL_SetRenderDrawColor(plotter->renderer, 127, 127, 127, 0);
            }
            SDL_RenderDrawLine(plotter->renderer, 0, to_pixel(y), plotter->width, to_pixel(y));
            y -= dist;
            i++;
        }
    }

    if (plotter->options & PLOTTER_DRAW_AXES) {
        x = to_pixel(to_window_x(plotter, 0));
        y = to_pixel(to_window_y(plotter, 0));
        SDL_SetRenderDrawColor(plotter->renderer, 191, 191, 191, 0);
        SDL_RenderDrawLine(plotter->renderer, 0, y, plotter->width, y);
        SDL_RenderDrawLine(plotter->renderer, x, 0, x, plotter->height);
    }

    if (plotter->options & PLOTTER_DRAW_ORIGIN) {
        //upper-left corner
        rect.x = to_pixel(to_window_x(plotter, 0)) - 1;
        rect.y = to_pixel(to_window_y(plotter, 0)) - 1;
        rect.w = 3;
        rect.h = 3;
        SDL_SetRenderDrawColor(plotter->renderer, 223, 223, 223, 0);
        SDL_RenderFillRect(plotter->renderer, &rect);
    }

    if (plotter->options & PLOTTER_DRAW_CURSOR) {
        SDL_SetRenderDrawColor(plotter->renderer, 223, 223, 223, 0);
        /*
        SDL_RenderDrawLine(plotter->renderer, to_pixel(plotter->width/2)-5, to_pixel(plotter->height/2), to_pixel(plotter->width/2)+5, to_pixel(plotter->height/2));
        SDL_RenderDrawLine(plotter->renderer, to_pixel(plotter->width/2), to_pixel(plotter->height/2)-5, to_pixel(plotter->width/2), to_pixel(plotter->height/2)+5);
        */
        SDL_RenderDrawLine(plotter->renderer, to_pixel(plotter->width/2)-2, to_pixel(plotter->height/2), to_pixel(plotter->width/2)-7, to_pixel(plotter->height/2));
        SDL_RenderDrawLine(plotter->renderer, to_pixel(plotter->width/2)+2, to_pixel(plotter->height/2), to_pixel(plotter->width/2)+7, to_pixel(plotter->height/2));
        SDL_RenderDrawLine(plotter->renderer, to_pixel(plotter->width/2), to_pixel(plotter->height/2)-2, to_pixel(plotter->width/2), to_pixel(plotter->height/2)-7);
        SDL_RenderDrawLine(plotter->renderer, to_pixel(plotter->width/2), to_pixel(plotter->height/2)+2, to_pixel(plotter->width/2), to_pixel(plotter->height/2)+7);
    }

    return 0;
}

int plotter_draw_end (plotter *plotter)
{
    SDL_RenderPresent(plotter->renderer);
    return 0;
}

int plotter_draw_point (plotter *plotter,
                        double x,
                        double y)
{
    SDL_SetRenderDrawColor(plotter->renderer, plotter->color[0], plotter->color[1], plotter->color[2], plotter->color[3]);
    SDL_RenderDrawPoint(plotter->renderer, to_pixel(to_window_x(plotter, x)), to_pixel(to_window_y(plotter, y)));
    return 0;
}

int plotter_draw_rect (plotter *plotter,
                       double x,
                       double y,
                       double w,
                       double h,
                       int fill)
{
    SDL_Rect rect;
    //upper-left corner
    rect.x = to_pixel(to_window_x(plotter, x-w/2));
    rect.y = to_pixel(to_window_y(plotter, y+h/2));
    rect.w = to_pixel(plotter->scale * w);
    rect.h = to_pixel(plotter->scale * h);
    if (rect.w == 0) {
        rect.w = 1;
    }
    if (rect.h == 0) {
        rect.h = 1;
    }
    SDL_SetRenderDrawColor(plotter->renderer, plotter->color[0], plotter->color[1], plotter->color[2], plotter->color[3]);
    if (fill) {
        SDL_RenderFillRect(plotter->renderer, &rect);
    } else {
        SDL_RenderDrawRect(plotter->renderer, &rect);
    }
    return 0;
}

int plotter_draw_func (plotter *plotter,
                       double x1,
                       double x2,
                       double step,
                       double (*func)(double),
                       int points)
{
    //TODO draw within visible space ?
    double x, y1, y2;
    int px1, px2, py1, py2;
    SDL_SetRenderDrawColor(plotter->renderer, plotter->color[0], plotter->color[1], plotter->color[2], plotter->color[3]);
    if (points) {
        for (x = x1; x <= x2; x += step) {
            y1 = func(x);
            plotter_draw_point(plotter, x, y1);
        }
    } else {
        for (x = x1; x <= x2-step; x += step) {
            y1 = func(x);
            y2 = func(x+step);
            px1 = to_pixel(to_window_x(plotter, x));
            py1 = to_pixel(to_window_y(plotter, y1));
            px2 = to_pixel(to_window_x(plotter, x+step));
            py2 = to_pixel(to_window_y(plotter, y2));
            SDL_RenderDrawLine(plotter->renderer, px1, py1, px2, py2);
        }
    }
    return 0;
}

static double to_window_x (plotter *plotter,
                           double x)
{
    return plotter->width/2 - plotter->scale * (plotter->cx - x);
}

static double to_window_y (plotter *plotter,
                           double y)
{
    return plotter->height/2 + plotter->scale * (plotter->cy - y);
}

static int to_pixel (double v)
{
    /*double f = floor(v);
    return v-f >= 0.5 ? f+1 : f;*/
    return round(v);
}
