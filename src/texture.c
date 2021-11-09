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
    Texture *texture = calloc(1, sizeof(Texture));
    texture->width = surface->w;
    texture->height = surface->h;
    glGenTextures(1, &texture->texture);
    texture->target = GL_TEXTURE_2D; // TODO configurable
    glBindTexture(texture->target, texture->texture);
    glTexImage2D(texture->target, 0, GL_RGB, texture->width, texture->height, 0, GL_RGB, GL_UNSIGNED_BYTE, surface->pixels);    
    glTexParameterf(texture->target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(texture->target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(texture->target, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(texture->target, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glBindTexture(texture->target, 0);
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
