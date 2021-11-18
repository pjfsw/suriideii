#ifndef _TILEMAP_H
#define _TILEMAP_H

typedef struct _Tilemap Tilemap;

Tilemap *tilemap_create();

int tilemap_get_tile_at(Tilemap *tilemap, int row, int col);

void tilemap_destroy(Tilemap *tilemap);

#endif