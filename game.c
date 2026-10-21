#include "game.h"

const Uint8 red[PARTICLE_TYPES] = RED;
const Uint8 green[PARTICLE_TYPES] = GREEN;
const Uint8 blue[PARTICLE_TYPES] = BLUE;

bool game_init(struct Game *g);
void game_events(struct Game *g);
void game_draw(const struct Game *g);
void rules_new(struct Game *g);
void particles_new(struct Game *g);
void particles_update(struct Game *g);

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

bool game_new(struct Game **game) {
	*game = calloc(1, sizeof(struct Game));
	if (*game == NULL) {
		perror("Error allocating game structure");
		return false;
	}
	struct Game *g = *game;

	if (!game_init(g)) {
		return false;
	}
	srand((unsigned)time(NULL));
	particles_new(g);
	rules_new(g);

	g->is_running = true;
	
	return true;
}

void game_free(struct Game **game) {
	if (*game != NULL) {
		struct Game *g = *game;

		if (g->renderer != NULL) {
			SDL_DestroyRenderer(g->renderer);
			g->renderer = NULL;
		}

		if (g->window != NULL) {
			SDL_DestroyWindow(g->window);
			g->window = NULL;
		}

		SDL_Quit();

		free(g);
		g = NULL;
		*game = NULL;

		printf("All Clean!\n");
	}
}

void rules_new(struct Game *g) {
	for (int i = 0; i < PARTICLE_TYPES; i++) {
		for (int j = 0; j < PARTICLE_TYPES; j++) {
			g->rules[i][j] = ((float)rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		}
	}
}

void particles_new(struct Game *g) {
	for (int i = 0; i < PARTICLE_NUMBER; i++) {
		g->particles[i].x = ((float)rand() / (float)RAND_MAX) * (float)WINDOW_WIDTH;
		g->particles[i].y = ((float)rand() / (float)RAND_MAX) * (float)WINDOW_HEIGHT;
		g->particles[i].type = rand() % PARTICLE_TYPES;
	}
}

void particles_update(struct Game *g) {
	for (int i = 0; i < PARTICLE_NUMBER; i++) {
		for (int j = 0; j < PARTICLE_NUMBER; j++) {
			if (i == j) continue;

			float dx = g->particles[j].x - g->particles[i].x;
		       	float dy = g->particles[j].y - g->particles[i].y;	
			
			//We need to take care of the wraping of the screen when calculating distances
			if (dx > WINDOW_WIDTH * 0.5f) dx -= WINDOW_WIDTH;
			else if (dx < -WINDOW_WIDTH * 0.5f) dx += WINDOW_WIDTH;
			if (dy > WINDOW_HEIGHT * 0.5f) dy -= WINDOW_HEIGHT;
			else if (dy < -WINDOW_HEIGHT * 0.5f) dy += WINDOW_HEIGHT;

			float distance = sqrtf(dx * dx + dy * dy);
			
			if (distance > 0.0f && distance < INTERACTION_RADIUS) {
				float force;
				//universal repulsion if two particles get too close
				if (distance < REPULSION_RADIUS) {
					force = distance / REPULSION_RADIUS - 1.0f;
				}
				//interaction between types (between REPULSION_RADIUS and INTERACTION_RADIUS)
				else {
					float interaction = g->rules[g->particles[i].type][g->particles[j].type];
					float range = INTERACTION_RADIUS - REPULSION_RADIUS;
					float norm_distance = (distance - REPULSION_RADIUS) / range;
					//interaction between types is implemented using a triangular function between two radii
					//maximum force present in the midpoint
					force = interaction * (1.0f - fabsf(norm_distance * 2.0f - 1.0f));
				}			
				//force here is equal to acceleration as m=1 for every particle
				g->particles[i].x_vel += (dx / distance) * force * 0.5f;
				g->particles[i].y_vel += (dy / distance) * force * 0.5f;
			}
		}
	}

	for (int i = 0; i < PARTICLE_NUMBER; i++) {
		g->particles[i].x += g->particles[i].x_vel;
		g->particles[i].y += g->particles[i].y_vel;
		
		//we need friction so that velocity does not accummulate indefinetly
		g->particles[i].x_vel *= FRICTION;
		g->particles[i].y_vel *= FRICTION;

		if (g->particles[i].x < 0) g->particles[i].x += WINDOW_WIDTH;
		else if (g->particles[i].x >= WINDOW_WIDTH) g->particles[i].x -= WINDOW_WIDTH;
		if (g->particles[i].y < 0) g->particles[i].y += WINDOW_HEIGHT;
		else if (g->particles[i].y >= WINDOW_HEIGHT) g->particles[i].y -= WINDOW_HEIGHT;
	}
}

void game_events(struct Game *g) {
	while(SDL_PollEvent(&g->event)) {
		switch (g->event.type) {
			case SDL_EVENT_QUIT:
				g->is_running = false;
				break;
			case SDL_EVENT_KEY_DOWN:
				switch (g->event.key.scancode) {
					case SDL_SCANCODE_R:
						rules_new(g);
						break;
					default:
						break;
				}
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
		particles_update(g);
		game_draw(g);
		SDL_Delay(16);
	}
}

