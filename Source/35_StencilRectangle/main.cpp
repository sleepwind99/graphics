// 35_StencilRectangle
//     no interaction

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

#include <shader.h>
#include <cube.h>
#include <plane.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
 

using namespace std;

// Function Prototypes
GLFWwindow *glAllInit();
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
unsigned int loadTexture(string);
void render();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *globalShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cube *cube;
glm::mat4 projection, view, model;
glm::vec3 camPosition(0.0f, 0.0f, 3.5f);
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

// for texture
static unsigned int texture1, texture2, texture3, texture4; // texture ids

// for virtual rectangle
Plane *vRect;


int main()
{
    mainWindow = glAllInit();
    
    // shader loading and compile (by calling the constructor)
    globalShader = new Shader("global.vs", "global.fs");
    
    // load texture images
    texture1 = loadTexture("container.bmp");
    texture2 = loadTexture("bricks2.jpg");
    texture3 = loadTexture("wood.png");
    texture4 = loadTexture("sample.png");
    
    // create a cube
    cube = new Cube();
    
    // create a virtual rectangle
    vRect = new Plane();
    
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Stencil Rectangle", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    
    // OpenGL states
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
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

unsigned int loadTexture(string texFileName) {
    unsigned int texture;
    
    // Create texture ids.
    glGenTextures(1, &texture);
    
    // All upcomming GL_TEXTURE_2D operations now on "texture" object
    glBindTexture(GL_TEXTURE_2D, texture);
    
    // Set texture parameters for wrapping.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    // Set texture parameters for filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);   // vertical flip the texture
    unsigned char *image = stbi_load(texFileName.c_str(), &width, &height, &nrChannels, 0);
    if (!image) {
        cout << "texture " << texFileName << " loading error" << endl;
    }
    else cout << "texture " << texFileName << " loaded" << endl;
    
    GLenum format;
    if (nrChannels == 1) format = GL_RED;
    else if (nrChannels == 3) format = GL_RGB;
    else if (nrChannels == 4) format = GL_RGBA;
    else {
        cout << "loadTexture in main.cpp error ... illegal nrChannels: " << nrChannels << endl;
        exit(-1);
    }
    
    glBindTexture( GL_TEXTURE_2D, texture );
    glTexImage2D( GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image );
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return texture;
}

void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    globalShader->use();
    
    // draw the virtual rectangle
    
    glEnable(GL_STENCIL_TEST);
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    glStencilMask(0xFF);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    glDisable(GL_DEPTH_TEST);

    // projection and view: OpenGL default (for NDC)
    projection = glm::mat4(1.0f);
    globalShader->setMat4("projection", projection);
    view = glm::mat4(1.0f);
    globalShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(0.7f, 0.7f, 1.0f));
    globalShader->setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, texture4);
    vRect->draw(globalShader);
    
    // draw the three cubes
    
    glStencilFunc(GL_EQUAL, 1, 0xFF);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    
    // projection and view matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    globalShader->setMat4("projection", projection);
    view = glm::lookAt(camPosition, camTarget, camUp);
    globalShader->setMat4("view", view);
    
    // cube 1
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(0.4f, -0.2f, 0.0f));
    model = glm::rotate(model, glm::radians(-120.0f), glm::vec3(1.0f, -1.0f, 2.0f));
    globalShader->setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, texture1);
    cube->draw(globalShader);
    
    // cube 2
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.3f, -0.5f, 0.3f));
    model = glm::rotate(model, glm::radians(110.0f), glm::vec3(-2.0f, -3.0f, 4.0f));
    globalShader->setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, texture2);
    cube->draw(globalShader);
    
    // cube 3
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(-0.2f, 0.3f, 0.5f));
    model = glm::rotate(model, glm::radians(110.0f), glm::vec3(3.0f, 1.0f, -2.0f));
    globalShader->setMat4("model", model);
    glBindTexture(GL_TEXTURE_2D, texture3);
    cube->draw(globalShader);
    
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
}

