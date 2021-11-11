#include <SDL2/SDL_image.h>
#include "texture.h"

Texture *texture_create(char *file_name) {
    printf("Loading texture %s\n", file_name);
    SDL_Surface *surface = IMG_Load(file_name);
    if (surface == NULL) {
        fprintf(stderr, "Failed to load texture '%s': %s\n", file_name, IMG_GetError());
        return NULL;
    }
    printf("Loaded texture '%s': %dx%d (%d bytes per pixel)\n", file_name, surface->w, surface->h, surface->format->BytesPerPixel);
    if (surface->format->BytesPerPixel != 3) {
        fprintf(stderr, "Unsupported byte width: %d\n", surface->format->BytesPerPixel);
        free(surface);
        return NULL;
    }
    Texture *texture = calloc(1, sizeof(Texture));
    texture->width = surface->w;
    texture->height = surface->h;
    glGenTextures(1, &texture->texture);
    texture->target = GL_TEXTURE_2D; // TODO configurable
    glBindTexture(texture->target, texture->texture);
    
    // Flip rows because OpenGL starts at the bottom
    char *flipped_image = malloc(texture->width * texture->height * surface->format->BytesPerPixel);
    char *source = (char*)surface->pixels;
    int bytes_per_row = texture->width * surface->format->BytesPerPixel;
    for (int i = 0; i < texture->height; i++) {
        int src_offset = i * bytes_per_row;
        int dst_offset = (texture->height - i - 1) * bytes_per_row; 
        if (src_offset != dst_offset) {
            memcpy(&flipped_image[dst_offset], &source[src_offset], bytes_per_row);
        }
    }

    glTexImage2D(texture->target, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, flipped_image);    
    glTexParameterf(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(texture->target, 0);
    free(flipped_image);    
    SDL_FreeSurface(surface);
    return texture;
}

void texture_destroy(Texture *texture) {
    if (texture == NULL) {
        return;
    }
    free(texture);
}

void texture_bind(Texture *texture, GLenum texture_unit) {
    glActiveTexture(texture_unit);
    glBindTexture(texture->target, texture->texture);
}
