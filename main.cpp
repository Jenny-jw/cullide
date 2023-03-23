#include "../glad/gl.h"
#include <GLFW/glfw3.h>
#include <iostream>

const color_t dark = {0.258f, 0.114f, 0.024f};
const color_t milk = {1.0f, 0.98f, 0.957f};
const color_t mochi = {1.0f, 0.878f, 0.941f};

color_t triangle_color = dark;
bool color_changed = false;
bool key_pressed = false;

int main(void)
{
    GLFWwindow* window;
    unsigned int VBO; // vertex buffer object
    unsigned int VAO; // vertex array object
    unsigned int vertexShader;
    unsigned int fragmentShader;
    unsigned int shaderProgram;

    /* Initialize GLFW */
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        
    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "mochi mochi :3", NULL, NULL);
   
    if (!window)
    {
        std::cerr << "Failed to create a window" << std::endl;
        glfwTerminate();
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLAD */
    if (!gladLoaderLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.3f, -0.3f, 0.0f,
        0.0f, 0.5f, 0.0f
    };

    glGenBuffers(1, &VBO); // generate a buffer with ID 1
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glGenVertexArrays(1, &VAO);
    // 1. bind Vertex Array Object
    glBindVertexArray(VAO);
    // 2. copy our vertices array in a buffer for OpenGL to use
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // 3. then set the vertex attributes pointers
    glVertexAttribPointer(0/*location of Position*/, 3/*size of vertex*/, GL_FLOAT, GL_FALSE, 3 * sizeof(float)/*the space between consecutive vertex*/, (void*)0);
    glEnableVertexAttribArray(0);

    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // attach the shader source code to the shader object and compile the shader
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // check if compiling a shader program failed
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "Vertex shader compile failed\n" <<
        infoLog << std::endl;
    }

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "Fragment shader compile failed\n" <<
        infoLog << std::endl;
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    // check if linking a shader program failed
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "Shader link failed\n" <<
        infoLog << std::endl;
    }

    //delete the shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader); 

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        /* Render here */
        glClearColor(mochi.r, mochi.g, mochi.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);

        triangle_color = (color_changed) ? dark : milk;

        int vertexColorLocation = glGetUniformLocation(shaderProgram, "ourColor");
        glUniform4f(vertexColorLocation, triangle_color.r, triangle_color.g, triangle_color.b, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0/*starting index of the vertex array*/, 3);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window)
{
    
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    {
        key_pressed = true;
    }
    if(glfwGetKey(window, GLFW_KEY_X) == GLFW_RELEASE)
    {
        if(key_pressed)
            color_changed = !color_changed;
        key_pressed = false;
    }
}
