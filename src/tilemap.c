#include <stdlib.h>
#include <string.h>

#include "tilemap.h"

char *static_tile_data[] = {
    "*****************************",
    "*           *               *",
    "*   *********   **********  *",
    "*               *        *  *",
    "*   ****                    *",
    "*   *           *        *  *",
    "*   *   *    *************  *",
    "*   *   *                *  *",
    "*       *                   *",
    "*****************************"
};

Tilemap *tilemap_create() {
    Tilemap *tilemap = calloc(1, sizeof(Tilemap));
    tilemap->rows = sizeof(static_tile_data)/sizeof(char*);
    for (int i = 0; i < tilemap->rows; i++) {
        if ((int)strlen(static_tile_data[i]) > tilemap->cols) {
            tilemap->cols = strlen(static_tile_data[i]);
        }
    }
    tilemap->tiles = calloc(tilemap->rows, sizeof(int*));
    int **tiles = tilemap->tiles;
    for (int row = 0; row < tilemap->rows; row++) {
        tiles[row] = calloc(tilemap->cols, sizeof(int));
        char *s = static_tile_data[row];
        for (int col = 0; col < tilemap->cols && col < (int)strlen(s); col++) {
            if (s[col] == '*') {
                tilemap->tiles[row][col] = 1;
            }
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
