#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "algebra.h"
#include "camera.h"
#include "mesh.h"
#include "meshloader.h"
#include "texture.h"
#include "shader_program.h"
#include "transform.h"
#include "light.h"
#include "lighting.h"
#include "object.h"
#include "uniform.h"
#include "shadowmap.h"

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

typedef struct {
    GLint sampler;
    GLint world;
    GLint ar;
} HudVariables;

typedef struct {
    GLint camera;
    GLint perspective;
    GLint world;
    GLint sampler;
    GLint camera_pos;
    GLint light_space;
    GLint shadowmap;
    GLint light_projection;
} RenderVariables;

typedef struct {
    GLint projection;
    GLint world;
    GLint camera;
} ShadowMapVariables;

typedef struct {
    int width;
    int height;
    GLuint render_program;
    GLuint hud_program;
    GLuint shadow_program;
    HudVariables hud_vars;
    ShadowMapVariables shadow_vars;
    RenderVariables render_vars;
    Matrix4f perspective;
    SDL_Window *window;
    SDL_GLContext context;
    Shadowmap *shadowmap;
    Matrix4f light_projection;
    Matrix4f light_view;
} Gui;

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
    float near_z;
    float far_z;
    double fov;
    float perspective_a;
    float perspective_b;
    Camera camera;
    Movement movement;    
    float mesh_y;
    Mesh **meshes;
    int mesh_count;
    Object **objects;
    Object *hud_object;
    int object_count;
    Texture **textures;
    int texture_count;
    Lighting *lighting;
    DirectionalLight *light;
    bool display_hud;
} App;

Gui gui;
App app;

void destroy_gui() {
    if (gui.window != NULL) {
        SDL_DestroyWindow(gui.window);
    }
    if (gui.shadowmap != NULL) {
        shadowmap_destroy(gui.shadowmap);
    }
    IMG_Quit();
    SDL_Quit();
}

void update_window_size() {
    glViewport(0, 0, gui.width, gui.height);

    float fov = app.fov * M_PI/180.0;
    float ar = (float)(gui.height) / (float)(gui.width);

    matrix4f_perspective(&gui.perspective, fov, ar, app.perspective_a, app.perspective_b);
    glUniformMatrix4fv(gui.render_vars.perspective, 1, GL_TRUE, &gui.perspective.m[0][0]);

    //matrix4f_perspective(&gui.light_projection, fov, ar, app.perspective_a, app.perspective_b);
    matrix4f_ortho(&gui.light_projection, -30, 30, -30, 30, -30, 30);

}

bool create_gui() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    int flags = IMG_INIT_JPG | IMG_INIT_PNG;
    if (flags != IMG_Init(flags)) {
        fprintf(stderr, "Failed to initialize IMG: %s\n", IMG_GetError());
        return false;
    }

    memset(&gui, 0, sizeof(Gui));

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, REQ_MAJOR_VERSION);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, REQ_MINOR_VERSION);
    SDL_GL_SetAttribute(
        SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    SDL_DisplayMode dm;
    SDL_GetCurrentDisplayMode(0, &dm);
    printf("Display reports size %dx%d\n", dm.w, dm.h);
    gui.width = dm.w;
    gui.height = dm.h;
    gui.window = SDL_CreateWindow("The Window Name", 0, 0, gui.width, gui.height,
        SDL_WINDOW_OPENGL |  SDL_WINDOW_FULLSCREEN_DESKTOP);

    if (gui.window == NULL) {
        fprintf(stderr, "Failed to create window: %s\n", SDL_GetError());
        destroy_gui();
        return false;
    }
    if ((gui.context = SDL_GL_CreateContext(gui.window)) == NULL) {
        fprintf(stderr, "Failed to create GL context: %s\n", SDL_GetError());
        destroy_gui();
        return false;
    }
    int major;
    int minor;
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, &major);
    SDL_GL_GetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, &minor);
    printf("OpenGL version %d.%d\n", major, minor);
    int r;
    int g;
    int b;
    SDL_GL_GetAttribute(SDL_GL_RED_SIZE, &r);
    SDL_GL_GetAttribute(SDL_GL_GREEN_SIZE, &g);
    SDL_GL_GetAttribute(SDL_GL_BLUE_SIZE, &b);
    printf("Color depth R=%d, G=%d, B=%d\n", r, g, b);

    glewExperimental = true;
    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "glew Init: '%s'\n", glewGetErrorString(res));
        return false;
    }    

    const char *gl_version = (const char*)glGetString(GL_VERSION);
    if (gl_version == NULL) {
        fprintf(stderr, "OpenGL %d.%d+ required\n", REQ_MAJOR_VERSION, REQ_MINOR_VERSION);
        return false;
    }
    printf("GL Version %s\n", gl_version);

    int texture_units;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texture_units);
    printf("Max texture units: %d\n", texture_units);

    printf("Loading shaders\n");
    if (!(gui.render_program = shader_program_build("render.vs", "render.fs")) ||
        !uniform_assign(gui.render_program, &gui.render_vars.camera, "gCamera") ||
        !uniform_assign(gui.render_program, &gui.render_vars.perspective, "gPerspective") ||
        !uniform_assign(gui.render_program, &gui.render_vars.world, "gWorld") ||
        !uniform_assign(gui.render_program, &gui.render_vars.sampler, "gSampler") ||
        !uniform_assign(gui.render_program, &gui.render_vars.camera_pos, "gCameraPos") ||
        !uniform_assign(gui.render_program, &gui.render_vars.light_space, "gLightSpace") ||
        !uniform_assign(gui.render_program, &gui.render_vars.shadowmap, "gShadowMap") ||
        !uniform_assign(gui.render_program, &gui.render_vars.light_projection, "gLightProjection")) {
        return false;
    }
    if (!(gui.hud_program = shader_program_build("hud.vs", "hud.fs")) ||
        !uniform_assign(gui.hud_program, &gui.hud_vars.sampler, "gSampler") ||
        !uniform_assign(gui.hud_program, &gui.hud_vars.world, "gWorld") ||
        !uniform_assign(gui.hud_program, &gui.hud_vars.ar, "gAspectRatio")) {
        return false;
    }

    if (!(gui.shadow_program = shader_program_build("shadow.vs", "shadow.fs")) ||
        !uniform_assign(gui.shadow_program, &gui.shadow_vars.world, "gWorld") ||
        !uniform_assign(gui.shadow_program, &gui.shadow_vars.camera, "gCamera") ||        
        !uniform_assign(gui.shadow_program, &gui.shadow_vars.projection, "gProjection")) {
            return false;
    }

    gui.shadowmap = shadowmap_create(gui.width, gui.height);


    printf("Init done\n");
    return true;
}

void init_lights() {
    lighting_set_default_reflection(app.lighting, 0.2, 0.7, 0.3, 32);

    Camera camera;
    camera_reset(&camera);
    camera_look(&camera, M_PI/2, 0);
    camera_move_unrestrained(&camera, 20);
    camera_transform_rebuild(&camera);
    camera_log(&camera, "Light camera: ");
    memcpy(&gui.light_view, &camera.m, sizeof(Matrix4f));
    //memcpy(&app.camera, &camera, sizeof(Camera));

    //vector3f_copy(&camera.position, &app.camera.position);
    Vector3f direction;    
    vector3f_multiply_scalar(-1, &camera.position, &direction);
    vector3f_add(&camera.target, &direction);
    vector3f_normalize(&direction);
    printf("Light direction: (%f,%f,%f)\n", direction.x, direction.y, direction.z);
    app.light = lighting_create_directional(app.lighting, direction.x, direction.y, direction.z, 1, 1, 1);
}

void create_vbos() {
    for (int i = 0; i < app.mesh_count; i++) {
        if (app.meshes[i] != NULL) {
            mesh_instantiate(app.meshes[i]);
        }
    }
}

void destroy_app() {
    if (app.objects != NULL) {
        for (int i = 0; i < app.object_count; i++) {
            if (app.objects[i] != NULL) {
                object_destroy(app.objects[i]);
            }
        }
        free(app.objects);
    }
    if (app.hud_object != NULL) {
        object_destroy(app.hud_object);
    }
    if (app.meshes != NULL) {
        for (int i = 0; i < app.mesh_count; i++) {
            if (app.meshes[i] != NULL) {
                mesh_destroy(app.meshes[i]);
            }
        }        
    }
    if (app.textures != NULL) {
        for (int i = 0; i < app.texture_count; i++) {
            if (app.textures[i] != NULL) {
                texture_destroy(app.textures[i]);
            }
        }
    }
    if (app.lighting != NULL) {
        lighting_destroy(app.lighting);
    }
}

Object *create_hud(Mesh **target_mesh, Texture *source_texture) {
    app.meshes[2] = mesh_quad();
    Object *obj = object_create(*target_mesh, source_texture);
    vector3f_set(&obj->transform.rotation, 0, 0, 0);
    obj->transform.scale = 0.25;
    vector3f_set(&obj->transform.position, -0.75, 0.75, 0);
    transform_rebuild(&obj->transform);
    return obj;
}

bool init_app() {
    app.lighting = lighting_create(gui.render_program);
    app.texture_count = 2;
    app.textures = calloc(app.texture_count, sizeof(Texture*));
    app.textures[0] = texture_create("texture.jpg");
    if (app.textures[0] == NULL) {
        return false;
    }

    app.textures[1] = texture_create("skull.jpg");
    if (app.textures[1] == NULL) {
        return false;
    }
    app.mesh_count = 3;
    app.meshes = calloc(app.mesh_count, sizeof(Mesh*));
    app.meshes[0] = mesh_cube();
    app.meshes[1] = mesh_loader_load("skull.obj");
    if (app.meshes[1] == NULL) {
        app.meshes[1] = mesh_cube();
    }
    app.hud_object = create_hud(&app.meshes[2], app.textures[0]);

    app.object_count = 3;
    app.objects = calloc(app.object_count, sizeof(Object*));
    app.objects[0] = object_create(app.meshes[0], app.textures[0]);
    for (int i = 1; i < app.object_count; i++) {
        app.objects[i] = object_create(app.meshes[1], app.textures[1]);
    }
    for (int i = 0; i < app.object_count; i++) {
        transform_reset(&app.objects[i]->transform);
        if (i == 0) {
            app.objects[i]->transform.scale = 7;
            app.objects[i]->transform.position.z = 4;
            app.objects[i]->transform.position.y = -8;
            app.objects[i]->transform.position.x = 0;
            transform_rebuild(&app.objects[i]->transform);
        } else {
            app.objects[i]->transform.rotation.y = (float)i * M_PI / 2;
            app.objects[i]->transform.position.z = i * 2 + 3;
            app.objects[i]->transform.scale = 0.2;  
        }
    }

    app.fov = 90;
    app.near_z = 1;
    app.far_z = 60;
    float z_range = app.near_z - app.far_z;
    app.perspective_a = (-app.far_z - app.near_z) / z_range;
    app.perspective_b = 2.0f * app.far_z * app.near_z / z_range;
    camera_reset(&app.camera);

    return true;
}

void render_mesh(Mesh *mesh) {
    MeshGL *gl = &mesh->gl;

    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    

    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    long size = sizeof(Vector3f);
    
    // Texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)size);
    size += sizeof(Vector2f);

    // Normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)size);

    glDrawElements(GL_TRIANGLES, mesh->index_count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableVertexAttribArray(1);    
    glDisableClientState(GL_VERTEX_ARRAY);
}


void render_object(Object *object) {
    texture_bind(object->texture, GL_TEXTURE0);
    glUniform1i(gui.render_vars.sampler, 0);
    glUniformMatrix4fv(gui.render_vars.world, 1, GL_TRUE, &object->transform.m.m[0][0]);
    render_mesh(object->mesh);
}

void render_object_for_shadowmap(Object *object) {
    glUniformMatrix4fv(gui.shadow_vars.world, 1, GL_TRUE, &object->transform.m.m[0][0]);
    render_mesh(object->mesh);
}

void render_hud(Object *object) {
    MeshGL *gl = &object->mesh->gl;

    glUniform1f(gui.hud_vars.ar, 1);
    glUniformMatrix4fv(gui.hud_vars.world, 1, GL_TRUE, &object->transform.m.m[0][0]);
    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    

    shadowmap_bind(gui.shadowmap, GL_TEXTURE0);
    glUniform1i(gui.hud_vars.sampler, 0);
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    long size = sizeof(Vector3f);
    
    // Texture
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)size);
    size += sizeof(Vector2f);

    glDrawElements(GL_TRIANGLES, object->mesh->index_count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableVertexAttribArray(1);    
    glDisableClientState(GL_VERTEX_ARRAY);

}

void render_objects(void (*object_renderer)(Object *object)) {
    for (int i = 0; i < app.object_count; i++) {
        object_renderer(app.objects[i]);
    }
}

void render_scene() {
    glUseProgram(gui.render_program);
    glUniformMatrix4fv(gui.render_vars.light_space, 1, GL_TRUE, &gui.light_view.m[0][0]);
    glUniformMatrix4fv(gui.render_vars.camera, 1, GL_TRUE, &app.camera.m.m[0][0]);
    glUniformMatrix4fv(gui.render_vars.light_projection, 1, GL_TRUE, &gui.light_projection.m[0][0]);
    glUniform3f(gui.render_vars.camera_pos, app.camera.position.x, app.camera.position.y, app.camera.position.z);
    shadowmap_bind(gui.shadowmap, GL_TEXTURE1);
    glUniform1i(gui.render_vars.shadowmap, 1);
    render_objects(render_object);
}

void render_debug() {
    glUseProgram(gui.hud_program);   
    glDisable(GL_DEPTH_TEST);
    render_hud(app.hud_object);
    glEnable(GL_DEPTH_TEST);
}

void render_shadows() {
    glUseProgram(gui.shadow_program);
    shadowmap_set_as_render_target(gui.shadowmap);

    glClear(GL_DEPTH_BUFFER_BIT);
    glUniformMatrix4fv(gui.shadow_vars.camera, 1, GL_TRUE, &gui.light_view.m[0][0]);
    glUniformMatrix4fv(gui.shadow_vars.projection, 1, GL_TRUE, &gui.light_projection.m[0][0]);

    render_objects(render_object_for_shadowmap);   
    glBindFramebuffer(GL_FRAMEBUFFER, 0);    
}

void render() {
    render_shadows();
    render_scene();
    if (app.display_hud) {
        render_debug();
    }
}

void update_object_state(Object *object) {
    object->transform.rotation.y += app.delta_time;
    float double_pi = 2.0f * M_PI;
    if (object->transform.rotation.y > double_pi) {
        object->transform.rotation.y -= double_pi;
    }

    object->transform.rotation.x = -M_PI/2;
    object->transform.rotation.z = 0;
    object->transform.position.y = app.mesh_y;

    transform_rebuild(&object->transform);
}

void update_state() {
    for (int i = 1; i < app.object_count; i++) {
        Object *object = app.objects[i];
        if (object != NULL) {
            object->transform.position.x = -3+(i-1)*6;
            update_object_state(object);
        }
    }

    if (app.movement.forward) {
        camera_move(&app.camera, false, app.delta_time);
    } else if (app.movement.backward) {
        camera_move(&app.camera, true, app.delta_time);
    }
    if (app.movement.left) {
        camera_move_left(&app.camera, app.delta_time);
    } else if (app.movement.right) {
        camera_move_right(&app.camera, app.delta_time);
    }
    camera_transform_rebuild(&app.camera);
}

void handle_camera_keys(unsigned char key, bool key_down) {
    switch (key) {
        case 'w':
            app.movement.forward = key_down;
            break;
        case 's':
            app.movement.backward = key_down;
            break;
        case 'a':
            app.movement.left = key_down;
            break;
        case 'd':
            app.movement.right = key_down;
    }
}

void mouse_motion(int x, int y) {
    int delta_x = x;
    int delta_y = y;

    float dx = (float)(delta_x)/320.0f;
    float dy = (float)(delta_y)/800.0f;

    camera_look(&app.camera, dx, dy);
}

void update_time() {
    int time = SDL_GetTicks();
    int diff = time-app.last_time;
    app.delta_time = (double)(diff)*0.001;
    app.last_time = time;
    app.frames++;
    app.frame_time += diff;
    if (app.frames > 100) {
        app.fps = app.frames * 1000 / app.frame_time;
        printf("Frame rate: %d fps\n", app.fps);
        app.frame_time = 0;
        app.frames = 0;
    }
}

bool handle_events() {
    SDL_Event e;
    bool down;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.scancode == SDL_SCANCODE_PAGEDOWN) {
                app.mesh_y -= 0.1;
                return true;
            } else if (e.key.keysym.scancode == SDL_SCANCODE_PAGEUP) {
                app.mesh_y += 0.1;
                return true;
            } else if (e.key.keysym.scancode == SDL_SCANCODE_HOME) {
                app.mesh_y = 0;
                return true;
            } else if (e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                return false;
            } else if (e.key.keysym.scancode == SDL_SCANCODE_TAB) {
                app.display_hud = !app.display_hud;
            } else if (e.key.keysym.scancode == SDL_SCANCODE_RETURN) {
                camera_log(&app.camera, "Main camera ");
            }
        }

        switch (e.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                down = e.type == SDL_KEYDOWN;
                handle_camera_keys((char)e.key.keysym.sym, down);
                break;
            case SDL_MOUSEMOTION:
                mouse_motion(e.motion.xrel, e.motion.yrel);
                break;
        }
    }
    return true;
}

int main(int argc, char **argv) {
    (void)argc; (void)argv;
    if (!create_gui() || !init_app()) {
        destroy_app();
        destroy_gui();
        return 1;
    }
    update_window_size();
    init_lights();
    create_vbos(app.objects, app.object_count);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_CONSTANT_ALPHA);    
    SDL_SetRelativeMouseMode(true);
    SDL_GL_SetSwapInterval(1);    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapWindow(gui.window);
    while (handle_events()) {
        update_state();
        update_time();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        render();
        SDL_GL_SwapWindow(gui.window);
    }
    destroy_app();
    destroy_gui();
}
