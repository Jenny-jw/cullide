#include "../glad/gl.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // in Joey's source code but not sure if it's needed
#include <glm/gtc/type_ptr.hpp> // same as above
#include <iostream>
// #include "main.hpp" // vector shader & fragment shader
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp" 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
bool AABB(Model & A, Model & B);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main(void)
{
    GLFWwindow* window;

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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "mochi mochi :3", NULL, NULL);
   
    if (!window)
    {
        std::cerr << "Failed to create a window" << std::endl;
        glfwTerminate();
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    /* Initialize GLAD */
    if (!gladLoaderLoadGL())
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    /* configure global opengl state */
    glEnable(GL_DEPTH_TEST);

    /* build and compile shaders */
    Shader ourShader("model_loading.vs", "model_loading.fs");

    /* load models */
    Model cubeModel("/root/OpenGL/Projects/Objects/doughnut.obj");
    Model pinkCubeModel("/root/OpenGL/Projects/Objects/pinkCube.obj");
    Model coneModel("/root/OpenGL/Projects/Objects/cone.obj");

    /* Set locations for models */
    glm::vec3 translation_A(0.5f, 0.5f, 0.5f);
    glm::vec3 translation_B(0.2f, 0.5f, 0.5f);
    glm::vec3 translation_C(-0.5f, -0.5f, -0.5f);

    GLuint queryObjects[3];
    glGenQueries(3, queryObjects);
    for(int i = 0; i< 3; i++)
        cout << queryObjects[i] << endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* input */
        processInput(window);

        /* Render here */
        glClearColor(.5f, .5f, .5f, .5f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Enable shader before setting uniforms */
        ourShader.use();

        /* view/projection transformations */
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        /* Render the 1st loaded model */
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_A);
            model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            cubeModel.Draw(ourShader);
            /* Update min xyz & max xyz values according to transform and scale matices*/
            cubeModel.minX = (cubeModel.minX + model[0][3] * SCR_WIDTH) * model[0][0];
            cubeModel.maxX = (cubeModel.maxX + model[0][3] * SCR_WIDTH) * model[0][0];
            cubeModel.minY = (cubeModel.minY + model[1][3] * SCR_HEIGHT) * model[0][0];
            cubeModel.maxY = (cubeModel.maxY + model[1][3] * SCR_HEIGHT) * model[0][0];
            cubeModel.minZ = (cubeModel.minZ + model[2][3]) * model[0][0];
            cubeModel.maxZ = (cubeModel.maxZ + model[2][3]) * model[0][0];
        }
        /* Render the 2nd loaded model */
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_B);
            model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            pinkCubeModel.Draw(ourShader);
            /* Update min xyz & max xyz values according to transform and scale matices*/
            pinkCubeModel.minX = (pinkCubeModel.minX + model[0][3] * SCR_WIDTH) * model[0][0];
            pinkCubeModel.maxX = (pinkCubeModel.maxX + model[0][3] * SCR_WIDTH) * model[0][0];
            pinkCubeModel.minY = (pinkCubeModel.minY + model[1][3] * SCR_HEIGHT) * model[0][0];
            pinkCubeModel.maxY = (pinkCubeModel.maxY + model[1][3] * SCR_HEIGHT) * model[0][0];
            pinkCubeModel.minZ = (pinkCubeModel.minZ + model[2][3]) * model[0][0];
            pinkCubeModel.maxZ = (pinkCubeModel.maxZ + model[2][3]) * model[0][0];
        }
        /* Render the 3rd loaded model */
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_C);
            model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            coneModel.Draw(ourShader);
            /* Update min xyz & max xyz values according to transform and scale matices*/
            coneModel.minX = (coneModel.minX + model[0][3] * SCR_WIDTH) * model[0][0];
            coneModel.maxX = (coneModel.maxX + model[0][3] * SCR_WIDTH) * model[0][0];
            coneModel.minY = (coneModel.minY + model[1][3] * SCR_HEIGHT) * model[0][0];
            coneModel.maxY = (coneModel.maxY + model[1][3] * SCR_HEIGHT) * model[0][0];
            coneModel.minZ = (coneModel.minZ + model[2][3]) * model[0][0];
            coneModel.maxZ = (coneModel.maxZ + model[2][3]) * model[0][0];
        }
        bool t1, t2, t3;
        t1 = AABB(cubeModel, pinkCubeModel);
        t2 = AABB(cubeModel, coneModel);
        t3 = AABB(pinkCubeModel, coneModel);
        cout << t1 << t2 << t3 << endl;
    
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

bool AABB(Model & A, Model & B)
{
    bool collisionX, collisionY, collisionZ;
    collisionX = (A.maxX >= B.minX) && (B.maxX >= A.minX);
    collisionY = (A.maxY >= B.minY) && (B.maxY >= A.minY);
    collisionZ = (A.maxZ >= B.minZ) && (B.maxZ >= A.minZ);
    
    // collision happens if 3 axes all overlap
    return collisionX && collisionY && collisionZ;
}

/* Process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly */
void processInput(GLFWwindow *window)
{  
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

/* glfw: whenever the window size changed (by OS or user resize) this callback function executes */
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

/* glfw: whenever the mouse moves, this callback is called */
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

/* glfw: whenever the mouse scroll wheel scrolls, this callback is called */
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}