// 37_Grass
//      Mouse: Arcball manipulation
//      Keyboard: 'r' - reset arcball

#include <GL/glew.h> 
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>
#include <vector>

#include <shader.h>
#include <cube.h>
#include <plane.h>
#include <arcball.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

using namespace std;

// Function Prototypes
GLFWwindow *glAllInit(const char *title);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
unsigned int loadTexture(const char *path, bool vflip);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void render();

// Global variables
GLFWwindow *mainWindow = NULL;
Shader *shader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
Cube *cube;
Plane *plane;
glm::mat4 projection, view, model;

// for camera
glm::vec3 cameraOrigPos(0.0f, 0.0f, 6.0f);
glm::vec3 cameraPos;
glm::vec3 camTarget(0.0f, 0.0f, 0.0f);
glm::vec3 camUp(0.0f, 1.0f, 0.0f);

glm::vec3 modelPan(0.0f, 0.0f, 0.0f);
float modelPanStep = 0.2f;

// for arcball
float arcballSpeed = 0.1f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);

// for texture
unsigned int cubeTexture;
unsigned int floorTexture;
unsigned int transparentTexture;

// vegetation locations
vector<glm::vec3> vegetation;

int main()
{
    mainWindow = glAllInit("Grass");
    
    // shader loading and compile (by calling the constructor)
    shader = new Shader("3.1.blending.vs", "3.1.blending.fs");
    
    // projection matrix
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader->use();
    shader->setMat4("projection", projection);
    
    // set defaul camera position
    cameraPos = cameraOrigPos;
    
    // load textures
    // -------------
    cubeTexture = loadTexture("marble.jpg", false);
    floorTexture = loadTexture("metal.png", false);
    transparentTexture = loadTexture("grass.png", true);
    
    shader->use();
    shader->setInt("texture1", 0);
    
    // create a cube and a plane
    cube = new Cube();
    plane = new Plane();
    
    // for preventing flickering, discard very small edges in the texture
    plane->texCoords[0] = 0.1;
    plane->texCoords[1] = 0.9;
    plane->texCoords[2] = 0.1;
    plane->texCoords[3] = 0.1;
    plane->texCoords[4] = 0.9;
    plane->texCoords[5] = 0.1;
    plane->texCoords[6] = 0.9;
    plane->texCoords[7] = 0.9;
    plane->updateVBO();
    
    // setting vegetation locations
    vegetation.push_back(glm::vec3(-1.5f,  0.0f, -0.48f));
    vegetation.push_back(glm::vec3( 1.5f,  0.0f,  0.51f));
    vegetation.push_back(glm::vec3( 0.0f,  0.0f,  0.7f));
    vegetation.push_back(glm::vec3(-0.3f,  0.0f, -2.3f));
    vegetation.push_back(glm::vec3( 0.5f,  0.0f, -0.6f));
    
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

// render
void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // send view to shader
    
    view = glm::lookAt(cameraPos, camTarget, camUp);
    view = view * camArcBall.createRotationMatrix();
    shader->use();
    shader->setMat4("view", view);
    
    // drawing a floor
    
    glBindTexture(GL_TEXTURE_2D, floorTexture);
    
    model = glm::mat4(1.0);
    model = glm::translate(model, modelPan);
    model = glm::translate(model, glm::vec3(0.0f, -0.51f, 0.0f));
    model = glm::rotate(model, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::scale(model, glm::vec3(7.0f, 7.0f, 0.0f));
    shader->setMat4("model", model);
    plane->draw(shader);
    
    // drawing two cubes
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    
    model = glm::mat4(1.0);
    model = glm::translate(model, modelPan);
    model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
    shader->setMat4("model", model);
    cube->draw(shader);
    
    model = glm::mat4(1.0f);
    model = glm::translate(model, modelPan);
    model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
    shader->setMat4("model", model);
    cube->draw(shader);
    
    // vegetation
    glBindTexture(GL_TEXTURE_2D, transparentTexture);
    for (GLuint i = 0; i < vegetation.size(); i++)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, modelPan);
        model = glm::translate(model, vegetation[i]);
        shader->setMat4("model", model);
        plane->draw(shader);
    }
    
    glfwSwapBuffers(mainWindow);
}

// glAllInit();
GLFWwindow *glAllInit(const char *title)
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
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
    glfwSetScrollCallback(window, scroll_callback);
    
    // OpenGL states
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const * path, bool vflip)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    
    if (vflip) stbi_set_flip_vertically_on_load(true);
    
    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else {
            format = GL_RGBA;
        }
        
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }
    
    return textureID;
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
    shader->use();
    shader->setMat4("projection", projection);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        camArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
        cameraPos = cameraOrigPos;
        modelPan[0] = modelPan[1] = modelPan[2] = 0.0f;
    }
    else if (key == GLFW_KEY_LEFT) {
        modelPan[0] -= modelPanStep;
    }
    else if (key == GLFW_KEY_RIGHT) {
        modelPan[0] += modelPanStep;
    }
    else if (key == GLFW_KEY_DOWN) {
        modelPan[1] -= modelPanStep;
    }
    else if (key == GLFW_KEY_UP) {
        modelPan[1] += modelPanStep;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    camArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    camArcBall.cursorCallback( window, x, y );
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    cameraPos[2] -= (yoffset * 0.5);
}

