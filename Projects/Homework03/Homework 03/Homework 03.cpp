
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

#include <shader.h> 

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mode);
void normalize_coordinate(double& x, double& y);

unsigned int SCR_WIDTH = 600;
unsigned int SCR_HEIGHT = 600;
double x_pos = 0, y_pos = 0;
bool togle = true;

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Homework03", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    glewExperimental = GL_TRUE;

    if (glewInit() != GLEW_OK)
    {
        printf("GLEW initialisation failed!");
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }

    Shader ourShader("3.2.shader.vs", "3.2.shader.fs");

    float vertices[] = {
        -0.1f, -0.1f,  
         0.1f, -0.1f, 
         0.1f,  0.1f,
        -0.1f,  0.1f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        //background color
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ourShader.use();
        ourShader.setVec2("posset", float(x_pos), float(y_pos));
        if (togle) ourShader.setVec4("outColor", 1.0f, 0.0f, 0.0f, 1.0f);
        else ourShader.setVec4("outColor", 0.0f, 1.0f, 0.0f, 1.0f);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    //delete buffer
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);

    //Terminate
    glfwTerminate();
    return 0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mode) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &x_pos, &y_pos);
        normalize_coordinate(x_pos, y_pos);
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (togle) togle = false;
        else togle = true;
    }
}

void normalize_coordinate(double &x, double &y) {
    x = x / SCR_WIDTH * 2 - 1;
    y = -1 *(y / SCR_HEIGHT * 2 - 1);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}
