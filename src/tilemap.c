#include <stdlib.h>

#include "tilemap.h"

struct _Tilemap {
    int rows;
    int cols;
    int **tiles;
};

Tilemap *tilemap_create() {
    Tilemap *tilemap = calloc(1, sizeof(Tilemap));
    tilemap->rows = 10;
    tilemap->cols = 10;
    tilemap->tiles = calloc(tilemap->rows, sizeof(int*));
    int **tiles = tilemap->tiles;
    for (int row = 0; row < tilemap->rows; row++) {
        tiles[row] = calloc(tilemap->cols, sizeof(int));
        if (row == 0 || row == tilemap->rows-1) {
            for (int col = 0; col < tilemap->cols; col++) {
                tiles[row][col] = 1;
            }
        } else {
            tiles[row][0] = 1;
            tiles[row][tilemap->cols-1] = 1;
        }
    }

    return tilemap;
}

int tilemap_get_tile_at(Tilemap *tilemap, int row, int col) {
    if (row < 0 || row >= tilemap->rows || col < 0 || col >= tilemap->cols) {
        return 0;
    }
    return tilemap->tiles[row][col];
}

void tilemap_destroy(Tilemap *tilemap) {
    if (tilemap == NULL) {
        free(tilemap);
    }
    for (int row = 0; row < tilemap->rows; row++) {
        free(tilemap->tiles[row]);
    }
    free(tilemap->tiles);
}
