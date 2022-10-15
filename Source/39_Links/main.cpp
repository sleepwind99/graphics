// 39_Links
//          : Mouse left button: arcball control for camera
//          : Keyboard 'r': to reset the arcball
//          : Keyboard 'space' : to start/stop/init the animation

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>
#include <cmath>

#include <shader.h>
#include <cube.h>
#include <arcball.h>
#include <keyframe.h>
#include "link.h"

using namespace std;

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void render();
void initLinks();
void drawLinks(Link *root, float t, glm::mat4 cmodel, Shader *shader);

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cube *cube;
Link *root;
glm::mat4 projection, view, model;

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );

// for lighting
glm::vec3 lightSize(0.2f, 0.2f, 0.2f);
glm::vec3 lightPos(20.0f, 20.0f, 20.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
float ambientStrenth = 0.5f;

// for camera
glm::vec3 cameraPos(0.0f, 5.0f, 20.0f);
glm::vec3 cameraAt(0.0f, 0.0f, 0.0f);

// for animation
enum RenderMode { INIT, ANIM };
RenderMode renderMode;                  // current rendering mode
float beginT;                           // animation beginning time (in sec)
float timeT;                            // current time (in sec)
const float animEndTime = 2.0f;         // ending time of animation (in sec)

//-----------------------------------------
// main
//-----------------------------------------
int main()
{
    mainWindow = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("basic_lighting.vs", "basic_lighting.fs");
    
    // lighting parameters to fragment shader
    globalShader->use();
    globalShader->setVec3("lightColor", lightColor);
    globalShader->setVec3("lightPos", lightPos);
    globalShader->setFloat("ambientStrenth", ambientStrenth);
    
    // projection matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->setMat4("projection", projection);
    
    // cube initialization
    cube = new Cube();
    
    // initialize animation data
    initLinks();
    timeT = 0.0f;
    renderMode = INIT;
    
    // render loop
    // -----------
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

void initLinks()
{
    //Link(string name, glm::vec3 color, bool isRoot, int nChild,
    //     glm::vec3 size,
    //     glm::vec3 offset,
    //     glm::vec3 trans1,
    //     glm::vec3 trans2,
    //     glm::vec3 rot1,       //angles in degree
    //     glm::vec3 rot2)       //angles in degree
    
    // root link: yellow
    root = new Link("ROOT", glm::vec3(1.0, 1.0, 0.0), true, 2,
                    glm::vec3(1.0, 5.0, 1.0),   // size
                    glm::vec3(0.0, 0.0, 0.0),   // offset
                    glm::vec3(0.0, 0.0, 0.0),   // trans1 w.r.t. origin (because root)
                    glm::vec3(0.0, 0.0, 0.0),   // trans2 w.r.t. origin (because root)
                    glm::vec3(0.0, 0.0, 0.0),   // no rotation
                    glm::vec3(0.0, 0.0, 0.0));  // no rotation
    
    // left upper arm: red
    root->child[0] = new Link("LEFT_ARM_UPPER", glm::vec3(1.0, 0.0, 0.0), false, 1,
                              glm::vec3(4.0, 1.0, 1.0),  // size
                              glm::vec3(2.0, 0.0, 0.0),  // offset
                              glm::vec3(0.0, 2.5, 0.0),  // trans1
                              glm::vec3(0.0, 2.5, 0.0),  // trans2
                              glm::vec3(0.0, 0.0, 0.0),  // rotation about parent
                              glm::vec3(0.0, 0.0, 60.0));
    
    // left low arm: orange
    root->child[0]->child[0] = new Link("LEFT_ARM_LOWER", glm::vec3(1.0, 0.5, 0.0), false, 0,
                                        glm::vec3(2.0, 1.0, 1.0),  // size
                                        glm::vec3(1.0, 0.0, 0.0),  // offset
                                        glm::vec3(4.0, 0.0, 0.0),
                                        glm::vec3(4.0, 0.0, 0.0),
                                        glm::vec3(0.0, 0.0, 0.0),
                                        glm::vec3(0.0, -30.0, -50.0));
    
    // right upper arm: green
    root->child[1] = new Link("RIGHT_ARM_UPPER", glm::vec3(0.0, 1.0, 0.0), false, 1,
                              glm::vec3(4.0, 1.0, 1.0),
                              glm::vec3(-2.0, 0.0, 0.0),
                              glm::vec3(0.0, 2.5, 0.0),  // trans1
                              glm::vec3(0.0, 2.5, 0.0),  // trans2
                              glm::vec3(0.0, 0.0, 0.0),  // rotation about parent
                              glm::vec3(0.0, 0.0, -60.0));
    
    // right lower arm: cyan
    root->child[1]->child[0] = new Link("RIGHT_ARM_LOWER", glm::vec3(0.0, 0.5, 1.0), false, 0,
                                        glm::vec3(2.0, 1.0, 1.0),
                                        glm::vec3(-1.0, 0.0, 0.0),  // offset
                                        glm::vec3(-4.0, 0.0, 0.0),
                                        glm::vec3(-4.0, 0.0, 0.0),
                                        glm::vec3(0.0, 0.0, 0.0),
                                        glm::vec3(0.0, 30.0, 50.0));
}

void drawLinks(Link *clink, float t, glm::mat4 cmodel, Shader *shader)
{
    
    if (t > 1.0) t = 1.0f;
    
    glm::mat4 thisMat = glm::mat4(1.0f);

    // accumulate the parent's transformation
    thisMat = thisMat * cmodel;
    
    // if root, interpolates the translation
    glm::vec3 ctrans = glm::mix(clink->trans1, clink->trans2, t);
    thisMat = glm::translate(thisMat, ctrans);
    
    // interpolates the rotation
    //glm::quat q = glm::slerp(clink->q1, clink->q2, t);
    glm::vec3 euler = glm::mix(clink->rot1, clink->rot2, t);
    glm::quat q = glm::quat(euler);
    
    glm::mat4 crot = q.operator glm::mat4x4();
    
    thisMat = thisMat * crot;

    // render the link
    shader->use();
    shader->setMat4("model", thisMat);
    clink->draw(shader);

    // recursively call the drawLinks for the children
    for (int i = 0; i < clink->nChild; i++) {
        drawLinks(clink->child[i], t, thisMat, shader);
    }
    
}


void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    if (renderMode == ANIM) {
        float cTime = (float)glfwGetTime(); // current time
        timeT = cTime - beginT;
    }
    
    // view matrix to fragment shader
    view = glm::lookAt(cameraPos, cameraAt, glm::vec3(0.0f, 1.0f, 0.0f));
    view = view * camArcBall.createRotationMatrix();
    globalShader->use();
    globalShader->setMat4("view", view);
    
    // links
    model = glm::mat4(1.0f);
    
    drawLinks(root, timeT/animEndTime, model, globalShader);
    
    // swap buffers
    glfwSwapBuffers(mainWindow);
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Keyframe Animation", NULL, NULL);
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
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
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


// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->use();
    globalShader->setMat4("projection", projection);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (renderMode == INIT) {
            renderMode = ANIM;
            beginT = glfwGetTime();
        }
        else {  // renderMode == ANIM
            renderMode = INIT;
            timeT = 0.0f;
        }
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    camArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    camArcBall.cursorCallback( window, x, y );
}
