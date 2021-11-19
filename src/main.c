#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "camera.h"
#include "mesh.h"
#include "meshloader.h"
#include "texture.h"
#include "object.h"
#include "shadowmap.h"
#include "renderer.h"
#include "tilemap.h"

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

#define OBJECT_TYPE_SKULL 1
#define OBJECT_TYPE_STATIC 0


typedef struct {
    bool forward;
    bool backward;
    bool left;
    bool right;
} Movement;

typedef struct {
    int last_time;
    double delta_time;
    int frames;
    int fps;
    int frame_time;
} AppTime;

void create_vbos(Mesh **meshes, int mesh_count) {
    for (int i = 0; i < mesh_count; i++) {
        if (meshes[i] != NULL) {
            mesh_instantiate(meshes[i]);
        }
    }
}

/*Object *create_hud(Mesh **target_mesh, Texture *source_texture) {
    app.meshes[2] = mesh_quad();
    Object *obj = object_create(*target_mesh, source_texture);
    vector3f_set(&obj->transform.rotation, 0, 0, 0);
    obj->transform.scale = 0.25;
    vector3f_set(&obj->transform.position, -0.75, 0.75, 0);
    transform_rebuild(&obj->transform);
    return obj;
}*/

void tiles_to_objects(Tilemap *tilemap, Object **objects, int max_distance, int object_count, Camera *camera) {
    int object = 0;
    Vector3f center;
    vector3f_set(&center, camera->position.x/2.0, 0, camera->position.z/2.0);  
    Vector3f tile_pos;
    Vector3f diff;
    for (int row = 0; row < tilemap->rows; row++) {
        for (int col = 0; col < tilemap->cols; col++) {
            vector3f_set(&tile_pos, col, 0, row);
            vector3f_sub(&tile_pos, &center, &diff);
            if (vector3f_length(&diff) < max_distance) {                
                int tile = tilemap_get_tile_at(tilemap, row, col);
                if (tile > 0 && object < object_count) {
                    Transform *transform = &objects[object++]->transform;                
                    transform->scale = 1;
                    transform->position.x = col*2;
                    transform->position.y = 0;
                    transform->position.z = row*2;
                    transform_rebuild(transform);
                }
            }
        }
    }
}

void update_camera(Movement *movement, Camera *camera, float delta_time) {
    if (movement->forward) {
        camera_move(camera, false, delta_time);
    } else if (movement->backward) {
        camera_move(camera, true, delta_time);
    }
    if (movement->left) {
        camera_move_left(camera, delta_time);
    } else if (movement->right) {
        camera_move_right(camera, delta_time);
    }
}

void mouse_motion(Camera *camera, int x, int y) {
    int delta_x = x;
    int delta_y = y;

    float dx = (float)(delta_x)/320.0f;
    float dy = (float)(delta_y)/800.0f;

    camera_look(camera, dx, dy);
}

void handle_camera_keys(Movement *movement, unsigned char key, bool key_down) {
    switch (key) {
        case 'w':
            movement->forward = key_down;
            break;
        case 's':
            movement->backward = key_down;
            break;
        case 'a':
            movement->left = key_down;
            break;
        case 'd':
            movement->right = key_down;
    }
}

bool handle_events(Camera *camera, Movement *movement) {
    SDL_Event e;
    bool down;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                return false;
            } 
        }

        switch (e.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                down = e.type == SDL_KEYDOWN;
                handle_camera_keys(movement, (char)e.key.keysym.sym, down);
                break;
            case SDL_MOUSEMOTION:
                mouse_motion(camera, e.motion.xrel, e.motion.yrel);
                break;
        }
    }
    return true;
}

void update_time(AppTime *app_time) {
    int time = SDL_GetTicks();
    int diff = time-app_time->last_time;
    app_time->delta_time = (double)(diff)*0.001;
    app_time->last_time = time;
    app_time->frames++;
    app_time->frame_time += diff;
    if (app_time->frames > 100) {
        app_time->fps = app_time->frames * 1000 / app_time->frame_time;
        printf("Frame rate: %d fps\n", app_time->fps);
        app_time->frame_time = 0;
        app_time->frames = 0;
    }
}

bool sdl_init() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (flags != IMG_Init(flags)) {
        fprintf(stderr, "Failed to initialize IMG: %s\n", IMG_GetError());
        return false;
    }
    return true;
}

void sdl_quit() {
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    if (!sdl_init()) {
        return 1;
    }
    Renderer *renderer = renderer_create(90.0);
    if (renderer == NULL) {
        return 1;
    }
    Tilemap *tilemap = tilemap_create();

    AppTime app_time;
    Camera camera;
    Movement movement;
    memset(&movement, 0, sizeof(Movement));
    camera_reset(&camera);

    Mesh *cube_mesh = mesh_cube();
    mesh_instantiate(cube_mesh);
    Texture *cube_texture = texture_create("texture.jpg");
    int max_distance = 8;
    int number_of_objects = max_distance*max_distance;
    Object *objects[number_of_objects];
    for (int i = 0; i < number_of_objects; i++) {
        objects[i] = object_create(cube_mesh, cube_texture, OBJECT_TYPE_STATIC);
        if (objects[i] == NULL) {
            printf("Cube %d is null\n", i);
        }
    }

    SDL_SetRelativeMouseMode(true);
    SDL_GL_SetSwapInterval(0);    

    while (handle_events(&camera,&movement)) {        
        update_time(&app_time);
        tiles_to_objects(tilemap, objects, max_distance, number_of_objects, &camera);
        update_camera(&movement, &camera, app_time.delta_time);
        camera_transform_rebuild(&camera);
        renderer_set_camera(renderer, &camera.m, &camera.position);
        renderer_draw(renderer, objects, number_of_objects);
    }
    for (int i = 0; i < number_of_objects; i++) {
        if (objects[i] != NULL) {
            object_destroy(objects[i]);
        }
    }
    texture_destroy(cube_texture);
    mesh_destroy(cube_mesh);
    renderer_destroy(renderer);
    tilemap_destroy(tilemap);
    sdl_quit();
}
