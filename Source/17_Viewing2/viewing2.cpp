// 17_Viewing2
// ESC to quit

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>
#include <cube.h>

using namespace std;

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void render();

// Global variables
GLFWwindow *window = NULL;
Shader *globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cube *cube[3];
glm::mat4 view, projection;
float speed0 = glm::radians(90.0f);
float speed1 = glm::radians(120.0f);
float speed2 = glm::radians(180.0f);

int main()
{
    window = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("5.2.coordinate_systems.vs", "5.2.coordinate_systems.fs");
    globalShader->use();
    
    // cube initialization
    for (int i = 0; i < 3; i++)
        cube[i] = new Cube();
    
    // viewing and projection matrices
    view          = glm::mat4(1.0f);
    projection    = glm::mat4(1.0f);
    view  = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    globalShader->setMat4("view", view);
    globalShader->setMat4("projection", projection);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        render();
        glfwPollEvents();
    }
    
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

GLFWwindow *glAllInit()
{
    GLFWwindow *window;
    
    // glfw: initialize and configure
    if (!glfwInit()) {
        printf("GLFW initialisation failed!");
        glfwTerminate();
        exit(-1);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    
    // glfw window creation
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Viewing 2", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // OpenGL states
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    
    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    
    return window;
}

void render() {

    float currentTime = glfwGetTime();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    globalShader->use();
    
    // cube[0]
    // modeling transformation of cube[0]
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, speed0 * currentTime, glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    globalShader->setMat4("model", model);
    cube[0]->draw(globalShader);
    
    // cube[1]
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(1.0f, 1.0f, -1.0f));
    model = glm::rotate(model, speed1 * currentTime, glm::vec3(1.0f, 1.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.5f, 0.5f, 0.5f));
    globalShader->setMat4("model", model);
    cube[1]->draw(globalShader);
    
    // cube[2]
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.5f, -0.5f, 0.5f));
    model = glm::rotate(model, speed2 * currentTime, glm::vec3(-0.2f, -2.0f, 0.5f));
    model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
    globalShader->setMat4("model", model);
    cube[2]->draw(globalShader);
    
    glfwSwapBuffers(window);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_HEIGHT = height;
    SCR_WIDTH = width; 
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}
