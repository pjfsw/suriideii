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
#include "physics.h"

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


void get_wanted_movement(Movement *movement, Camera *camera, float delta_time, Vector3f *wanted_movement) {
    float move = 10.0f * delta_time;
    vector3f_zero(wanted_movement);
    if (movement->forward) {
        camera_get_move_vector(camera, false, move, wanted_movement);
    } else if (movement->backward) {
        camera_get_move_vector(camera, true, move, wanted_movement);
    }
    if (movement->left) {
        camera_get_move_left_vector(camera, move, wanted_movement);
    } else if (movement->right) {
        camera_get_move_right_vector(camera, move, wanted_movement);
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

void init_floor(ObjectPool *floor, float room_size, float room_height) {
    (void)room_height;
    (void)room_size;
    for (int i = 0; i < floor->object_count; i++) {
        Transform *t = &floor->objects[i]->geometry.transform;
        vector3f_set(&t->position, 0, -room_height/2.0, 0);
        vector3f_x(&t->rotation, M_PI/2);
        t->scale = 1;
        transform_rebuild(t);
    }
}

void init_walls(ObjectPool *walls, float room_size, float room_height) {
    (void)room_height;
    float rotation[4] = {0,M_PI/2.0,M_PI,3.0*M_PI/2.0};
    float x[4] = {0,room_size/2.0,0, -room_size/2.0};
    float z[4] = {room_size/2.0,0,-room_size/2.0,0};
    for (int i = 0; i < walls->object_count; i++) {
        Transform *t = &walls->objects[i]->geometry.transform;
        vector3f_y(&t->rotation, rotation[i]);        
        vector3f_set(&t->position, x[i],0,z[i]);
        t->scale = 1;
        transform_rebuild(t);
    }
}

void init_cubes(ObjectPool *cubes, float room_height) {
    (void)room_height;
    for (int i = 0; i < cubes->object_count; i++) {
        Geometry *g = &cubes->objects[i]->geometry;
        vector3f_y(&g->transform.rotation, M_PI/3);
        vector3f_set(&g->transform.position, 0, -room_height/4, 7);
        g->transform.scale = 1;
        transform_rebuild(&g->transform);
        geometry_set_collider_sphere(g, 2);
    }
}

void check_collision(Geometry *geometry, Object **objects, int object_count, Vector3f *wanted_movement) {
    Vector3f new_pos;
    vector3f_copy(&geometry->transform.position, &new_pos);
    vector3f_add(wanted_movement, &new_pos);

    float movement_length = vector3f_length(wanted_movement);
    float max_length = movement_length;
    for (int i = 0; i < object_count; i++) {
        if (objects[i] != NULL) {
            float dist = physics_object_distance(
                &objects[i]->geometry.transform.position, &objects[i]->geometry.collider,
                &new_pos, &geometry->collider);
            if (dist < max_length) {
                max_length = dist;
            }
        }
    }
    if (max_length < 0.0001) {
        vector3f_zero(wanted_movement);
    } else if (max_length < movement_length) {
        vector3f_normalize(wanted_movement);
        vector3f_scale(max_length, wanted_movement);
    }   
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
    AppTime app_time;
    Camera camera;
    Movement movement;
    memset(&movement, 0, sizeof(Movement));
    camera_reset(&camera);

    Texture *floor_texture = texture_create("floor.png");
    Texture *wall_texture = texture_create("texture.jpg");
    Texture *cube_texture = texture_create("texture.jpg");

    float room_size = 20;
    float room_height = 4;
    float room_min = -0.4 * room_size;
    float room_max = 0.4 * room_size;

    Mesh *floor_mesh = mesh_quad(-room_size/2.0, -room_size/2.0, room_size, room_size, room_size/2, room_size/2);
    mesh_instantiate(floor_mesh);
    Mesh *wall_mesh = mesh_quad(-room_size/2.0,-room_height/2.0,room_size,room_height, room_size/4, room_height/4);
    mesh_instantiate(wall_mesh);
    Mesh *cube_mesh = mesh_cube();
    mesh_instantiate(cube_mesh);

    ObjectPool *floor_pool = object_pool_create(floor_mesh, floor_texture, 1, 0);
    ObjectPool *wall_pool = object_pool_create(wall_mesh, wall_texture, 4, 0);    
    ObjectPool *cube_pool = object_pool_create(cube_mesh, cube_texture, 1, 1);

    int object_count = wall_pool->object_count + floor_pool->object_count + cube_pool->object_count;
    Object **objects = calloc(object_count, sizeof(Object));
    int n = 0;
    for (int i = 0 ; i < floor_pool->object_count; i++) {
        objects[n++] = floor_pool->objects[i];                
    }
    for (int i = 0 ; i < wall_pool->object_count; i++) {
        objects[n++] = wall_pool->objects[i];                
    }
    for (int i = 0; i < cube_pool->object_count; i++) {
        objects[n++] = cube_pool->objects[i];
    }
    init_floor(floor_pool, room_size, room_height);
    init_walls(wall_pool, room_size, room_height);
    init_cubes(cube_pool, room_height);

    SDL_SetRelativeMouseMode(true);
    SDL_GL_SetSwapInterval(0);    

    camera.position.z = -4;

    Geometry player;
    player.transform.scale = 1;
    geometry_set_collider_sphere(&player, 1);

    while (handle_events(&camera,&movement)) {        
        update_time(&app_time);
        vector3f_copy(&camera.position, &player.transform.position);

        Vector3f wanted_movement;
        get_wanted_movement(&movement, &camera, app_time.delta_time, &wanted_movement);

        check_collision(&player, objects, object_count, &wanted_movement);
        camera_add_movement(&camera, &wanted_movement);

        if (camera.position.x < room_min) {
            camera.position.x = room_min;
        }
        if (camera.position.x > room_max) {
            camera.position.x = room_max;
        }
        if (camera.position.z < room_min) {
            camera.position.z = room_min;
        }
        if (camera.position.z > room_max) {
            camera.position.z = room_max;
        }
        camera_transform_rebuild(&camera);
        renderer_set_camera(renderer, &camera.m, &camera.position);
        renderer_draw(renderer, objects, object_count);
    }
    free(objects);
    object_pool_destroy(wall_pool, true);
    object_pool_destroy(floor_pool, true);
    object_pool_destroy(cube_pool, true);
    renderer_destroy(renderer);
    sdl_quit();
}
