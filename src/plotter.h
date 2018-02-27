#ifndef PLOTTER_H_INCLUDED
#define PLOTTER_H_INCLUDED


#include <SDL2/SDL.h>


#define PLOTTER_DRAW_ORIGIN  (1 << 0)
#define PLOTTER_DRAW_AXES    (1 << 1)
#define PLOTTER_DRAW_GRID    (1 << 2)
#define PLOTTER_DRAW_CURSOR  (1 << 3)


typedef struct plotter_s {
    int width, height;
    unsigned char bgcolor[4]; //rgba
    unsigned char color[4]; //rgba
    double cx, cy; //view center
    double scale;
    double grid_step;
    unsigned int options;
    SDL_Window *window;
    SDL_Renderer *renderer;
} plotter;


int plotter_open (plotter *plotter,
                  const char *title,
                  int width,
                  int height,
                  unsigned int options);

int plotter_close (plotter *plotter);

void plotter_set_bgcolor (plotter *plotter,
                          unsigned char r,
                          unsigned char g,
                          unsigned char b,
                          unsigned char a);

void plotter_set_color (plotter *plotter,
                        unsigned char r,
                        unsigned char g,
                        unsigned char b,
                        unsigned char a);

void plotter_set_center (plotter *plotter,
                         double cx,
                         double cy);

void plotter_set_scale (plotter *plotter,
                        double scale);

void plotter_set_grid_step (plotter *plotter,
                            double step);

void plotter_move_center (plotter *plotter,
                          double dpx,
                          double dpy);

int plotter_draw_begin (plotter *plotter);

int plotter_draw_end (plotter *plotter);

int plotter_draw_point (plotter *plotter,
                        double x,
                        double y);

int plotter_draw_rect (plotter *plotter,
                       double x,
                       double y,
                       double w,
                       double h,
                       int fill);

int plotter_draw_func (plotter *plotter,
                       double x1,
                       double x2,
                       double step,
                       double (*func)(double),
                       int points);


#endif // PLOTTER_H_INCLUDED
