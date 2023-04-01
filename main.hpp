#ifndef MAIN_H
#define MAIN_H

void processInput(GLFWwindow *window);

// we don't use the shader here anymore, instead, use model_loading.vs & model_loading.fs
const char *vertexShaderSource = 
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"out vec4 vertexColor;\n"
"void main()\n"
"{\n"
" gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragmentShaderSource =
"#version 330 core\n"
"out vec4 FragColor;\n"
//"uniform vec4 ourColor;"
"in vec4 vertexColor;\n"
"void main()\n"
"{\n"
" FragColor = vertexColor;\n"
"}\0";

typedef struct color {
    float r;
    float g;
    float b;
} color_t;

#endif