// 34_Text
//    draw some texts using Text class
// 
#include <GL/glew.h>
// GLFW
#include <GLFW/glfw3.h>
// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Std. Includes
#include <iostream>

// local includes
#include <shader.h>
#include <text.h>
using namespace std;

// Globals
GLuint SCR_WIDTH = 600, SCR_HEIGHT = 600;
GLFWwindow *mainWindow = NULL;
Shader *textShader = NULL;
Text *text = NULL;
glm::mat4 projection, view, model;

// Function prototypes
void render();
GLFWwindow *glAllInit();


int main()
{
    
    mainWindow = glAllInit();

    textShader = new Shader("text.vs", "text.frag");
    text = new Text((char*)"fonts/arial.ttf", textShader, SCR_WIDTH, SCR_HEIGHT);
    
    // Game loop
    while (!glfwWindowShouldClose(mainWindow))
    {
        glfwPollEvents();
        
        render();
        
        glfwSwapBuffers(mainWindow);
    }
    
    glfwTerminate();
    return 0;
}

void render()
{
    // Clear the colorbuffer
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Drawing texts
    text->RenderText("This is sample text", 25.0f, 25.0f, 1.0f, glm::vec3(0.5, 0.8f, 0.2f));
    text->RenderText("Yonsei University", 370.0f, 570.0f, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));
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
    
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Text", NULL, NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    
    // Allow modern extension features
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        cout << "GLEW initialisation failed!" << endl;
        glfwDestroyWindow(window);
        glfwTerminate();
        exit(-1);
    }
    
    // Define the viewport dimensions
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    
    // Set OpenGL options
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    return(window);
}

