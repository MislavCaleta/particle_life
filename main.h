#ifndef MAIN_H
#define MAIN_H

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//game init definitions
#define SDL_INIT_FLAGS SDL_INIT_VIDEO
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Particle Life"

//particle definitions
#define PARTICLE_SIZE 3.0f
#define PARTICLE_NUMBER 2000
#define PARTICLE_TYPES 6
#define REPULSION_RADIUS 20.0f
#define INTERACTION_RADIUS 80.0f
#define FRICTION 0.85
#define RED {255, 200, 150, 100, 50, 0}
#define GREEN {0, 50, 100, 150, 200, 255}
#define BLUE {100, 150, 50, 0, 255, 200}

#endif
