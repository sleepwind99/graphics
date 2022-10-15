// Std. Includes
#include <string>
#include <cstdlib>
#include <time.h>
#include <iostream>
#include <vector>

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

//Animation
#include <keyframe.h>

//Text
#include <text.h>

using std::cout;
using std::endl;
using std::string;
using std::copy;

#define XHITBOX 0.4f
#define YHITBOX 0.4f
#define MAXASTROID 15

unsigned int SCR_WIDTH = 800;
unsigned int SCR_HEIGHT = 1200;
GLFWwindow* mainWindow = NULL;
glm::mat4 projection;
Shader* shader;

class Star {
public:

    Star() {
        setColor();
        setVer();
        initBuffers();
    };

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize + cSize, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, Vertex);
        glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, Colors);

        // attribute position initialization
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)vSize);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    void setVer() {
        for (unsigned int i = 0; i < 24; i += 2) {
            float angle = (float) i * 15;
            Vertex[i] = (float) (0.5 * cos(glm::radians(angle))) * 1.5f;
            Vertex[i+1] = (float) 0.5 * sin(glm::radians(angle));
        }
    }

    void setColor() {
        for (int i = 0; i < 36; i++) {
            Colors[i] = 1.0f;
        }
    }

    void draw(Shader* shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 12);
        glBindVertexArray(0);
    };

    void translate(float dx, float dy, float s, glm::vec3 scolor) {
        for (int i = 0; i < 36; i += 3 ) {
            Colors[i] = scolor.x;
            Colors[i] = scolor.y;
            Colors[i] = scolor.z;
        }
        for (int i = 0; i < 24; i++) {
            Vertex[i] *= s;
            if (i % 2 == 0) Vertex[i] += dx;
            else Vertex[i] += dy;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, Vertex);
        glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, Colors);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    void move(float dy) {
        if (Vertex[7] < -1) {
            for (int i = 1; i < 24; i += 2) Vertex[i] += 2;
        }
        else {
            for (int i = 1; i < 24; i += 2) Vertex[i] -= dy;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void setSpeed(float s) {
        speed = s;
    }

    int getSpeed() {
        return speed;
    }

private:
    GLfloat Colors[36];
    GLfloat Vertex[24];
    unsigned int VAO, VBO;
    unsigned int vSize = sizeof(Vertex);
    unsigned int cSize = sizeof(Colors);
    float speed;
};
class Planet {

public:
    int speed;

    Planet() {
        setRandom();
        initBuffers();
        for(int i = 0; i < 10; i++) loadTexture(i);
    };

    void initBuffers() {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);

        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize + tSize, 0, GL_STATIC_DRAW);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, Vertex);
        glBufferSubData(GL_ARRAY_BUFFER, vSize, tSize, Texcoord);

        // attribute position initialization
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)vSize);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };

    void loadTexture(int i) {
        // Create texture ids.
        glGenTextures(1, &pTexture[i]);
        glBindTexture(GL_TEXTURE_2D, pTexture[i]);

        // Set texture parameters for wrapping.;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        
        imgName[19] = (char)i + 48;

        int width, height, nrChannels;
        unsigned char* image = stbi_load(imgName, &width, &height, &nrChannels, 0);
        if (!image) printf("texture %s loading error ... \n", imgName);
        else printf("texture %s loaded\n", imgName);

        GLenum format;
        if (nrChannels == 1) format = GL_RED;
        else if (nrChannels == 3) format = GL_RGB;
        else if (nrChannels == 4) format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, pTexture[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    void setVer() {
        for (unsigned int i = 0; i < 120; i += 2) {
            float angle = (float)i * 3;
            Vertex[i] = (float)0.6 * cos(glm::radians(angle));
            Texcoord[i] = (float)(Vertex[i] + 1.0f) / 2.0f;
            Vertex[i + 1] = (float)0.6 * sin(glm::radians(angle));
            Texcoord[i + 1] = (float)(Vertex[i + 1] + 1.0f) / 2.0f;
            Vertex[i] *= 1.5f;
        }
    }

    void translate(float dx, float dy, float s) {
        for (int i = 0; i < 120; i++) {
            Vertex[i] *= s;
            if (i % 2 == 0) Vertex[i] += dx;
            else Vertex[i] += dy;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, Vertex);
        glBufferSubData(GL_ARRAY_BUFFER, vSize, tSize, Texcoord);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    };

    void move(float dy) {
        if (Vertex[31] < -1) {
            setRandom();
            for (int i = 1; i < 120; i += 2) Vertex[i] += 1.0f;
        }
        else {
            for (int i = 1; i < 120; i += 2) Vertex[i] -= dy;
        }
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, Vertex);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void setRandom() {
        setVer();
        srand((unsigned int)time(NULL));
        float size = (float)((rand() % 5) + 1.0f) / 7;
        float posx = (float)((rand() % 1000) / 500.0f) - 1.0f;
        speed = (rand() % 3) + 1;
        preIdx = curIdx;
        while(preIdx == curIdx) curIdx = (rand() % 10);
        translate(posx, 1.0f, size);
    }

    void draw(Shader* shader) {
        shader->use();

        glBindTexture(GL_TEXTURE_2D, pTexture[curIdx]);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 60);
        glBindVertexArray(0);
    };

private:
    GLfloat Texcoord[120];
    GLfloat Vertex[120];
    unsigned int VAO, VBO;
    unsigned int vSize = sizeof(Vertex);
    unsigned int tSize = sizeof(Texcoord);
    char imgName[25] = "res/textures/planet0.jpg";
    unsigned int pTexture[10];
    int curIdx, preIdx = 0;
};
class backGround {
public:
    backGround(int n) {
        gamespeed = 1.0f;
        numStars = n;
        Stars = new Star[numStars];
        planet_ = new Planet();
        starShader = new Shader("res/shaders/star.vs", "res/shaders/star.fs");
        planetShader = new Shader("res/shaders/planet.vs", "res/shaders/planet.fs");
        srand((unsigned int)time(NULL));
        for (int i = 0; i < numStars; i++) {
            float size = (float)((rand() % 5) + 1.0f) / 500;
            float posx = (float)((rand() % 1000) / 500.0f) - 1.0f;
            float posy = (float)((rand() % 100) / 50.0f) - 1.0f;
            int scolor = rand() % 6;
            float speed = ((rand() % 100)/10.0f)+1.0f;
            Stars[i].translate(posx, posy, size, colorSet[scolor]);
            Stars[i].setSpeed(speed);
        }
    }
    void Draw(bool isPause) {
        if (!isPause) {
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < numStars; i++) Stars[i].move((Stars[i].getSpeed() / 100000.0f) * gamespeed);
            for (int i = 0; i < numStars; i++) Stars[i].draw(starShader);
            planet_->draw(planetShader);
            planet_->move((planet_->speed / 100000.0f) * gamespeed);
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
            for (int i = 0; i < numStars; i++) Stars[i].draw(starShader);
            planet_->draw(planetShader);
            glEnable(GL_DEPTH_TEST);
        }
    }

    void setSpeed(float k) {
        gamespeed = ((k * k) / 10000.0f) + 1.0f;
    }
private:
    Star* Stars;
    Planet* planet_;
    Shader *starShader, *planetShader;
    int numStars;
    float gamespeed;
    glm::vec3 colorSet[6] = {
        glm::vec3(0.85f, 0.85f, 1.0f),
        glm::vec3(0.7f, 0.95f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 0.7f),
        glm::vec3(0.7f, 1.0f, 0.7f),
        glm::vec3(1.0f, 0.7f, 0.7f)
    };
};
class Asteroid {
public:
    Asteroid() {
        for (int i = 0; i < 6; i++) {
            asteroidPath[19] = (char)i + 48;
            Model* oneAsteroid = new Model((GLchar*)asteroidPath); 
            asteroids.push_back(oneAsteroid);
        }
        gamespeed = 1.0f;
        nAsteroid = 1;
        detetor = true;
        setRandom(0);
        srand((unsigned int)time(NULL));
    }

    void setRandom(int i) {
        rotSpeed[i] = (rand() % 5) + 1;
        speed[i] = (rand() % 5) + 1;
        rotateBias[i].x = (float)(rand() % 100) / 100.0f;
        rotateBias[i].y = (float)(rand() % 100) / 100.0f;
        rotateBias[i].z = (float)(rand() % 100) / 100.0f;
        transpos[i].x = (float)((rand() % 200) / 40.0f) - 2.5f;
        transpos[i].y = 0.0f;
        transpos[i].z = 7.5f;
        curIdx[i] = rand() % 6;
    }

    void move(int i) {
        if (transpos[i].z < -5.0f) setRandom(i);
        transpos[i].z -= (float(speed[i]) * 0.0005f) * gamespeed;
    }

    void Draw(Shader *shader, bool isPause) {
        if (!isPause) {
            for (int i = 0; i < nAsteroid; i++) {
                glm::mat4 model(1.0);
                move(i);
                model = glm::translate(model, transpos[i]);
                curpos[i * 2] = (float)model[3][0];
                curpos[i * 2 + 1] = (float)model[3][2];
                model = glm::rotate(model, (float)rotSpeed[i] * (float)glfwGetTime(), rotateBias[i]);
                model = glm::translate(model, glm::vec3(-0.1f, -0.2f, -0.5f));
                model = glm::scale(model, glm::vec3(0.0002f, 0.0002f, 0.0002f));
                shader->use();
                shader->setMat4("model", model);
                asteroids[curIdx[i]]->Draw(shader);
            }
        }
        else {
            for (int i = 0; i < nAsteroid; i++) {
                glm::mat4 model(1.0);
                model = glm::translate(model, transpos[i]);
                curpos[i * 2] = (float)model[3][0];
                curpos[i * 2 + 1] = (float)model[3][2];
                model = glm::translate(model, glm::vec3(-0.1f, -0.2f, -0.5f));
                model = glm::scale(model, glm::vec3(0.0002f, 0.0002f, 0.0002f));
                shader->use();
                shader->setBool("isCompos", false);
                shader->setMat4("model", model);
                asteroids[curIdx[i]]->Draw(shader);
            }
        }
    }

    void upnAsteroid() {
        if (nAsteroid < MAXASTROID) setRandom(nAsteroid++);
    }

    bool attackdetetor(bool on) {
        if (!on) {
            detetor = true;
            return true;
        }
        else {
            detetor = false;
            return false;
        }
    }

    bool isAttack(float dx, float dy) {
        for (int i = 0; i < nAsteroid; i++) {
            if (dx - XHITBOX < curpos[i * 2] && curpos[i * 2] < dx + XHITBOX) {
                if (dy - 0.3f < curpos[i * 2 + 1] && curpos[i * 2 + 1] < dy + YHITBOX) {
                    if(detetor) setRandom(i);
                    return true;
                }
            }
        }
        return false;
    }

    void setSpeed(float k) {
        gamespeed = (k * k) / 10000.0f + 1.0f;
    }
private:
    vector<Model*> asteroids;
    char asteroidPath[39] = "res/models/asteroid0/asteroid1.obj";
    glm::vec3 transpos[MAXASTROID];
    glm::vec3 rotateBias[MAXASTROID];
    int rotSpeed[MAXASTROID], speed[MAXASTROID], curIdx[MAXASTROID], nAsteroid;
    float curpos[MAXASTROID * 2], gamespeed;
    bool detetor;
};
class SpaceShip {
public:
    SpaceShip() {
        Ship = new Model((GLchar*)"res/models/space ship/nave_orion.obj");
        shipPan = glm::vec3(0.0f, 0.0f, -3.0f);
        blend = false;
    }

    void movex(float dx) {
        if(-2.5f < shipPan.x + dx && shipPan.x + dx < 2.5f) shipPan.x += dx;
    }

    void movez(float dz) {
        if(-3.7f < shipPan.z + dz && shipPan.z + dz < - 0.9f) shipPan.z += dz;
    }

    void Draw(Shader* shader, glm::mat4 mat) {
        // Draw the loaded model
        glm::mat4 model(1.0);
        model = glm::translate(model, shipPan);

        //Meteorite Collision Test
        shipPos[0] = model[3][0];
        shipPos[1] = model[3][2];

        model = glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = model * mat;

        //scale and rotate bias modify
        model = glm::scale(model, glm::vec3(0.003f, 0.003f, 0.003f));

        shader->use();
        shader->setMat4("model", model);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        if (blend) shader->setBool("isAttack", true);
        Ship->Draw(shader);
        shader->setBool("isAttack", false);
        glDisable(GL_BLEND);
    }

    void setBlend(bool t) {blend = t;}

    float xpos() { return shipPos[0]; }
    float zpos() { return shipPos[1]; }
private:
    Model* Ship;
    glm::vec3 shipPan;
    float shipPos[2];
    bool blend;
};
class Textbox {
public:
    Textbox() {
        textShader = new Shader("res/shaders/text.vs", "res/shaders/text.fs");
        text = new Text((char*)"res/fonts/arial.ttf", textShader, SCR_WIDTH, SCR_HEIGHT);
        ntext = 0;
    }

    void append(char* line, float xpos, float ypos, float scale, glm::vec3 Color) {
        x.push_back(xpos);
        y.push_back(ypos);
        size.push_back(scale);
        textline.push_back(line);
        color.push_back(Color);
        ntext++;
    }

    void Draw() {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        for (int i = 0; i < ntext; i++) {
            text->RenderText(textline[i], x[i], y[i], size[i], color[i]);
        }
        glDisable(GL_BLEND);
    }

    void modify(int idx, char* t) {
        textline[idx] = t;
    }

private:
    int ntext;
    Text* text;
    Shader* textShader;
    vector<float> x, y, size;
    vector<glm::vec3> color;
    vector<char*> textline;
};

// Globals
backGround *BackG = NULL;
Asteroid* meteo = NULL;
SpaceShip* spaceShip = NULL;
Textbox* Start = NULL;
Textbox* Playing = NULL;
Textbox* EndGame = NULL;

 //For model
int prenum = 0;
float invincible = -3.0f;
bool isInvincible = false;

// for camera
glm::vec3 cameraPos(0.0f, 9.0f, -3.0f);

//for animation
enum Turn { LEFT, RIGHT, STOP, BACK };
Turn isTurn;
float zAngle;
float beginT, timeT, endT;
KeyFraming LRKF(4), RRKF(4);

// Function prototypes
GLFWwindow* glAllInit();
void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods);
void render();
void initKeyframes();
void updateAnimData();
void isAttack();
glm::mat4 animeting();
void appendText();

//for game
enum Game {INIT, KEEP, PAUSE, END};
Game isState;
int score, life;
float Gbegin, Gkeep, curtime;
bool isPause;
string tscore = "Score : ";
string totalScore = "Total Score: ";

int main( )
{
    mainWindow = glAllInit();
    BackG = new backGround(200);
    meteo = new Asteroid();
    spaceShip = new SpaceShip();
    Start = new Textbox();
    Playing = new Textbox();
    EndGame = new Textbox();
    shader = new Shader("res/shaders/model.vs", "res/shaders/model.fs", "res/shaders/model.gs"); 
    
    appendText();

    // Projection initialization
    projection = glm::perspective(glm::radians(55.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    shader->use();
    shader->setMat4("projection", projection);
    shader->setBool("isCompos", false);
    
    //initial animation
    endT = 0.1f;
    timeT = 0.0f;
    isTurn = STOP;
    initKeyframes();
    updateAnimData();

    //initail game
    life = 4;
    score = 0;
    isState = INIT;
    isPause = false;

    //for anti aliasing
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_POINT_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_DONT_CARE);

    //Game loop
    while( !glfwWindowShouldClose( mainWindow ) ){
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

    if (isState == INIT) {
        BackG->Draw(isPause);
        Start->Draw();
        return;
    }
    if (isState == END) {
        isPause = true;
        string newtotal = totalScore + to_string(score);
        EndGame->modify(0, (char*)newtotal.c_str());
        BackG->Draw(isPause);
        spaceShip->setBlend(false);
        spaceShip->Draw(shader, glm::mat4(1.0f));
        meteo->Draw(shader, isPause);
        EndGame->Draw();
        return;
    }

    glm::mat4 view = glm::lookAt(cameraPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    shader->use();
    shader->setVec3("viewPos", cameraPos);
    shader->setVec3("direction", glm::vec3(0.0f, 9.0f, -1.0f));
    shader->setMat4("view", view);

    string newscore = tscore + to_string(score);
    Playing->modify(0, (char*)newscore.c_str());

    if (isPause) {
        shader->setBool("isCompos", true);
        shader->setFloat("time", glfwGetTime());

        BackG->Draw(isPause);
        Playing->Draw();
        spaceShip->Draw(shader, glm::mat4(1.0f));
        glDisable(GL_BLEND);
        meteo->Draw(shader, isPause);
        return;
    }

    curtime = (float)glfwGetTime();
    Gkeep = (curtime - Gbegin);
    score = (int)(Gkeep * 10);

    BackG->setSpeed(Gkeep);
    meteo->setSpeed(Gkeep);
    spaceShip->setBlend(false);

    isAttack();

    if ((float)glfwGetTime() - invincible < 2.0f) spaceShip->setBlend(true);
    if (glfwGetTime() - invincible < 2.2f) isInvincible = true;
    else isInvincible = false;

    //for animation
    glm::mat4 rotMatrix = animeting();

    if (prenum != (int)(Gkeep/10)) {
        meteo->upnAsteroid();
        prenum = (int)(Gkeep/10);
    }
   
    BackG->Draw(isPause);
    Playing->Draw();
    spaceShip->Draw(shader, rotMatrix);
    meteo->Draw(shader, isPause);
}

void isAttack() {
    if (meteo->isAttack(spaceShip->xpos(), spaceShip->zpos()) && meteo->attackdetetor(isInvincible)) {
        life--;
        invincible = (float)glfwGetTime();
        if (life == 2) Playing->modify(1, (char*)"Life : 2");
        else if (life == 1) Playing->modify(1, (char*)"Life : 1");
        else if (life == 0) {
            Playing->modify(1, (char*)"Life : 0");
            isState = END;
        }
    }
}

void initKeyframes() {

    // right-rotation keyframes
    RRKF.setKey(0, 0, 0.0);
    RRKF.setKey(1, endT/3.0f, 10.0f);
    RRKF.setKey(2, endT * (2.0f/3.0f), 25.0f);
    RRKF.setKey(3, endT, 30.0f);

    //left-rotation keyframes
    LRKF.setKey(0, 0, 0.0);
    LRKF.setKey(1, endT / 3.0f, -10.0f);
    LRKF.setKey(2, endT * (2.0f / 3.0f), -25.0f);
    LRKF.setKey(3, endT, -30.0f);
}

void updateAnimData() {
    if (timeT > endT) {
        isTurn = STOP;
    }
    if (isTurn == RIGHT) {
        zAngle = RRKF.getValLinear(timeT);
    }
    else if (isTurn == LEFT) {
        zAngle = LRKF.getValLinear(timeT);
    }
    else if (isTurn == BACK) {
        zAngle = 0;
    }
}

glm::mat4 animeting() {
    if (isTurn != STOP) {
        float cTime = (float)glfwGetTime(); // current time
        timeT = cTime - beginT;
    }
    updateAnimData();
    glm::vec3 eulerAngles(0, 0, glm::radians(zAngle));
    glm::quat q(eulerAngles);
    glm::mat4 Matrix = q.operator glm::mat4x4();
    return Matrix;
}

void appendText() {
    Start->append((char*)"Exploration of Space", 130.0f, 800.0f, 1.2f, glm::vec3(1.0f, 1.0f, 1.0f));
    Start->append((char*)"Press the space button to start", 230.0f, 300.0f, 0.5f, glm::vec3(0.8f, 0.8f, 1.0f));
    Start->append((char*)"Press ESC to exit", 310.0f, 250.0f, 0.45f, glm::vec3(1.0f, 0.6f, 0.6f));

    Playing->append((char*)"Score :  ", 0.0f, 1170.0f, 0.6f, glm::vec3(0.6f, 0.8f, 1.0f));
    Playing->append((char*)"Life : 3", 710.0f, 1170.0f, 0.6f, glm::vec3(1.0f, 0.9f, 0.6f));

    EndGame->append((char*)"Total Score : ", 170.0f, 600.0f, 1.3f, glm::vec3(0.5f, 0.5f, 1.0f));
    EndGame->append((char*)"Press the space button to restart", 230.0f, 300.0f, 0.5f, glm::vec3(0.8f, 0.8f, 1.0f));
    EndGame->append((char*)"Press ESC to exit", 310.0f, 250.0f, 0.45f, glm::vec3(1.0f, 0.6f, 0.6f));
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
    glfwWindowHint( GLFW_SAMPLES, 4);
    
    // Create a GLFWwindow object that we can use for GLFW's functions
    GLFWwindow *window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "Exploration of Space", nullptr, nullptr );
    
    if ( nullptr == window )
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate( );
        exit(-1);
    }
    
    glfwMakeContextCurrent( window );
    
    // Set the required callback functions
    glfwSetKeyCallback(window, key_callback);
    
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
    glEnable(GL_MULTISAMPLE);
    glEnable( GL_DEPTH_TEST );
    
    return(window);
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    else if (key == GLFW_KEY_LEFT && !isPause) {
        if (action == GLFW_PRESS) {
            spaceShip->movex(0.1f);
            beginT = glfwGetTime();
            isTurn = LEFT;
        }
        else if (action == GLFW_RELEASE) {
            beginT = glfwGetTime();
            isTurn = BACK;
        }
        else {
            spaceShip->movex(0.1f);
        }
    }
    else if (key == GLFW_KEY_RIGHT && !isPause) {
        if (action == GLFW_PRESS) {
            spaceShip->movex(-0.1f);
            beginT = glfwGetTime();
            isTurn = RIGHT;
        }
        else if (action == GLFW_RELEASE) {
            beginT = glfwGetTime();
            isTurn = BACK;
        }
        else {
            spaceShip->movex(-0.1f);
        }
    }
    else if (key == GLFW_KEY_DOWN && !isPause) {
        spaceShip->movez(-0.1f);
    }
    else if (key == GLFW_KEY_UP && !isPause) {
        spaceShip->movez(0.1f);
    }
    else if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
        if (isState == INIT) {
            isState = KEEP;
            glfwSetTime(0.0f);
            Gbegin = glfwGetTime();
        }
        else if (isState == END) {
            isPause = false;
            isState = INIT;
            life = 3;
            score = 0;
            Gbegin = glfwGetTime();
            Playing->modify(1, (char*)"Life : 3");
        }
        else if(isState == KEEP) {
            glfwSetTime(0.0f);
            isPause = true;
            isState = PAUSE;
        }
        else {
            glfwSetTime(curtime);
            isPause = false;
            isState = KEEP;
        }
    }
}

