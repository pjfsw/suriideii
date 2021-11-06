#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define REQ_MAJOR_VERSION 4
#define REQ_MINOR_VERSION 2

typedef struct {
    float x;
    float y;
    float z;
} Vector3f;
#define VECTOR3F_NUMBER_OF_COMPONENTS 3

typedef struct {
    int window;
    int width;
    int height;
    GLuint vao;
    GLuint vbo;
} Gui;

Gui gui;

void destroy_gui() {
    if (gui.window > 0) {
        glutDestroyWindow(gui.window);
    }
}


bool create_shader(GLuint shader_program, int shader_type, char *shader) {
    GLuint shader_obj = glCreateShader(shader_type);
    if (shader_obj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", shader_type);
        return false;
    }

    const GLchar *p[1];
    p[0] = shader;
    GLint lengths[1];
    lengths[0] = strlen(shader);

    glShaderSource(shader_obj, 1, p, lengths);
    glCompileShader(shader_obj);
    
    GLint success;
    glGetShaderiv(shader_obj, GL_COMPILE_STATUS, &success);

    if (success == 0) {
        int log_size = 1024;
        GLchar info_log[log_size];
        glGetShaderInfoLog(shader_obj, log_size, NULL, info_log);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", shader_type, info_log);
        return false;
    }
    glAttachShader(shader_program, shader_obj);
    return true;
}

bool compile_shaders() {
    GLuint shader_program = glCreateProgram();
    if (shader_program == 0) {
        fprintf(stderr, "Failed to create shader program\n");
        return false;
    }
    char vertex_shader[] = 
        "#version 420 core\n"\
        "layout (location = 0) in vec3 position;\n"\
        "void main() {\n"\
        "  gl_Position = vec4(0.5 * position.x, 0.5 * position.y, position.z, 1.0);\n"\
        "}\n";
    char fragment_shader[] = 
        "#version 420 core\n"\
        "out vec4 frag_color;\n"\
        "void main() {\n"\
        "  frag_color = vec4(1.0, 0.0, 0.0, 0.0);\n"\
        "}\n";

    if (!create_shader(shader_program, GL_VERTEX_SHADER, vertex_shader)) {
        return false;
    }
    if (!create_shader(shader_program, GL_FRAGMENT_SHADER, fragment_shader)) {
        return false;
    }
    glLinkProgram(shader_program);

    GLint success;
    int log_size = 1024;
    GLchar info_log[log_size];
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Error linking shader program: '%s'\n", info_log);
        return false;
    }

    glValidateProgram(shader_program);
    glGetProgramiv(shader_program, GL_VALIDATE_STATUS, &success);
    if (success == 0) {
        glGetProgramInfoLog(shader_program, sizeof(info_log), NULL, info_log);
        fprintf(stderr, "Invalid shader program: '%s'\n", info_log);
        return false;
    }

    glUseProgram(shader_program);

    return true;
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

    if (!compile_shaders()) {
        destroy_gui();
        return false;
    }

    return true;
}

void render() {
    glBindBuffer(GL_ARRAY_BUFFER, gui.vbo);
    glEnableClientState(GL_VERTEX_ARRAY);    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, VECTOR3F_NUMBER_OF_COMPONENTS, GL_FLOAT, GL_FALSE, 0, NULL);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    glDisableVertexAttribArray(0);    
    glDisableClientState(GL_VERTEX_ARRAY);
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
    create_vbo();
    printf("VBO created\n");
    glutDisplayFunc(display_func); 
    glutKeyboardFunc(keyboard_func);
    glutReshapeFunc(reshape_func);
    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);    
    printf("Starting main loop\n");
    glutMainLoop();
    printf("Hello world\n");
    destroy_vbo();
    destroy_gui();
}
