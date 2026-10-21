#ifndef GAME_H
#define GAME_H

#include "main.h"

struct Particle {
	float x;
	float y;
	float x_vel;
	float y_vel;
	int type;
};

struct Game {
	//particle management
	struct Particle particles[PARTICLE_NUMBER];
	float rules[PARTICLE_TYPES][PARTICLE_TYPES];
	//game engine
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	bool is_running;
};

bool game_new(struct Game **game);
void game_run(struct Game *g);
void game_free(struct Game **game);

#endif
