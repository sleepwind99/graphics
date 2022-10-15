// 33_ModelLoading
//     Mouse: left mouse dragging - arcball rotation
//            wheel - zooming
//     Keyboards:  r - reset camera and object position
//                 a - toggle camera/object rotations for arcball
//                 arrow left, right, up, down: panning object position

// Std. Includes
#include <string>

// GLEW
//#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

// GL includes
#include <shader.h>
#include <arcball.h>
#include <Model.h>

// GLM Mathemtics
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Globals
unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 600;
GLFWwindow *mainWindow = NULL;
glm::mat4 projection;
Shader *shader;

// For model
Model *ourModel;

// for arcball
float arcballSpeed = 0.2f;
static Arcball camArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true );
static Arcball modelArcBall(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
bool arcballCamRot = true;

// for camera
glm::vec3 cameraOrigPos(0.0f, 0.0f, 9.0f);
glm::vec3 cameraPos;
glm::vec3 modelPan(0.0f, 0.0f, 0.0f);

// Function prototypes

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
GLFWwindow *glAllInit();
void render();


int main( )
{
    mainWindow = glAllInit();

    // Create shader program object
    shader = new Shader( "res/shaders/modelLoading.vs", "res/shaders/modelLoading.frag" );
    
    // Load models
    ourModel = new Model((GLchar *)"res/models/cyborg/cyborg.obj" );
    //ourModel = new Model((GLchar *)"res/models/rock/rock.obj" );
    //ourModel = new Model((GLchar *)"res/models/nanosuit/nanosuit.obj" );
    //ourModel = new Model((GLchar *)"res/models/planet/planet.obj" );
    
    // Projection initialization
    projection = glm::perspective(glm::radians(45.0f),
                                  (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader->use();
    shader->setMat4("projection", projection);
    cameraPos = cameraOrigPos;
    
    // Game loop
    while( !glfwWindowShouldClose( mainWindow ) )
    {
        glfwPollEvents( );
        
        render();
        
        glfwSwapBuffers( mainWindow );
    }
    
    glfwTerminate( );
    return 0;
}

void render()
{
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    shader->use();
    
    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    view = view * camArcBall.createRotationMatrix();
    shader->setMat4("view", view);
    
    // Draw the loaded model
    glm::mat4 model(1.0);
    
    // Rotate model by arcball and panning
    model = glm::translate( model, modelPan);
    model = model * modelArcBall.createRotationMatrix();
    
    // It's a bit too big for our scene, so scale it down
    //model = glm::scale( model, glm::vec3( 0.2f, 0.2f, 0.2f ) );
    
    shader->setMat4("model", model);
    
    ourModel->Draw( shader );
}

GLFWwindow *glAllInit()
{
    // Init GLFW
    glfwInit( );
    // Set all the required options for GLFW
    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "Model Loading", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        exit(-1);
    }
    
    glfwMakeContextCurrent( window );
    
    // Set the required callback functions
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    // Set this to true so GLEW knows to use a modern approach to retrieving function pointers and extensions
    glewExperimental = GL_TRUE;
    // Initialize GLEW to setup the OpenGL Function pointers
    if ( GLEW_OK != glewInit( ) )
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }
    
    // Define the viewport dimensions
    glViewport( 0, 0, SCR_WIDTH, SCR_HEIGHT );
    
    // OpenGL options
    glClearColor( 0.05f, 0.05f, 0.05f, 1.0f );
    glEnable( GL_DEPTH_TEST );
    
    return(window);
}

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
        modelArcBall.init(SCR_WIDTH, SCR_HEIGHT, arcballSpeed, true, true);
        cameraPos = cameraOrigPos;
        modelPan[0] = modelPan[1] = modelPan[2] = 0.0f;
    }
    else if (key == GLFW_KEY_A && action == GLFW_PRESS) {
        arcballCamRot = !arcballCamRot;
        if (arcballCamRot) {
            cout << "ARCBALL: Camera rotation mode" << endl;
        }
        else {
            cout << "ARCBALL: Model  rotation mode" << endl;
        }
    }
    else if (key == GLFW_KEY_LEFT) {
        modelPan[0] -= 0.1;
    }
    else if (key == GLFW_KEY_RIGHT) {
        modelPan[0] += 0.1;
    }
    else if (key == GLFW_KEY_DOWN) {
        modelPan[1] -= 0.1;
    }
    else if (key == GLFW_KEY_UP) {
        modelPan[1] += 0.1;
    }
}

void mouse_button_callback(GLFWwindow *window, int button, int action, int mods) {
    if (arcballCamRot)
        camArcBall.mouseButtonCallback( window, button, action, mods );
    else
        modelArcBall.mouseButtonCallback( window, button, action, mods );
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    if (arcballCamRot)
        camArcBall.cursorCallback( window, x, y );
    else
        modelArcBall.cursorCallback( window, x, y );
}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    cameraPos[2] -= (yoffset * 0.5);
}
