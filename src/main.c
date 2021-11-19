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
#include "object_pool.h"
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

void tiles_to_objects(Tilemap *tilemap, ObjectPool *cubes, int max_distance, Camera *camera) {
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
                if (tile > 0 && object < cubes->object_count) {
                    Transform *transform = &cubes->objects[object++]->transform;                
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

void floors_to_objects(Tilemap *tilemap, ObjectPool *floors, int max_distance, Camera *camera) {
    int object = 0;
    Vector3f center;
    float scale = 8;
    vector3f_set(&center, camera->position.x/scale, 0, camera->position.z/scale);  
    Vector3f tile_pos;
    Vector3f diff;
    for (int row = 0; row < tilemap->rows; row++) {
        for (int col = 0; col < tilemap->cols; col++) {
            vector3f_set(&tile_pos, col, 0, row);
            vector3f_sub(&tile_pos, &center, &diff);
            if (vector3f_length(&diff) < max_distance) {                
                if (object < floors->object_count) {
                    Transform *transform = &floors->objects[object++]->transform;                
                    transform->scale = scale;
                    transform->rotation.x = M_PI/2;
                    transform->position.x = col*scale;
                    transform->position.y = -1;
                    transform->position.z = row*scale;
                    transform_rebuild(transform);
                }
            }
        }
    }
}

void check_movement(Movement *movement, Camera *camera, float delta_time, Vector3f *move_vector) {
    double move = delta_time * 10.0;

    vector3f_zero(move_vector);

    if (movement->forward) {
        camera_move(camera, false, move, move_vector);
    } else if (movement->backward) {
        camera_move(camera, true, move, move_vector);
    }
    if (movement->left) {
        camera_move_left(camera, move, move_vector);
    } else if (movement->right) {
        camera_move_right(camera, move, move_vector);
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

    int max_distance = 8;
    int floor_distance = 2;

    Texture *floor_texture = texture_create("floor.png");
    Texture *cube_texture = texture_create("texture.jpg");

    Mesh *floor_mesh = mesh_quad(-0.5, -0.5, 1, 1, 4);
    mesh_instantiate(floor_mesh);
    ObjectPool *floor_pool = object_pool_create(floor_mesh, floor_texture, max_distance * max_distance, 0);

    Mesh *cube_mesh = mesh_cube();
    mesh_instantiate(cube_mesh);
    ObjectPool *cube_pool = object_pool_create(cube_mesh, cube_texture, max_distance * max_distance, 0);    

    int object_count = cube_pool->object_count + floor_pool->object_count;
    Object **objects = calloc(object_count, sizeof(Object));
    int n = 0;
    for (int i = 0 ; i < floor_pool->object_count; i++) {
        objects[n++] = floor_pool->objects[i];                
    }
    for (int i = 0 ; i < cube_pool->object_count; i++) {
        objects[n++] = cube_pool->objects[i];                
    }
    
    SDL_SetRelativeMouseMode(true);
    SDL_GL_SetSwapInterval(0);    

    camera.position.z = 1;
    while (handle_events(&camera,&movement)) {        
        update_time(&app_time);
        floors_to_objects(tilemap, floor_pool, floor_distance, &camera);
        tiles_to_objects(tilemap, cube_pool, max_distance, &camera);
        Vector3f move_vector;
        check_movement(&movement, &camera, app_time.delta_time, &move_vector);
        vector3f_add(&move_vector, &camera.position);
        camera_transform_rebuild(&camera);
        renderer_set_camera(renderer, &camera.m, &camera.position);
        renderer_draw(renderer, objects, object_count);
    }
    free(objects);
    object_pool_destroy(cube_pool, true);
    object_pool_destroy(floor_pool, true);
    renderer_destroy(renderer);
    tilemap_destroy(tilemap);
    sdl_quit();
}
