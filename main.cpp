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
void AABB(Model & A, Model & B);

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

/* Potencial colliding Set */
Model * PCS[3];
int numCollision;

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
    Shader ourShader("/root/OpenGL/Projects/model_loading.vs", "/root/OpenGL/Projects/model_loading.fs");
    
    /* load models */
    // Model toriModel("/root/OpenGL/Projects/Objects/doughnut.obj");
    // Model cubeModel("/root/OpenGL/Projects/Objects/pinkCube.obj");
    // Model coneModel("/root/OpenGL/Projects/Objects/cone.obj");

    /* Set locations for models */
    glm::vec3 translation_A(0.5f, 0.5f, 0.5f);
    glm::vec3 translation_B(0.2f, 0.5f, 0.5f);
    glm::vec3 translation_C(-0.5f, -0.5f, -0.5f);

    glm::mat4 modelA = glm::translate(glm::mat4(1.0f), translation_A);
    modelA = glm::scale(modelA, glm::vec3(.2f, .2f, .2f));	// scale down the model in the scene           
    Model toriModel("/root/OpenGL/Projects/Objects/doughnut.obj", modelA); // load model and pass in the model matrix
    // GLuint queryObjects[3];
    // GLuint pixelCount;
    // glGenQueries(3, queryObjects);
    // for(int i = 0; i< 3; i++)
    //     cout <<  "queryObjects"<< queryObjects[i] << endl;

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        numCollision = 0;

        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        /* input */
        processInput(window);

        /* Render here */
        glClearColor(.26f, .26f, .26f, .7f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Enable shader before setting uniforms */
        ourShader.use();

        /* view/projection transformations */
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        /* Render the 1st loaded model: moved out of loop for testing */
        
        // after loading the model, min/max are calculated, so we can do AABB
        // after AABB, colliding models are stored in PCS -> the occlusio query
        ourShader.setMat4("modelA", modelA);
        toriModel.Draw(ourShader);

        /* Render the 2nd loaded model */
        glm::mat4 modelB = glm::translate(glm::mat4(1.0f), translation_B);
        modelB = glm::scale(modelB, glm::vec3(.2f, .2f, .2f));    
        Model cubeModel("/root/OpenGL/Projects/Objects/pinkCube.obj", modelB);
        ourShader.setMat4("modelB", modelB);
        cubeModel.Draw(ourShader);

        /* Render the 3rd loaded model */
        glm::mat4 modelC = glm::translate(glm::mat4(1.0f), translation_C);
        modelC= glm::scale(modelC, glm::vec3(.2f, .2f, .2f)); 
        Model coneModel("/root/OpenGL/Projects/Objects/cone.obj", modelC);
        ourShader.setMat4("modelC", modelC);
        coneModel.Draw(ourShader);

        /* Do the AABB here?*/
        AABB(toriModel, cubeModel); // 1
        AABB(toriModel, coneModel); // 0
        AABB(cubeModel, coneModel); // 0

        // glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
        
        // for (int i = 0; i < 3 /* total number of objects */; i++)
        // {
        //     // glDepthMask(GL_FALSE); // goal: perform the depth test on all fragments and discard them accordingly, but not update the depth buffer
        //     glDepthFunc(GL_GEQUAL); // Passes if the incoming depth value is greater than or equal to the stored depth value.
            
        //     /* Decompose object into sub-objects here? */

        //     for (int j = 0; j < numCollision; j++)
        //     {
        //         /* Render sub-object j using occlusion query */
        //         glBeginQuery(GL_SAMPLES_PASSED, i);
        //         // !! render bounding box for sub-object j (use the min/max?)
        //         // like this: ?
        //         // glDrawArrays(GL_TRIANGLES, 0, 8); // each cuboid has 8 vertices
        //         // or, glDrawElements(GL_TRIANGLES, /*number of elements to be rendered*/, GL_UNSIGNED_INT, NULL);
        //         // glDrawElements(GL_TRIANGLES, 1, GL_UNSIGNED_INT, NULL);
        //         glEndQuery(GL_SAMPLES_PASSED);
                
        //         // Number of pixels passed: if it > 0, rander the objest
        //         int Passed = 0;
        //         glGetQueryObjectiv(i, GL_QUERY_RESULT, &Passed);

        //         // if(Passed > 0)
        //     }

        //     // glDepthMask(GL_TRUE);
        //     glDepthFunc(GL_LEQUAL); // Passes if the incoming depth value is less than or equal to the stored depth value.
            
        //     for (int j = 0; j < numCollision; j++)
        //     {
        //         // Render sub-object j
        //     }

        //     for (int j = 0; j < numCollision; j++)
        //     {
        //         // Test if sub-object j is visible repect to depth buffer.
        //         // If it's not fully visible, note it as fully visible ?!
                
        //     }
        // }

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void AABB(Model & A, Model & B)
{
    // either the numCollision wasn't updated, or all the models collide
    if(numCollision == 3) return;

    bool collisionX, collisionY, collisionZ;
    collisionX = (A.MAXs_t[0] >= B.MINs_t[0]) && (B.MAXs_t[0] >= A.MINs_t[0]);
    collisionY = (A.MAXs_t[1] >= B.MINs_t[1]) && (B.MAXs_t[1] >= A.MINs_t[1]);
    collisionZ = (A.MAXs_t[2] >= B.MINs_t[2]) && (B.MAXs_t[2] >= A.MINs_t[2]);

    /* collision happens if 3 axes all overlap */
    if(collisionX && collisionY && collisionZ)
    {
        if(numCollision == 0)
        {
            numCollision += 2;
            PCS[0] = &A;
            PCS[1] = &B;
        }else { /* only PCS[2] is empty */
            numCollision++;
            /* If the model has been in PCS, the other colliding model is stored at PCS[2] */
            if(PCS[0] == &A || PCS[1] == &A) PCS[2] = &B;
            if(PCS[0] == &B || PCS[1] == &B) PCS[2] = &A;
        }
    }
    // return collisionX && collisionY && collisionZ;
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