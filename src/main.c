#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "algebra.h"
#include "shader_program.h"

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
    int last_time;
    double delta_time;
    float rotation;
    float pos_index;
} App;

typedef struct {
    Vector3f position;
    Vector3f color;
} Vertex;

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
    app.rotation += app.delta_time;
    float double_pi = 2.0f * M_PI;
    if (app.rotation > double_pi) {
        app.rotation -= double_pi;
    }
    app.pos_index += app.delta_time;
    if (app.pos_index > double_pi) {
        app.pos_index -= double_pi;
    }
    Matrix4f scale;
    float scale_amount = 0.2 + 0.5 * fabs(cos(app.pos_index));
    matrix4f_scale(&scale, scale_amount, scale_amount, scale_amount);
    Matrix4f rotation;
    matrix4f_rotation(&rotation, app.rotation, app.rotation,0);
    Matrix4f translation;
    matrix4f_translation(&translation, 0.5*cos(app.pos_index), 0.5*sin(2*app.pos_index), 4.0);
    Matrix4f perspective;
    float xfov = 45.0 * M_PI/180.0;
    float yfov = xfov;
    matrix4f_perspective(&perspective, xfov, yfov);
    Matrix4f transform;    
    matrix4f_multiply(&rotation, &scale, &transform);
    matrix4f_multiply(&translation, &transform, &transform);
    matrix4f_multiply(&perspective, &transform, &transform);

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

    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glDisableVertexAttribArray(0);    
    glDisableVertexAttribArray(1);    
    glDisableClientState(GL_VERTEX_ARRAY);
    glutPostRedisplay();
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT);
    int time = glutGet(GLUT_ELAPSED_TIME);
    app.delta_time = (double)(time-app.last_time)*0.001;
    app.last_time = time;
    render();
    glutSwapBuffers();
}

void keyboard_func(unsigned char key, int x, int y) {
    x = x;
    y = y;
    if (key == 27) {
        glutLeaveMainLoop();
        gui.window = 0;
    }
}

void reshape_func(int width, int height) {
    gui.width = width;
    gui.height = height;
    glViewport(0, 0, width, height);
}

void randomize_vertex_color(Vertex *v) {
    v->color.x = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
    v->color.y = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
    v->color.z = 0.2 + 0.8 * (float)rand() / (float)RAND_MAX;
}

void create_vbo() {

    GLuint *vbo = &gui.vbo;
    GLuint *vao = &gui.vao;
    GLuint *ibo = &gui.ibo;

    Vertex vertices[8];
    for (int i = 0; i < 8; i++) {
        randomize_vertex_color(&vertices[i]);
    }
    double size = 0.5;
    // Create coords for a square
    vector3f_set(&vertices[0].position, -size, size, -size);
    vector3f_set(&vertices[1].position, size, size, -size);
    vector3f_set(&vertices[2].position, size, -size, -size);
    vector3f_set(&vertices[3].position, -size, -size, -size);

    vector3f_set(&vertices[4].position, -size, size, size);
    vector3f_set(&vertices[5].position, size, size, size);
    vector3f_set(&vertices[6].position, size, -size, size);
    vector3f_set(&vertices[7].position, -size, -size, size);

    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Create indices for a cube
    unsigned int indices[] = {
        0,1,2, // front
        0,2,3,

        5,4,6, // back
        4,7,6,

        0,4,5, // top
        0,5,1,

        6,7,3,// bottom
        2,6,3,

        1,5,6, // right
        1,6,2,

        4,0,3, // left
        4,3,7
    };
    glGenBuffers(1, ibo);
    // TODO vertex array?
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void destroy_vbo() {
    glDeleteBuffers(1, &gui.vbo);
}

int main(int argc, char **argv) {
    if (!create_gui(&argc, argv)) {
        return 1;
    }
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    create_vbo();
    glutDisplayFunc(display_func); 
    glutKeyboardFunc(keyboard_func);
    glutReshapeFunc(reshape_func);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);    
    app.last_time = glutGet(GLUT_ELAPSED_TIME);

    glutMainLoop();
    destroy_vbo();
    destroy_gui();
}
