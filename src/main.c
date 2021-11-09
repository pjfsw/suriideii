#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "algebra.h"
#include "camera.h"
#include "mesh.h"
#include "shader_program.h"
#include "transform.h"

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

#define VECTOR3F_NUMBER_OF_COMPONENTS 3

typedef struct {
    GLint transformation;
} ShaderVariables;

typedef struct {
    int width;
    int height;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint program;
    ShaderVariables variables;
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
    float rotation;
    float pos_index;
    double fov;
    float perspective_a;
    float perspective_b;
    Mesh *mesh;
    Transform transform;
    Camera camera;
    Movement movement;    
} App;

Gui gui;
App app;

void destroy_gui() {
    if (gui.window != NULL) {
        SDL_DestroyWindow(gui.window);
    }
    SDL_Quit();
}

void update_window_size() {
    glViewport(0, 0, gui.width, gui.height);

    float fov = app.fov * M_PI/180.0;
    float ar = (float)(gui.height) / (float)(gui.width);

    matrix4f_perspective(&gui.perspective, fov, ar, app.perspective_a, app.perspective_b);
}


bool create_gui(int *argc, char **argv) {
    (void)argc;
    (void)argv;
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
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
        destroy_gui();
        return false;
    }
    printf("GL Version %s\n", gl_version);

    printf("Loading shaders\n");
    if (!(gui.program = shader_program_build("shader.vs", "shader.fs"))) {
        destroy_gui();
        return false;
    }

    printf("Loading location\n");

    gui.variables.transformation = glGetUniformLocation(gui.program, "gTransformation");
    if (gui.variables.transformation < 0) {
        fprintf(stderr, "Failed to get gTransformation variable\n");
        destroy_gui();
        return false;
    }

    printf("Init done\n");
    return true;
}

void create_vbo(Mesh *mesh) {

    GLuint *vbo = &gui.vbo;
    GLuint *vao = &gui.vao;
    GLuint *ibo = &gui.ibo;

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);    
    glBufferData(GL_ARRAY_BUFFER, mesh->vertex_count*sizeof(Vertex), mesh->vertices, GL_STATIC_DRAW);

    glGenBuffers(1, ibo);
    // TODO vertex array?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->index_count*sizeof(int), mesh->indices, GL_STATIC_DRAW);
}

void destroy_vbo() {
    glDeleteBuffers(1, &gui.vbo);
}

void init_app() {
    app.mesh = mesh_cube();
    app.fov = 90;
    float near_z = 1;
    float far_z = 20;
    float z_range = near_z - far_z;
    app.perspective_a = (-far_z - near_z) / z_range;
    app.perspective_b = 2.0f * far_z * near_z / z_range;
    update_window_size();
    camera_reset(&app.camera);
    transform_reset(&app.transform);
}

void render() {
    Matrix4f transform;    
    matrix4f_multiply_target(&app.camera.m, &app.transform.m, &transform);
    matrix4f_multiply(&gui.perspective, &transform);

    glUniformMatrix4fv(gui.variables.transformation, 1, GL_TRUE, &transform.m[0][0]);
    glBindBuffer(GL_ARRAY_BUFFER, gui.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gui.ibo);
    glEnableClientState(GL_VERTEX_ARRAY);    
    // Position
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VECTOR3F_NUMBER_OF_COMPONENTS, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);

    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, VECTOR3F_NUMBER_OF_COMPONENTS, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(sizeof(Vector3f)));

    glDrawElements(GL_TRIANGLES, app.mesh->index_count, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableVertexAttribArray(1);    
    glDisableClientState(GL_VERTEX_ARRAY);
}

void update_state() {
    app.rotation += app.delta_time;
    float double_pi = 2.0f * M_PI;
    if (app.rotation > double_pi) {
        app.rotation -= double_pi;
    }
    app.pos_index += app.delta_time;
    if (app.pos_index > double_pi) {
        app.pos_index -= double_pi;
    }

    app.transform.scale = 1.0; // 0.9 + 0.2 * fabs(cos(app.pos_index));
    app.transform.rotation.x = app.rotation;
    app.transform.rotation.y = app.rotation;
    app.transform.rotation.z = app.rotation;

    app.transform.position.x = 0;//0.5*cos(app.pos_index);
    app.transform.position.y = 0;//0.5*sin(2*app.pos_index);
    app.transform.position.z = 4.0;

    transform_rebuild(&app.transform);

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
        switch (e.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                down = e.type == SDL_KEYDOWN;
                if (down && e.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
                    return false;
                }
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
    if (!create_gui(&argc, argv)) {
        return 1;
    }
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    init_app();
    create_vbo(app.mesh);
    SDL_SetRelativeMouseMode(true);
    SDL_GL_SetSwapInterval(1);    
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(gui.window);
    while (handle_events()) {
        update_state();
        update_time();
        glClear(GL_COLOR_BUFFER_BIT);
        render();
        SDL_GL_SwapWindow(gui.window);
    }
    destroy_vbo();
    mesh_destroy(app.mesh);
    destroy_gui();
}
