// 19_CameraFPS
// Keyboard: w, s, a, d: move camera to front, back, left, and right
//                  r: reset to initial camera parameters
// Mouse: left button: toggle the camera move mode
//              wheel: zoom in and out (by for change)

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>
#include <cube.h>

#ifndef M_PI
#define M_PI 3.141592
#endif

using namespace std;

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void render();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cube *cube;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  10.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;
float cameraSpeed = 100.0f;

// for mouse
bool validCursor = false;
float yaw   = -90.0f;    // yaw is initialized to -90.0 degrees since a yaw of 0.0 results
                         // in a direction vector pointing to the right
                         // so we initially rotate a bit to the left.
float pitch =  0.0f;
float lastX =  SCR_WIDTH / 2.0;
float lastY =  SCR_HEIGHT / 2.0;
float fov   =  45.0f;

int main()
{
    mainWindow = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("global.vs", "global.fs");
    
    // cube initialization
    cube = new Cube();
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(mainWindow)) {
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Camera FPS", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
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
    
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    globalShader->use();
    
    // pass projection matrix to shader (note that in this case it could change every frame)
    glm::mat4 projection = glm::perspective(glm::radians(fov),
                                            (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->setMat4("projection", projection);
    
    // camera/view transformation
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    globalShader->setMat4("view", view);
    
    // first cube
    glm::mat4 model = glm::mat4(1.0f);
    globalShader->setMat4("model", model);
    cube->draw(globalShader);
    
    // second cube
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-2.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (float)(M_PI/3.0f), glm::vec3(1.0f, 1.0f, 0.5f));
    globalShader->setMat4("model", model);
    cube->draw(globalShader);
    
    // third cube
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    model = glm::rotate(model, (float)(-M_PI/3.0f), glm::vec3(1.0f, 1.0f, 0.5f));
    globalShader->setMat4("model", model);
    cube->draw(globalShader);
    
    glfwSwapBuffers(mainWindow);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        // initialize all camera related parameters
        cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
        validCursor = false;
        yaw   = -90.0f;
        pitch =  0.0f;
        lastX =  SCR_WIDTH / 2.0;
        lastY =  SCR_HEIGHT / 2.0;
        fov   =  45.0f;
        glm::vec3 front;
        front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        front.y = sin(glm::radians(pitch));
        front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        cameraFront = glm::normalize(front);
    }
    else {
        float cameraDelta = cameraSpeed * deltaTime;
        
        if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT))
            cameraPos += cameraDelta * cameraFront;
        if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT))
            cameraPos -= cameraDelta * cameraFront;
        if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT))
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraDelta;
        if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT))
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraDelta;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        if (!validCursor) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            lastX = xpos;
            lastY = ypos;
            validCursor = true;
        }
        else validCursor = false;
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (!validCursor) return;
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    
    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw += xoffset;
    pitch += yoffset;
    
    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

