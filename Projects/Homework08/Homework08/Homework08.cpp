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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
using namespace std;

// Arcball
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

        if (xAxis) coord.x = (2 * (float)x - windowWidth) / windowWidth;
        if (yAxis) coord.y = -(2 * (float)y - windowHeight) / windowHeight;

        coord.x = glm::clamp(coord.x, -1.0f, 1.0f);
        coord.y = glm::clamp(coord.y, -1.0f, 1.0f);

        float length_squared = coord.x * coord.x + coord.y * coord.y;
        if (length_squared <= 1.0) coord.z = sqrt(1.0 - length_squared);
        else coord = glm::normalize(coord);

        return coord;
    }

    void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
        mouseEvent = (action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT);
    }

    void cursorCallback(GLFWwindow* window, double x, double y) {
        if (mouseEvent == 0) return;
        else if (mouseEvent == 1) {
            prevPos = toScreenCoord(x, y);
            mouseEvent = 2;
            return;
        }

        currPos = toScreenCoord(x, y);

        float dot = (float)glm::dot(prevPos, currPos);
        angle = acos(((1.0f < dot) ? 1.0f : dot));

        camAxis = glm::cross(prevPos, currPos);
    }

    glm::mat4 createRotationMatrix() {
        return glm::rotate(glm::degrees(angle) * rollSpeed, camAxis);
    }

};

// Cube
class Cube {
public:

    // vertex position array
    GLfloat cubeVertices[72] = { // initialized as size = 1 for each dimension, 72 elements
        .5f, .5f, .5f,  -.5f, .5f, .5f,  -.5f,-.5f, .5f,  .5f,-.5f, .5f, // v0,v1,v2,v3 (front)
        .5f, .5f, .5f,   .5f,-.5f, .5f,   .5f,-.5f,-.5f,  .5f, .5f,-.5f, // v0,v3,v4,v5 (right)
        .5f, .5f, .5f,   .5f, .5f,-.5f,  -.5f, .5f,-.5f, -.5f, .5f, .5f, // v0,v5,v6,v1 (top)
        -.5f, .5f, .5f,  -.5f, .5f,-.5f,  -.5f,-.5f,-.5f, -.5f,-.5f, .5f, // v1,v6,v7,v2 (left)
        -.5f,-.5f,-.5f,   .5f,-.5f,-.5f,   .5f,-.5f, .5f, -.5f,-.5f, .5f, // v7,v4,v3,v2 (bottom)
        .5f,-.5f,-.5f,  -.5f,-.5f,-.5f,  -.5f, .5f,-.5f,  .5f, .5f,-.5f  // v4,v7,v6,v5 (back)
    };

    // normal array
    GLfloat cubeNormals[72] = {  // 72 elements
        0, 0, 1,   0, 0, 1,   0, 0, 1,   0, 0, 1,  // v0,v1,v2,v3 (front)
        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,  // v0,v3,v4,v5 (right)
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,  // v0,v5,v6,v1 (top)
        -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  -1, 0, 0,  // v1,v6,v7,v2 (left)
        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0,  // v7,v4,v3,v2 (bottom)
        0, 0,-1,   0, 0,-1,   0, 0,-1,   0, 0,-1   // v4,v7,v6,v5 (back)
    };

    // colour array
    GLfloat cubeColors[96] = { // initialized as RGBA sollid color for each face, 96 elements
        1, 0, 0, 1,   1, 0, 0, 1,   1, 0, 0, 1,   1, 0, 0, 1, // v0,v1,v2,v3 (front)
        1, 1, 0, 1,   1, 1, 0, 1,   1, 1, 0, 1,   1, 1, 0, 1, // v0,v3,v4,v5 (right)
        0, 1, 0, 1,   0, 1, 0, 1,   0, 1, 0, 1,   0, 1, 0, 1, // v0,v5,v6,v1 (top)
        0, 1, 1, 1,   0, 1, 1, 1,   0, 1, 1, 1,   0, 1, 1, 1, // v1,v6,v7,v2 (left)
        0, 0, 1, 1,   0, 0, 1, 1,   0, 0, 1, 1,   0, 0, 1, 1, // v7,v4,v3,v2 (bottom)
        1, 0, 1, 1,   1, 0, 1, 1,   1, 0, 1, 1,   1, 0, 1, 1  // v4,v7,v6,v5 (back)
    };

    // texture coord array

    GLfloat cubeTexCoords[48] = { // 48 elements
        1, 1,   0, 1,   0, 0,   1, 0,               // v0,v1,v2,v3 (front)
        0, 1,   0, 0,   1, 0,   1, 1,               // v0,v3,v4,v5 (right)
        1, 0,   1, 1,   0, 1,   0, 0,               // v0,v5,v6,v1 (top)
        1, 1,   0, 1,   0, 0,   1, 0,               // v1,v6,v7,v2 (left)
        1, 1,   0, 1,   0, 0,   1, 0,               // v7,v4,v3,v2 (bottom)
        0, 0,   1, 0,   1, 1,   0, 1                // v4,v7,v6,v5 (back)
    };

    // index array for glDrawElements()
    // A cube requires 36 indices = 6 sides * 2 tris * 3 verts

    GLuint cubeIndices[36] = { // 36 elements
        0, 1, 2,   2, 3, 0,    // v0-v1-v2, v2-v3-v0 (front)
        4, 5, 6,   6, 7, 4,    // v0-v3-v4, v4-v5-v0 (right)
        8, 9, 10,  10, 11, 8,    // v0-v5-v6, v6-v1-v0 (top)
        12,13,14,  14,15,12,    // v1-v6-v7, v7-v2-v1 (left)
        16,17,18,  18,19,16,    // v7-v4-v3, v3-v2-v7 (bottom)
        20,21,22,  22,23,20     // v4-v7-v6, v6-v5-v4 (back)
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int vSize = sizeof(cubeVertices);
    unsigned int nSize = sizeof(cubeNormals);
    unsigned int cSize = sizeof(cubeColors);
    unsigned int tSize = sizeof(cubeTexCoords);

    Cube() {
        initBuffers();
    };

    Cube(float sx, float sy, float sz) {
        scale2(sx, sy, sz);
        initBuffers();
    };

    Cube(float dx, float dy, float dz, float s) {
        scale(s);
        translate(dx, dy, dz);
        initBuffers();
    };

    Cube(float dx, float dy, float dz, float sx, float sy, float sz) {
        scale2(sx, sy, sz);
        translate(dx, dy, dz);
        initBuffers();
    };

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);

        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize + nSize + cSize + tSize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, cubeVertices);                  // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, nSize, cubeNormals);               // copy norms after verts
        glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize, cSize, cubeColors);          // copy cols after norms
        glBufferSubData(GL_ARRAY_BUFFER, vSize + nSize + cSize, tSize, cubeTexCoords); // copy texs after cols

        // copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);

        // attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vSize); // normal attrib
        glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(vSize + nSize)); //color attrib
        glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vSize + nSize + cSize)); // tex coord

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    void draw(Shader* shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

    void translate(float dx, float dy, float dz) {
        for (int i = 0; i < 72; i++) {
            if (i % 3 == 0) cubeVertices[i] += dx;
            else if (i % 3 == 1) cubeVertices[i] += dy;
            else if (i % 3 == 2) cubeVertices[i] += dz;
        }
    };

    void scale(float s) {
        for (int i = 0; i < 72; i++)
            cubeVertices[i] *= s;
    };

    void scale2(float sx, float sy, float sz) {
        for (int i = 0; i < 72; i++) {
            if (i % 3 == 0) cubeVertices[i] *= sx;
            else if (i % 3 == 1) cubeVertices[i] *= sy;
            else if (i % 3 == 2) cubeVertices[i] *= sz;
        }
    }
};

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
Shader *smoothShader = NULL;
Shader *lampShader = NULL;
unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;

// quadpyra and modeling variables
QuadPyra *quadpyra;
glm::mat4 projection, view, model;

// for smoothing mode
bool smoothing = false;

// for lighting
Cube *lamp;
glm::vec3 lightSize(0.1f, 0.1f, 0.1f);
glm::vec3 lightPos(1.0f, 1.2f, 2.0f);
glm::vec3 lightColor(1.0f, 1.0f, 1.0f);
glm::vec3 objectColor(1.0f, 0.5f, 0.31f);
float ambientStrength = 0.1f;
float specularStrength = 0.5f;
float specularPower = 64.0f;

//camera position
glm::vec3 cameraPos(0.0f, 0.0f, 7.0f);

//for arcball
float arcballSpeed = 0.1f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool cammode = true;

//main
int main()
{
    mainWindow = glAllInit();
    
    //shader loading and compile (by calling the constructor)
    //smoothShader = new Shader("smooth_lighting.vs", "smooth_lighting.fs");
    globalShader = new Shader("basic_lighting.vs", "basic_lighting.fs");
    lampShader = new Shader("lamp.vs", "lamp.fs");
    
    //projection matrix
    projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

    //quadpyra 
    globalShader->use();
    globalShader->setMat4("projection", projection);
    globalShader->setVec3("objectColor", objectColor);
    globalShader->setVec3("lightColor", lightColor);
    globalShader->setVec3("lightPos", lightPos);
    globalShader->setVec3("viewPos", cameraPos);
    globalShader->setFloat("ambientStrength", ambientStrength);
    globalShader->setFloat("specularStrength", specularStrength);
    globalShader->setFloat("specularPower", specularPower);

    //lamp
    lampShader->use();
    lampShader->setMat4("projection", projection);

    lamp = new Cube();
    quadpyra = new QuadPyra();
    
    //render loop
    while (!glfwWindowShouldClose(mainWindow)) {
        render();
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}

//window set
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
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework08", NULL, NULL);
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

//render
void render() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = view * camArcBall.createRotationMatrix();

    // quadpyra object
    globalShader->use();
    globalShader->setMat4("view", view);
    globalShader->setVec3("viewPos", glm::inverse(glm::mat3(camArcBall.createRotationMatrix())) * cameraPos);
    model = glm::mat4(1.0f);
    model = model * modelArcBall.createRotationMatrix();
    globalShader->setMat4("model", model);
    quadpyra->isSmooth(smoothing);
    quadpyra->draw(globalShader);

    // lamp
    lampShader->use();
    lampShader->setMat4("view", view);
    model = glm::mat4(1.0f);
    model = glm::translate(model, lightPos);
    model = glm::scale(model, lightSize);
    lampShader->setMat4("model", model);
    lamp->draw(lampShader);
    
    glfwSwapBuffers(mainWindow);
}


void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
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
        modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        cammode = !cammode;
        if (cammode) {
            cout << "ARCBALL: camera rotation mode" << endl;
        }
        else {
            cout << "ARCBALL: model  rotation mode" << endl;
        }
    }
    else if (key == GLFW_KEY_S && action == GLFW_PRESS) {
        smoothing = !smoothing;
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (cammode)
        camArcBall.mouseButtonCallback(window, button, action, mods);
    else
        modelArcBall.mouseButtonCallback(window, button, action, mods);
}

void cursor_position_callback(GLFWwindow* window, double x, double y) {
    if (cammode)
        camArcBall.cursorCallback(window, x, y);
    else
        modelArcBall.cursorCallback(window, x, y);
}