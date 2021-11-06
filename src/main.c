#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "shader_program.h"

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;
#define VECTOR3F_NUMBER_OF_COMPONENTS 3

typedef struct {
    GLint gScale;
} ShaderVariables;

typedef struct {
    int window;
    int width;
    int height;
    GLuint vao;
    GLuint vbo;
    GLuint program;
    ShaderVariables variables;
} Gui;

typedef struct {
    float scale;
    float delta;
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

    gui.variables.gScale = glGetUniformLocation(gui.program, "gScale");
    if (gui.variables.gScale < 0) {
        fprintf(stderr, "Failed to get gScale variable\n");
        destroy_gui();
        return false;
    }


    return true;
}

void render() {
    app.scale += app.delta;
    if (app.scale <= -1.0f || app.scale >= 1.0f) {
        app.delta *= -1.0f;
    }

    glUniform1f(gui.variables.gScale, app.scale);
    glBindBuffer(GL_ARRAY_BUFFER, gui.vbo);
    glEnableClientState(GL_VERTEX_ARRAY);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VECTOR3F_NUMBER_OF_COMPONENTS, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);    
    glDisableClientState(GL_VERTEX_ARRAY);
    glutPostRedisplay();
}

void display_func() {
    glClear(GL_COLOR_BUFFER_BIT);
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

void set_vector(Vector3f *v, float x, float y, float z) {
    v->x = x;
    v->y = y;
    v->z = z;
}

void create_vbo() {
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CW);
    //glCullFace(GL_FRONT);

    GLuint *vbo = &gui.vbo;
    GLuint *vao = &gui.vao;

    Vector3f vertices[3];
    set_vector(&vertices[0], -1, -1, 0);
    set_vector(&vertices[1], 0, 1, 0);
    set_vector(&vertices[2], 1, -1, 0);


    glGenVertexArrays(1, vao);
    glGenBuffers(1, vbo);
    glBindVertexArray(*vao);
    glBindBuffer(GL_ARRAY_BUFFER, *vbo);    
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
}

void destroy_vbo() {
    glDeleteBuffers(1, &gui.vbo);
}

int main(int argc, char **argv) {
    if (!create_gui(&argc, argv)) {
        return 1;
    }
    app.delta = 0.005f;
    create_vbo();
    glutDisplayFunc(display_func); 
    glutKeyboardFunc(keyboard_func);
    glutReshapeFunc(reshape_func);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);    
    glutMainLoop();
    destroy_vbo();
    destroy_gui();
}
