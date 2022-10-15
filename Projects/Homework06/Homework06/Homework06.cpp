// 20_Arcball
//      Keyboard: r: reset the arc ball to initial state
//                a: toggle camera rotation and model rotation mode
//      Mouse: left button: begin arc ball dragging
#include "quadpyra.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <shader.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/matrix_inverse.hpp>

class Arcball {
private:
    int windowWidth;
    int windowHeight;
    int mouseEvent;
    GLfloat rollSpeed;
    GLfloat angle;
    glm::vec3 prevPos;
    glm::vec3 currPos;
    glm::vec3 camAxis;

    bool xAxis;
    bool yAxis;

public:

    Arcball(int window_width, int window_height, GLfloat roll_speed, bool x_axis, bool y_axis)
    {
        init(window_width, window_height, roll_speed, x_axis, y_axis);
    }

    void init(int window_width, int window_height, GLfloat roll_speed, bool x_axis, bool y_axis)
    {
        this->windowWidth = window_width;
        this->windowHeight = window_height;

        this->mouseEvent = 0;
        this->rollSpeed = roll_speed;
        this->angle = 0.0f;
        this->camAxis = glm::vec3(0.0f, 0.0f, 5.0f);

        this->xAxis = x_axis;
        this->yAxis = y_axis;
    }

    glm::vec3 toScreenCoord(double x, double y) {
        glm::vec3 coord(0.0f);

        if (xAxis)
            coord.x = (2 * (float)x - windowWidth) / windowWidth;

        if (yAxis)
            coord.y = -(2 * (float)y - windowHeight) / windowHeight;

        /* Clamp it to border of the windows, comment these codes to allow rotation when cursor is not over window */
        coord.x = glm::clamp(coord.x, -1.0f, 1.0f);
        coord.y = glm::clamp(coord.y, -1.0f, 1.0f);

        float length_squared = coord.x * coord.x + coord.y * coord.y;
        if (length_squared <= 1.0)
            coord.z = sqrt(1.0 - length_squared);
        else
            coord = glm::normalize(coord);

        return coord;
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        mouseEvent = (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT);
    }

    void cursorCallback(GLFWwindow* window, double x, double y) {
        if (mouseEvent == 0)
            return;
        else if (mouseEvent == 1) {
            /* Start of trackball, remember the first position */
            prevPos = toScreenCoord(x, y);
            mouseEvent = 2;
            return;
        }

        /* Tracking the subsequent */
        currPos = toScreenCoord(x, y);

        /* Calculate the angle in radians, and clamp it between 0 and 90 degrees */
        //angle    = acos( std::min(1.0f, glm::dot(prevPos, currPos) ));
        float dot = (float)glm::dot(prevPos, currPos);
        angle = acos(((1.0f < dot) ? 1.0f : dot));

        /* Cross product to get the rotation axis */
        camAxis = glm::cross(prevPos, currPos);
    }

    glm::mat4 createRotationMatrix() {
        return glm::rotate(glm::degrees(angle) * rollSpeed, camAxis);
    }

};


using namespace std;

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void render();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
QuadPyra *quadpyra;
glm::mat4 projection, view, model;
float radius = 5.0f;
float camx, camz;

// for arcball
float arcballSpeed = 0.1f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool togle = true;


int main()
{
    mainWindow = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("global.vs", "global.fs");
    
    // projection matrix
    globalShader->use();
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->setMat4("projection", projection);
    
    // cube initialization
    quadpyra = new QuadPyra();

    cout << "ARCBALL: camera rotation mode" << endl;
    
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework06", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    
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
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (togle) {
        view = glm::lookAt(glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        view = view * camArcBall.createRotationMatrix();

        globalShader->use();
        globalShader->setMat4("view", view);
    }
    else {
        camx = sin(glfwGetTime()) * radius;
        camz = cos(glfwGetTime()) * radius;

        view = glm::lookAt(glm::vec3(camx, 3.0f, camz), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));

        globalShader->use();
        globalShader->setMat4("view", view);
    }
    
    // quadpyra
    model = glm::mat4(1.0f);
    globalShader->setMat4("model", model);
    quadpyra->draw(globalShader);
    
    glfwSwapBuffers(mainWindow);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height; 
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
        togle = true;
    }
    else if (key == GLFW_KEY_P && action == GLFW_PRESS) {
        togle = !togle;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (togle) camArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    if (togle) camArcBall.cursorCallback( window, x, y );
}
