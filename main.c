#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>
#include <time.h>

//game init definitions
#define SDL_INIT_FLAGS SDL_INIT_VIDEO
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE "Particle Life"

//particle definitions
#define PARTICLE_SIZE 3.0f
#define PARTICLE_NUMBER 200
#define PARTICLE_TYPES 6
#define RED {255, 200, 150, 100, 50, 0}
#define GREEN {0, 50, 100, 150, 200, 255}
#define BLUE {100, 150, 50, 0, 255, 200}

const Uint8 red[PARTICLE_TYPES] = RED;
const Uint8 green[PARTICLE_TYPES] = GREEN;
const Uint8 blue[PARTICLE_TYPES] = BLUE;

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
	//game engine
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Event event;
	bool is_running;
};

bool game_init(struct Game *g);
bool game_new(struct Game *g);
void game_events(struct Game *g);
void game_run(struct Game *g);
void particles_new(struct Game *g);
void game_free(struct Game *g);

bool game_init(struct Game *g) {
	if(!SDL_Init(SDL_INIT_FLAGS)) {
		fprintf(stderr, "Error initializing SDL: %s\n", SDL_GetError());
		return false;
	}
	
	g->window = SDL_CreateWindow(WINDOW_TITLE, WINDOW_WIDTH, WINDOW_HEIGHT, 0);
	if (g->window == NULL) {
		fprintf(stderr, "Error creating window: %s\n", SDL_GetError());
		return false;
	}

	g->renderer = SDL_CreateRenderer(g->window, NULL);
	if (g->renderer == NULL) {
		fprintf(stderr, "Error creating renderer: %s\n", SDL_GetError());
		return false;
	}

	return true;
}

bool game_new(struct Game *g) {
	if (!game_init(g)) {
		return false;
	}
	srand((unsigned)time(NULL));
	particles_new(g);

	g->is_running = true;
	
	return true;
}

void game_free(struct Game *g) {
	if (g->renderer != NULL) {
		SDL_DestroyRenderer(g->renderer);
		g->renderer = NULL;
	}

	if (g->window != NULL) {
		SDL_DestroyWindow(g->window);
		g->window = NULL;
	}

	SDL_Quit();
	printf("All Clean!\n");
}

void particles_new(struct Game *g) {
	for (int i = 0; i < PARTICLE_NUMBER; i++) {
		g->particles[i].x = ((float)rand() / (float)RAND_MAX) * (float)WINDOW_WIDTH;
		g->particles[i].y = ((float)rand() / (float)RAND_MAX) * (float)WINDOW_HEIGHT;
		g->particles[i].type = rand() % PARTICLE_TYPES;
	}
}

void game_events(struct Game *g) {
	while(SDL_PollEvent(&g->event)) {
		switch (g->event.type) {
			case SDL_EVENT_QUIT:
				g->is_running = false;
				break;
			default:
				break;
		}
	}
}

void particles_draw(const struct Game *g) {
	SDL_FRect particle_rect = {0, 0, PARTICLE_SIZE, PARTICLE_SIZE};
	for (int i = 0; i < PARTICLE_NUMBER; i++) {
		particle_rect.x = g->particles[i].x - 0.5 * PARTICLE_SIZE;
		particle_rect.y = g->particles[i].y - 0.5 * PARTICLE_SIZE;
		//fix so that particles can not be the same color as background
		SDL_SetRenderDrawColor(g->renderer, red[g->particles[i].type], green[g->particles[i].type], blue[g->particles[i].type], 255);
		SDL_RenderFillRect(g->renderer, &particle_rect);
	}
}

void game_draw(const struct Game *g) {
	SDL_SetRenderDrawColor(g->renderer, 35, 30, 35, 255);
	SDL_RenderClear(g->renderer);
	particles_draw(g);
	SDL_RenderPresent(g->renderer);
}

void game_run(struct Game *g) {
	while (g->is_running) {
		game_events(g);
		game_draw(g);
	}
}

int main(void) {
	int exit_status = EXIT_FAILURE;
	struct Game game = {0};

	if(game_new(&game)) {
		game_run(&game);
		exit_status = EXIT_SUCCESS;
	}

	game_free(&game);

	return exit_status;
}
