#include "../glad/gl.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // in Joey's source code but not sure if it's needed
#include <glm/gtc/type_ptr.hpp> // same as above
#include <iostream>
#include <stdbool.h>
// #include "main.hpp" // vector shader & fragment shader
#include "shader.hpp"
#include "camera.hpp"
#include "model.hpp" 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
bool AABB(Model & A, Model & B);

/* Screen size */
const unsigned int SCR_WIDTH = 1000;
const unsigned int SCR_HEIGHT = 750;

/* camera */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

/* timing */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* model transformations */
glm::mat4 modelA, modelB, modelC;

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
    Model toriModel("/root/OpenGL/Projects/Objects/doughnut.obj");
    Model cubeModel("/root/OpenGL/Projects/Objects/pinkCube.obj");
    Model coneModel("/root/OpenGL/Projects/Objects/cone.obj");

    /* Set locations for models */
    glm::vec3 translation_A(0.5f, 0.5f, 0.5f);
    glm::vec3 translation_B(0.2f, 0.5f, 0.5f);
    glm::vec3 translation_C(-0.5f, -0.5f, -0.5f);

    GLuint queryObjects[3];
    glGenQueries(3, queryObjects);
    for(int i = 0; i< 3; i++)
        cout << queryObjects[i] << endl;

    bool firstTime = 1;
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
            toriModel.Draw(ourShader);
            /* Update min xyz & max xyz values according to transform and scale matices*/
            // if(firstTime)
            {
                // cout << "AminX_before " << toriModel.minX << endl;
                // cout << "AmaxX_before " << toriModel.maxX << endl;
                // cout << "AminY_before " << toriModel.minY << endl;
                // cout << "AmaxY_before " << toriModel.maxY << endl;
                // cout << "AminZ_before " << toriModel.minZ << endl;
                // cout << "AmaxZ_before " << toriModel.maxZ << endl;

                // glm::vec3 Amin, Amax;
                // glm::vec4 Amin_t, Amax_t;
                // Amin.x = toriModel.minX;
                // Amin.y = toriModel.minY;
                // Amin.z = toriModel.minZ;
                // Amax.x = toriModel.maxX;
                // Amax.y = toriModel.maxY;
                // Amax.z = toriModel.maxZ;
                // Amin_t = projection * view * model * glm::vec4(Amin, 1.0);
                // Amax_t = projection * view * model * glm::vec4(Amax, 1.0);
                // cout << "Amin_t: " << Amin_t[0] << " " << Amin_t[1] << " " << Amin_t[2] << " " << Amin_t[3] << endl;
                // toriModel.minX = Amin_t[0];
                // toriModel.minY = Amin_t[1];
                // toriModel.minZ = Amin_t[2];
                // toriModel.maxX = Amax_t[0];
                // toriModel.maxY = Amax_t[1];
                // toriModel.maxZ = Amax_t[2];

                // toriModel.minX = (toriModel.minX + model[3][0] * 0.5 * SCR_WIDTH) * model[0][0];
                // toriModel.maxX = (toriModel.maxX + model[3][0] * 0.5 * SCR_WIDTH) * model[0][0];
                // toriModel.minY = (toriModel.minY + model[3][1] * 0.5 * SCR_HEIGHT) * model[1][1];
                // toriModel.maxY = (toriModel.maxY + model[3][1] * 0.5 * SCR_HEIGHT) * model[1][1];
                // toriModel.minZ = (toriModel.minZ + model[3][2]) * model[2][2];
                // toriModel.maxZ = (toriModel.maxZ + model[3][2]) * model[2][2];
                // cout << "AminX_after " << toriModel.minX << endl;
                // cout << "AmaxX_after " << toriModel.maxX << endl;
                // cout << "AminY_after " << toriModel.minY << endl;
                // cout << "AmaxY_after " << toriModel.maxY << endl;
                // cout << "AminZ_after " << toriModel.minZ << endl;
                // cout << "AmaxZ_after " << toriModel.maxZ << endl;
                // cout << "------------" << endl;
            }
        }
        /* Render the 2nd loaded model */
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_B);
            model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            cubeModel.Draw(ourShader);
            /* Update min xyz & max xyz values according to transform and scale matices*/
            // if(firstTime)
            {
                // cout << "BminX_before " << cubeModel.minX << endl;
                // cout << "BmaxX_before " << cubeModel.maxX << endl;
                // cout << "BminY_before " << cubeModel.minY << endl;
                // cout << "BmaxY_before " << cubeModel.maxY << endl;
                // cout << "BminZ_before " << cubeModel.minZ << endl;
                // cout << "BmaxZ_before " << cubeModel.maxZ << endl;

                /* 1st try, only consider model matrix: seems wrong */
                // cubeModel.minX = (cubeModel.minX + model[3][0] * 0.5 * SCR_WIDTH) * model[0][0];
                // cubeModel.maxX = (cubeModel.maxX + model[3][0] * 0.5 * SCR_WIDTH) * model[0][0];
                // cubeModel.minY = (cubeModel.minY + model[3][1] * 0.5 * SCR_HEIGHT) * model[1][1];
                // cubeModel.maxY = (cubeModel.maxY + model[3][1] * 0.5 * SCR_HEIGHT) * model[1][1];
                // cubeModel.minZ = (cubeModel.minZ + model[3][2]) * model[2][2];
                // cubeModel.maxZ = (cubeModel.maxZ + model[3][2]) * model[2][2];

                /* 2nd try, consider 3 matrices: seems wrong...? */
                // glm::vec3 Bmin, Bmax;
                // glm::vec4 Bmin_t, Bmax_t;
                // Bmin.x = cubeModel.minX;
                // Bmin.y = cubeModel.minY;
                // Bmin.z = cubeModel.minZ;
                // Bmax.x = cubeModel.maxX;
                // Bmax.y = cubeModel.maxY;
                // Bmax.z = cubeModel.maxZ;
                // Bmin_t = projection * view * model * glm::vec4(Bmin, 1.0);
                // Bmax_t = projection * view * model * glm::vec4(Bmax, 1.0);
                // cout << "Bmin_t: " << Bmin_t[0] << " " << Bmin_t[1] << " " << Bmin_t[2] << " " << Bmin_t[3] << endl;
                // cubeModel.minX = Bmin_t[0];
                // cubeModel.minY = Bmin_t[1];
                // cubeModel.minZ = Bmin_t[2];
                // cubeModel.maxX = Bmax_t[0];
                // cubeModel.maxY = Bmax_t[1];
                // cubeModel.maxZ = Bmax_t[2];

                // cout << "BminX_after " << cubeModel.minX << endl;
                // cout << "BmaxX_after " << cubeModel.maxX << endl;
                // cout << "BminY_after " << cubeModel.minY << endl;
                // cout << "BmaxY_after " << cubeModel.maxY << endl;
                // cout << "BminZ_after " << cubeModel.minZ << endl;
                // cout << "BmaxZ_after " << cubeModel.maxZ << endl;
            }
        }
        /* Render the 3rd loaded model */
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation_C);
            model = glm::scale(model, glm::vec3(.2f, .2f, .2f));	// it's a bit too big for our scene, so scale it down
            ourShader.setMat4("model", model);
            coneModel.Draw(ourShader);
            /* Update min xyz & max xyz values according to transform and scale matices*/
            // if(firstTime)
            {
                // cout << "CminX_before " << coneModel.minX << endl;
                // cout << "CmaxX_before " << coneModel.maxX << endl;
                // cout << "CminY_before " << coneModel.minY << endl;
                // cout << "CmaxY_before " << coneModel.maxY << endl;
                // cout << "CminZ_before " << coneModel.minZ << endl;
                // cout << "CmaxZ_before " << coneModel.maxZ << endl;
                // coneModel.minX = (coneModel.minX + model[3][0] * 0.5 * SCR_WIDTH) * model[0][0];
                // coneModel.maxX = (coneModel.maxX + model[3][0] * 0.5 * SCR_WIDTH) * model[0][0];
                // coneModel.minY = (coneModel.minY + model[3][1] * 0.5 * SCR_HEIGHT) * model[1][1];
                // coneModel.maxY = (coneModel.maxY + model[3][1] * 0.5 * SCR_HEIGHT) * model[1][1];
                // coneModel.minZ = (coneModel.minZ + model[3][2]) * model[2][2];
                // coneModel.maxZ = (coneModel.maxZ + model[3][2]) * model[2][2];
                // cout << "CminX_after " << coneModel.minX << endl;
                // cout << "CmaxX_after " << coneModel.maxX << endl;
                // cout << "CminY_after " << coneModel.minY << endl;
                // cout << "CmaxY_after " << coneModel.maxY << endl;
                // cout << "CminZ_after " << coneModel.minZ << endl;
                // cout << "CmaxZ_after " << coneModel.maxZ << endl;
            }
        }

        firstTime = 0;

        bool t1 = 0, t2 = 0, t3 = 0;
        t1 = AABB(toriModel, cubeModel); // 1
        t2 = AABB(toriModel, coneModel); // 0
        t3 = AABB(cubeModel, coneModel); // 0
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
    collisionX = (A.MAXs_t[0] >= B.MINs_t[0]) && (B.MAXs_t[0] >= A.MINs_t[0]);
    collisionY = (A.MAXs_t[1] >= B.MINs_t[1]) && (B.MAXs_t[1] >= A.MINs_t[1]);
    collisionZ = (A.MAXs_t[2] >= B.MINs_t[2]) && (B.MAXs_t[2] >= A.MINs_t[2]);
    
    /* collision happens if 3 axes all overlap */
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