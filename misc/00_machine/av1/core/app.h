#include "SDL.h"
#include <avrcpu.h>

class App {

protected:

    int width, height;

    SDL_Event    event;
    SDL_Surface* sdl_screen;
    AVRCPU*      avrcpu;

    int     cpu_start;

public:

    App(int, int, const char*);

    void init(int, char**);
    void start();
    void update();
    void pset(int, int, uint);
    void flip();

    int  get_key(SDL_Event);
};
