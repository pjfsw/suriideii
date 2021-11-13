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
#include "shadowmap.h"
#include "transform.h"
#include "light.h"
#include "object.h"

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

typedef struct {
    GLint direction;
    GLint color;
    GLint ambient_intensity;
    GLint diffuse_intensity; 
    GLint specular_intensity;
    GLint specular_power;
} ShaderLight;

typedef struct {
    GLint camera;
    GLint perspective;
    GLint world;
    GLint sampler;
    GLint camera_pos;
    ShaderLight light;
    GLint shadowmap;
    GLint light_depth;
} ShaderVariables;

typedef struct {
    GLint world;
    GLint light_matrix;
} ShadowmapShaderVariables;

typedef struct {
    int width;
    int height;
    GLuint program;
    GLuint shadow_program;
    ShaderVariables variables;
    ShadowmapShaderVariables shadowmap_variables;
    Matrix4f perspective;
    SDL_Window *window;
    SDL_GLContext context;
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
    double fov;
    float perspective_a;
    float perspective_b;
    Camera camera;
    Movement movement;    
    float mesh_y;
    Mesh **meshes;
    int mesh_count;
    Object **objects;
    int object_count;
    Texture **textures;
    int texture_count;
    Shadowmap *shadowmap;
    Matrix4f light_matrix;
} App;

Gui gui;
App app;

void destroy_gui() {
    if (gui.window != NULL) {
        SDL_DestroyWindow(gui.window);
    }
    IMG_Quit();
    SDL_Quit();
}

void update_window_size() {
    glViewport(0, 0, gui.width, gui.height);

    float fov = app.fov * M_PI/180.0;
    float ar = (float)(gui.height) / (float)(gui.width);

    matrix4f_perspective(&gui.perspective, fov, ar, app.perspective_a, app.perspective_b);
}

bool init_shader_light(char *prefix, ShaderLight *light) {
    glUseProgram(gui.program);
    char var[100];
    strcpy(var, prefix);
    strcat(var, ".direction");
    light->direction = glGetUniformLocation(gui.program, var);
    if (light->direction < 0) {
        fprintf(
            stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var, prefix);
    strcat(var, ".color");
    light->color = glGetUniformLocation(gui.program, var);
    if (light->color < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".diffuse_intensity");
    light->diffuse_intensity = glGetUniformLocation(gui.program, var);
    if (light->diffuse_intensity < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".ambient_intensity");
    light->ambient_intensity = glGetUniformLocation(gui.program, var);
    if (light->ambient_intensity < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".specular_intensity");
    light->specular_intensity = glGetUniformLocation(gui.program, var);
    if (light->specular_intensity < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }
    strcpy(var,prefix);
    strcat(var, ".specular_power");
    light->specular_power = glGetUniformLocation(gui.program, var);
    if (light->specular_power < 0) {
        fprintf(stderr, "Failed to get %s variable from fragment shader\n", var);
        return false;
    }

    return true;
}

bool assign_uniform(GLuint shader_program, GLint *uniform, char *name) {
    glUseProgram(shader_program);

    *uniform = glGetUniformLocation(shader_program, name);
    if (*uniform < 0) {
        fprintf(stderr, "Failed to get %s variable from shader program\n", name);
        return false;
    }
    return true;
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
    if (!(gui.program = shader_program_build("shader.vs", "shader.fs")) ||
        !assign_uniform(gui.program, &gui.variables.camera, "gCamera") ||
        !assign_uniform(
            gui.program, &gui.variables.perspective, "gPerspective") ||
        !assign_uniform(gui.program, &gui.variables.world, "gWorld") ||
        !assign_uniform(gui.program, &gui.variables.sampler, "gSampler") ||
        !assign_uniform(gui.program, &gui.variables.camera_pos, "gCameraPos") ||
        !assign_uniform(gui.program, &gui.variables.shadowmap, "gShadowmap") ||
        !assign_uniform(gui.program, &gui.variables.light_depth, "gLightDepth")) {
        return false;
    }
    if (!(gui.shadow_program =
                shader_program_build("shadow.vs", "shadow.fs")) ||
        !assign_uniform(gui.shadow_program,
            &gui.shadowmap_variables.light_matrix, "gLightMatrix") ||
        !assign_uniform(
            gui.shadow_program, &gui.shadowmap_variables.world, "gWorld")) {
        return false;
    }

    if (!init_shader_light("gLight", &gui.variables.light)) {
        return false;
    }

    printf("Init done\n");
    return true;
}

void setup_light(Light *light, ShaderLight *sl) {
    glUniform3f(sl->direction, light->direction.x, light->direction.y,
        light->direction.z);
    glUniform3f(sl->color, light->color.x, light->color.y, light->color.z);
    glUniform1f(sl->ambient_intensity, light->ambient_intensity);
    glUniform1f(sl->diffuse_intensity, light->diffuse_intensity);
    glUniform1f(sl->specular_intensity, light->specular_intensity);
    glUniform1f(sl->specular_power, light->specular_power);
}

void init_lights() {
    Light light;
    vector3f_set(&light.color, 1, 1, 1);
    vector3f_set_and_normalize(&light.direction, 0, 0, 1);
    matrix4f_rotation(&app.light_matrix, 0,M_PI/4,0); 
    //vector3f_set_and_normalize(&light.direction, 1, -0.5, 1);
    matrix4f_multiply_vector(&app.light_matrix, &light.direction);
    vector3f_normalize(&light.direction);
    light.ambient_intensity = 0.4;
    light.diffuse_intensity = 0.4;
    light.specular_intensity = 0.3;
    light.specular_power = 32;
    setup_light(&light, &gui.variables.light);    
    glUniformMatrix4fv(gui.shadowmap_variables.light_matrix, 1, GL_TRUE, &app.light_matrix.m[0][0]);

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
    shadowmap_destroy(app.shadowmap);
}

bool init_app() {
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
    app.mesh_count = 2;
    app.meshes = calloc(app.mesh_count, sizeof(Mesh*));
    app.meshes[0] = mesh_cube();
    app.meshes[1] = mesh_loader_load("skull.obj");
    if (app.meshes[1] == NULL) {
        app.meshes[1] = mesh_cube();
    }

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
    float near_z = 1;
    float far_z = 30;
    float z_range = near_z - far_z;
    app.perspective_a = (-far_z - near_z) / z_range;
    app.perspective_b = 2.0f * far_z * near_z / z_range;
    camera_reset(&app.camera);

    app.shadowmap = shadowmap_create(gui.width, gui.height);

    return true;
}

void render_object_shadowmap(Object *object) {
    MeshGL *gl = &object->mesh->gl;
    glUniformMatrix4fv(gui.shadowmap_variables.world, 1, GL_TRUE, &object->transform.m.m[0][0]);
    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);

    glDrawElements(GL_TRIANGLES, object->mesh->index_count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableClientState(GL_VERTEX_ARRAY);
}

void render_object(Object *object) {
    MeshGL *gl = &object->mesh->gl;

    glUniformMatrix4fv(
        gui.variables.light_depth, 1, GL_TRUE, &app.light_matrix.m[0][0]);

    glUniformMatrix4fv(gui.variables.world, 1, GL_TRUE, &object->transform.m.m[0][0]);
    glBindBuffer(GL_ARRAY_BUFFER, gl->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gl->ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    

    texture_bind(object->texture, GL_TEXTURE0);
    glUniform1i(gui.variables.sampler, 0);
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

void render_shadowmap() {
    glClear(GL_DEPTH_BUFFER_BIT);
    glUseProgram(gui.shadow_program);
    shadowmap_set_as_render_target(app.shadowmap);
    render_objects(render_object_shadowmap);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void render_scene() {
    glUseProgram(gui.program);
    shadowmap_bind(app.shadowmap, GL_TEXTURE1);
    glUniformMatrix4fv(gui.variables.camera, 1, GL_TRUE, &app.camera.m.m[0][0]);
    glUniform3f(gui.variables.camera_pos, app.camera.position.x, app.camera.position.y, app.camera.position.z);
    glUniform1i(gui.variables.shadowmap, 1);
    render_objects(render_object);
}

void render() {
    render_shadowmap();
    render_scene();
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
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    //glDisable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST); 
    init_lights();
    create_vbos(app.objects, app.object_count);
    SDL_SetRelativeMouseMode(true);
    SDL_GL_SetSwapInterval(1);    
    glUniformMatrix4fv(gui.variables.perspective, 1, GL_TRUE, &gui.perspective.m[0][0]);
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
