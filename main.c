/**
 *
 * This is a demonstration of tilemap rendering in sge2d
 *
 * */

#include <sge.h>

#define MOVESPEED 3
#define TILESIZE 32
#define PLAYERWIDTHHALF 12

// define our data that is passed to our redraw function
typedef struct {
	SGETILEMAP *tileMap;
	SGETILELAYER *tileLayer;
	int width;
	int height;
	int moveLeft;
	int moveRight;
	int quit;
	SGESPRITE *player;
	Uint32 skyColor;
} MainStateData;

// redraw the screen and update game logics, if any
void onRedraw(SGEGAMESTATE *state)
{
	SGEPOSITION pos;
	SGETILE *tile;
	MainStateData *data = (MainStateData*)state->data;
	int testPositionX=0;
	int isMoving=0;

	// ESC to exit
	if (data->quit) {
		sgeGameStateManagerQuit(state->manager);
		return;
	}

	sgeFillRect( screen, 0, 0, screen->w, screen->h, data->skyColor );
	sgeTileMapDraw( data->tileMap, screen );

	sgeSpriteDraw( data->player, screen );

	// finally display the screen
	sgeFlip();

	// get the current player position in world space
	sgeTileMapSpriteToWorld( data->tileMap, &pos, data->player->x, data->player->y );
	if ( data->moveRight ) {
		// update camera
		data->tileMap->cameraX+=MOVESPEED;
		// test a bit into walking direction
		testPositionX=pos.x+PLAYERWIDTHHALF;
		isMoving=1;
	} else if ( data->moveLeft ) {
		// update camera
		data->tileMap->cameraX-=MOVESPEED;
		// test a bit into walking direction
		testPositionX=pos.x-PLAYERWIDTHHALF;
		isMoving=1;
	}

	if (isMoving) {
		// get nearest tile in walking direction with a little offset to not let the player walk into the wall to far
		tile = sgeTileLayerTileFromCoords( data->tileLayer, testPositionX, pos.y-1 );

		if ( tile != NULL ) {
			printf( "%d/%d = %d\n", tile->x, tile->y, tile->type );
		}
	}

}

int onKeyUp(SGEGAMESTATE *state, SGEEVENT *event) {
	MainStateData *data = (MainStateData*)state->data;
	switch (event->key.keysym.sym) {
		case SDLK_ESCAPE:
			data->quit=1;
			return EVENT_HANDLED;
		case SDLK_LEFT:
			if (!data->moveRight) {
				sgeSpriteSetAnimBank( data->player, 0 );
			}
			data->moveLeft=0;
			return EVENT_HANDLED;
		case SDLK_RIGHT:
			if (!data->moveLeft) {
				sgeSpriteSetAnimBank( data->player, 2 );
			}
			data->moveRight=0;
			return EVENT_HANDLED;
		default:
			break;
	}
	return EVENT_UNHANDLED;
}

int onKeyDown(SGEGAMESTATE *state, SGEEVENT *event) {
	MainStateData *data = (MainStateData*)state->data;
	switch (event->key.keysym.sym) {
		case SDLK_LEFT:
			sgeSpriteSetAnimBank( data->player, 1 );
			data->moveLeft=1;
			data->moveRight=0;
			return EVENT_HANDLED;
		case SDLK_RIGHT:
			sgeSpriteSetAnimBank( data->player, 3 );
			data->moveLeft=0;
			data->moveRight=1;
			return EVENT_HANDLED;
		default:
			break;
	}
	return EVENT_UNHANDLED;
}

// this reads a very simple map format, a text
// file with numbers 0-9 representing a tile type,
// see data/map.txt
void prepareTilemap( MainStateData *data ) {
	// read our map, simple text format
	SGEFILE *f=sgeOpenFile( "data.d", "asdf" );
	char *map = sgeReadFile( f, "data/map.txt" );
	// allocate necessary size
	int size = strlen( map );
	int width = -1;
	int height = 0;
	int i, x, y;
	int n=0;
	int tile;
	int *intMap;
	SGESPRITE *wall;
	SGETILE *tileObject;

	data->skyColor = sgeMakeColor( screen, 0x4c, 0x92, 0xc8, 0xff );
	// we need a temporary storage, as we know width/height only
	// after reading and we need them for the tile layer
	sgeMalloc( intMap, int, size );

	// read our file
	for ( i = 0; i < size; i++ ) {
		if ( map[i] == '\n' ) {
			if ( width == -1 ) {
				width = i;
			}
			height++;
		}
		tile = ((int) map[i]) - 48;
		if ( ( tile >= 0 ) && ( tile <= 9 ) ) {
			intMap[n++]=tile;
		}
	}

	// store width and height
	data->width = width;
	data->height = height;

	// prepare our sprites
	wall = sgeSpriteNewFile( f, "data/wall.png" );

	sgeCloseFile(f);

	// finally create our tilemap
	data->tileMap = sgeTileMapNew();
	// for the demo there is only one layer
	data->tileLayer = sgeTileLayerNew( width, height, TILESIZE );

	for ( y = 0 ; y < height ; y++ ) {
		for ( x = 0; x < width; x++ ) {
			switch( intMap[ y*width+x ] ) {
				case 1: // basic wall
					tileObject=sgeTileNew( wall, intMap[y*width+x], x, y, NO, NO );
					sgeTileLayerAdd( data->tileLayer, tileObject );
					break;
				case 2: // wall you can jump through from below (isJumpable)
					tileObject=sgeTileNew( wall, intMap[y*width+x], x, y, NO, YES );
					sgeTileLayerAdd( data->tileLayer, tileObject );
					break;
				default:
					break;
			}
		}
	}

	sgeTileMapAdd( data->tileMap, data->tileLayer, TILELAYER );
}

void preparePlayer( MainStateData *data ) {
	SGEFILE *f=sgeOpenFile( "data.d", "asdf" );

	data->player=sgeSpriteNew();
	// anim bank 0 = standing left
	sgeSpriteAddFile(data->player,f,"data/animbanks_0014.png" );
	sgeSpriteAddAnimBank(data->player);
	// anim bank 1 = walking left
	sgeSpriteAddFileRange(data->player,f,"data/animbanks_%04d.png", 11, 20);
	sgeSpriteAddAnimBank(data->player);
	// anim bank 2 = standing right
	sgeSpriteAddFile(data->player,f,"data/animbanks_0034.png" );
	sgeSpriteAddAnimBank(data->player);
	// anim bank 3 = walking right
	sgeSpriteAddFileRange(data->player,f,"data/animbanks_%04d.png", 31, 40);
	// set standing right position as default
	sgeSpriteSetAnimBank(data->player, 2);
	data->player->x = (screen->w>>1);
	data->player->centerX = sgeSpriteWidth(data->player)>>1; // center sprite horizontally
	data->player->y = 9*TILESIZE;
	data->player->centerY = 55; // feet position an y = 55

	sgeCloseFile(f);
}

void initialize( MainStateData *data ) {
	data->quit = 0;
	data->moveRight = 0;
	data->moveLeft = 0;
}

// this is the main function, you don't use main(), as this is handled different
// on some platforms
int run(int argc, char *argv[]) {
	SGEGAMESTATEMANAGER *manager;
	SGEGAMESTATE *mainstate;
	MainStateData data;

	// initialize engine and set up resolution and depth
	sgeInit(NOAUDIO,NOJOYSTICK);
	sgeOpenScreen("Tilemap Demo",640,480,32,NOFULLSCREEN);
	sgeHideMouse();

	// add a new gamestate. you will usually have to add different gamestates
	// like 'main menu', 'game loop', 'load screen', etc.
	mainstate = sgeGameStateNew();
	mainstate->onRedraw = onRedraw;
	mainstate->onKeyDown = onKeyDown;
	mainstate->onKeyUp = onKeyUp;
	mainstate->data = &data;

	// now initialize tile map
	prepareTilemap( &data );
	// and the player sprite
	preparePlayer( &data );
	// and the rest of the variables
	initialize( &data );

	// now finally create the gamestate manager and change to the only state
	// we defined, which is the on_redraw function
	manager = sgeGameStateManagerNew();
	sgeGameStateManagerChange(manager, mainstate);

	// start the game running with 30 frames per seconds
	sgeGameStateManagerRun(manager, 30);

	// close the screen and quit
	sgeCloseScreen();
	return 0;
}
