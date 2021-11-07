#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
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
    int window;
    int width;
    int height;
    GLuint vao;
    GLuint vbo;
    GLuint ibo;
    GLuint program;
    ShaderVariables variables;
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
    int frame_time;
    float rotation;
    float pos_index;
    double x_fov;
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
    if (gui.window > 0) {
        glutDestroyWindow(gui.window);
    }
}



bool create_gui(int *argc, char **argv) {
    glutInit(argc, argv);
    memset(&gui, 0, sizeof(Gui));
    int scrw = glutGet(GLUT_SCREEN_WIDTH);
    int scrh = glutGet(GLUT_SCREEN_HEIGHT);    
    printf("Screen width: %d\n", scrw);
    printf("Screen height: %d\n", scrh);
    gui.width = scrw * 5 / 6;
    gui.height = scrh * 5 / 6;
    if (gui.width < gui.height) {
        gui.height = gui.width;
    } else {
        gui.width = gui.height;
    }

    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE/* | GLUT_DEPTH*/);
    glutInitWindowSize(gui.width, gui.height);
    glutInitWindowPosition((scrw-gui.width)/2, (scrh-gui.height)/2);
    glutInitContextVersion(REQ_MAJOR_VERSION, REQ_MINOR_VERSION);
    glutInitContextProfile(GLUT_CORE_PROFILE);
    gui.window = glutCreateWindow("SURIIdeII");    

    GLenum res = glewInit();
    if (res != GLEW_OK)
    {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return false;
    }    

    const char *gl_version = (const char*)glGetString(GL_VERSION);
    if (gl_version == NULL || gui.window < 1) {
        fprintf(stderr, "Failed to create a window\n");
        fprintf(stderr, "OpenGL %d.%d+ required\n", REQ_MAJOR_VERSION, REQ_MINOR_VERSION);
        destroy_gui();
        return false;
    }
    printf("GL Version %s\n", gl_version);

    if (!(gui.program = shader_program_build("shader.vs", "shader.fs"))) {
        destroy_gui();
        return false;
    }

    gui.variables.transformation = glGetUniformLocation(gui.program, "gTransformation");
    if (gui.variables.transformation < 0) {
        fprintf(stderr, "Failed to get gTransformation variable\n");
        destroy_gui();
        return false;
    }

    return true;
}

void render() {
    Matrix4f perspective;
    float xfov = app.x_fov * M_PI/180.0;
    float yfov = app.x_fov * M_PI * (float)(gui.height) / ((float)(gui.width) * 180.0);

    matrix4f_perspective(&perspective, xfov, yfov, app.perspective_a, app.perspective_b);
    Matrix4f transform;    
    matrix4f_multiply_target(&app.camera.m, &app.transform.m, &transform);
    matrix4f_multiply(&perspective, &transform);

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
    glutPostRedisplay();
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

    app.transform.position.x = 0.5*cos(app.pos_index);
    app.transform.position.y = 0.5*sin(2*app.pos_index);
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

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT);
    int time = glutGet(GLUT_ELAPSED_TIME);
    int diff = time-app.last_time;
    app.delta_time = (double)(diff)*0.001;
    app.last_time = time;
    app.frames++;
    app.frame_time += diff;
    if (app.frames > 100) {
        printf("Frames %d, frame time %d ms, rate: %d fps\n", app.frames, app.frame_time, app.frames * 1000 / app.frame_time);
        app.frame_time = 0;
        app.frames = 0;
    }
    update_state();
    render();
    glutSwapBuffers();
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

void keyboard_func(unsigned char key, int x, int y) {
    (void)x;
    (void)y;
    if (key == 27) {
        glutLeaveMainLoop();
        gui.window = 0;
        return;
    }
    handle_camera_keys(key, true);
}

void keyboard_up_func(unsigned char key, int x, int y) {
    (void)x;
    (void)y;

    handle_camera_keys(key, false);
}



void reshape_func(int width, int height) {
    gui.width = width;
    gui.height = height;
    glViewport(0, 0, width, height);
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
    app.x_fov = 90;
    float near_z = 1;
    float far_z = 20;
    float z_range = near_z - far_z;
    app.perspective_a = (-far_z - near_z) / z_range;
    app.perspective_b = 2.0f * far_z * near_z / z_range;
    camera_reset(&app.camera);
    transform_reset(&app.transform);
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
    glutDisplayFunc(display_func); 
    glutKeyboardFunc(keyboard_func);
//    glutSpecialFunc(special_func);
    glutKeyboardUpFunc(keyboard_up_func);
    glutReshapeFunc(reshape_func);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);    
    app.last_time = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();    
    destroy_vbo();
    mesh_destroy(app.mesh);
    destroy_gui();
}
