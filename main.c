#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sge.h>

#define CHUNKSIZE 10
#define BLOCKSIZE 5
#define PRECISION 32

struct chunk {
	int myX, myY;
	int types[CHUNKSIZE][CHUNKSIZE];
	struct chunk *next;
};

struct chunk *firstChunk = NULL;

struct position {
	int chunkX, chunkY;
	int x, y;
};

struct position myPos;

char isInsideAnything(struct position where)
{
	struct chunk *runner = firstChunk;
	while (runner) {
		if (runner->myX == where.chunkX && runner->myY == where.chunkY) {
			return (0 != runner->types[where.y/PRECISION][where.x/PRECISION]);
		}
		runner = runner->next;
	}
	return 0;
}

void move(int dx, int dy)
{
	struct position oldPos = myPos;

	myPos.x += dx;
	while (myPos.x < 0) {
		myPos.x += PRECISION * CHUNKSIZE;
		myPos.chunkX--;
	}
	while (myPos.x >= PRECISION * CHUNKSIZE) {
		myPos.x -= PRECISION * CHUNKSIZE;
		myPos.chunkX++;
	}
	myPos.y += dy;
	while (myPos.y < 0) {
		myPos.y += PRECISION * CHUNKSIZE;
		myPos.chunkY--;
	}
	while (myPos.y >= PRECISION * CHUNKSIZE) {
		myPos.y -= PRECISION * CHUNKSIZE;
		myPos.chunkY++;
	}

	if (isInsideAnything(myPos))
		myPos = oldPos;
}

int keyFunc(SGEGAMESTATE *state, SGEEVENT *event)
{
	switch (event->key.keysym.sym) {
	case SDLK_LEFT:
		move(-PRECISION, 0);
		break;
	case SDLK_RIGHT:
		move(PRECISION, 0);
		break;
	case SDLK_UP:
		move(0, -PRECISION);
		break;
	case SDLK_DOWN:
		move(0, PRECISION);
		break;
	case SDLK_ESCAPE:
		sgeGameStateManagerQuit(state->manager);
		break;
	default:
		return EVENT_UNHANDLED;
	}
	
	return EVENT_HANDLED;
}

void generateChunk(int x, int y)
{
	struct chunk *newChunk = malloc(sizeof(struct chunk));
	newChunk->myX = x;
	newChunk->myY = y;
	newChunk->next = firstChunk;
	firstChunk = newChunk;
	for (x = 0; x < CHUNKSIZE; x++) {
		for (y = 0; y < CHUNKSIZE; y++) {
			newChunk->types[y][x] = random() % 4 == 0;
		}
	}
}

void onRedraw(SGEGAMESTATE *state)
{
	//This is inefficient - we should only have to check for chunks whenever I cross a chunk border.
	int requiredChunks[9][2];
	int numRequired = 0;
	int i, j;
	int x, y;
	for (i = -1; i <= 1; i++) {
		for (j = -1; j <= 1; j++) {
			requiredChunks[numRequired][0] = myPos.chunkX+i;
			requiredChunks[numRequired][1] = myPos.chunkY+j;
			numRequired++;
		}
	}
	struct chunk *runner = firstChunk;
	while (runner) {
		x = runner->myX;
		y = runner->myY;
		for (i = 0; i < numRequired; i++) {
			if (requiredChunks[i][0] == x && requiredChunks[i][1] == y) {
				requiredChunks[i][0] = requiredChunks[--numRequired][0];
				requiredChunks[i][1] = requiredChunks[numRequired][1];
				break;
			}
		}
		runner = runner->next;
	}

	for (i = 0; i < numRequired; i++)
		generateChunk(requiredChunks[i][0], requiredChunks[i][1]);

	SDL_Rect r = {.w = BLOCKSIZE, .h = BLOCKSIZE};
	int baseX;
	uint32_t color;
	runner = firstChunk;
	sgeClearScreen();
	sgeLock(screen);

	while (runner) {
		r.y = (runner->myY - myPos.chunkY) * BLOCKSIZE * CHUNKSIZE - BLOCKSIZE * myPos.y / PRECISION + 250;
		baseX = (runner->myX - myPos.chunkX) * BLOCKSIZE * CHUNKSIZE - BLOCKSIZE * myPos.x / PRECISION + 250;
		color = (runner->myX + runner->myY) % 2 ? 0xFFFF0000 : 0xFF0000FF;
		for (i = 0; i < CHUNKSIZE; i++) {
			r.x = baseX;
			for (j = 0; j < CHUNKSIZE; j++) {
				if (runner->types[i][j])
					SDL_FillRect(screen, &r, color);
				r.x += BLOCKSIZE;
			}
			r.y += BLOCKSIZE;
		}
		runner = runner->next;
	}
	r.x = 249;
	r.y = 249;
	r.w = 3;
	r.h = 3;
	SDL_FillRect(screen, &r, (myPos.chunkX + myPos.chunkY) % 2 ? 0xFFFF8080 : 0xFF8080FF);

	sgeUnlock(screen);
	sgeFlip();
}

int run(int argc, char **argv)
{
	srandom(time(NULL));

	SGEGAMESTATEMANAGER *manager;
	SGEGAMESTATE *game_state;

	sgeInit(NOAUDIO, NOJOYSTICK);
	sgeOpenScreen("Terraria ... in ... SPAAAAAAACE!!!", 500, 500, 32, NOFULLSCREEN);

	game_state = sgeGameStateNew();
	game_state->onKeyDown = keyFunc;
	game_state->onRedraw = onRedraw;

	myPos.chunkX = 0;
	myPos.chunkY = 0;
	myPos.x = CHUNKSIZE / 2 * PRECISION + PRECISION / 2;
	myPos.y = CHUNKSIZE / 2 * PRECISION + PRECISION / 2;

	struct chunk mine = {.myX = 0, .myY = 0, .next = NULL, .types={
		{1,0,0,0,0,0,0,0,0,1},
		{0,0,0,0,0,0,0,0,0,0},
		{0,0,0,1,0,1,0,1,0,0},
		{0,0,1,0,0,1,0,0,0,0},
		{1,0,0,0,0,1,1,0,0,0},
		{1,1,1,0,0,0,1,1,0,0},
		{1,0,0,0,0,0,0,0,0,0},
		{0,0,0,1,1,1,1,0,0,1},
		{0,0,0,0,0,0,0,0,1,1},
		{0,1,1,1,1,1,1,1,1,0}}};
	firstChunk = &mine;

	manager = sgeGameStateManagerNew();
	sgeGameStateManagerChange(manager, game_state);
	sgeGameStateManagerRun(manager, 30);

	sgeCloseScreen();
	return 0;
}
