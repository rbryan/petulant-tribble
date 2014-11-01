#ifndef __SGETILE_H
#define __SGETILE_H

#ifdef __cplusplus
extern "C" {
#endif

	/** @file sgetile.h
	 * @brief Functions for managing tilemaps
	 */

	/*
	 * Copyright (c) 2007 Heiko Irrgang
	 *
	 * The license and distribution terms for this file may be
	 * found in the file COPYING in this distribution or at
	 * http://93-interactive.com/cms/products/software/sdl-game-engine/license/
	 */

	/**
	 * @defgroup sgeTile
	 * @brief Functions for managing tilemaps
	 *
	 * @{
	 */

#define TILELAYER 1
#define IMAGELAYER 2

	typedef struct {
		SGESPRITE *sprite;
        int type;
		int x;
		int y;
		int walkable;
		int jumpable;
	} SGETILE;

	typedef struct {
		int width;
		int height;
		int tileSize;
		SGETILE **tiles;
	} SGETILELAYER;

    typedef struct {
        int type;
    } SGETILELAYERINFO;

	typedef struct {
		int cameraX;
		int cameraY;
		SGEARRAY *layers; // can hold image and tile layers
		SGEARRAY *layerTypes; // specifies the type of the layer in layers
	} SGETILEMAP;

	SGETILEMAP *sgeTileMapNew(void);
	void sgeTileMapDestroy(SGETILEMAP *m);

	SGETILELAYER *sgeTileLayerNew(int width, int height, int tileSize);
	void sgeTileLayerDestroy(SGETILELAYER *l);

	SGETILE *sgeTileNew(SGESPRITE *s, int type, int x, int y, int isWalkable, int isJumpable);
	void sgeTileDestroy(SGETILE *t);

	void sgeTileMapAdd(SGETILEMAP *m, void *layer, int type);
    SGETILELAYER *sgeTileMapGetLayer(SGETILEMAP *m, int layer);
    int sgeTileMapGetLayerType(SGETILEMAP *m, int layer);
	void sgeTileMapDraw(SGETILEMAP *m, SDL_Surface *dest);
    void sgeTileMapSpriteToWorld( SGETILEMAP *m, SGEPOSITION *pos, int spriteX, int spriteY );
    void sgeTileMapWorldToSprite( SGETILEMAP *m, SGEPOSITION *pos, int worldX, int worldY );

	void sgeTileLayerAdd(SGETILELAYER *l, SGETILE *t);
	void sgeTileLayerDraw(SGETILELAYER *l, SGETILEMAP *m, SDL_Surface *dest);
    SGETILE *sgeTileLayerTileFromCoords( SGETILELAYER *l, int worldX, int worldY );

	// }@

#ifdef __cplusplus
}
#endif

#endif
