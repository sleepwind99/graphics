
#ifndef QUADPYRA_H
#define QUADPYRA_H

#include "shader.h"

class QuadPyra {
public:
    
    // vertex position array
    GLfloat vertices[48]  = { // (4 faces x 3 vertices x 3) + (4 vertices x 3) = 36 + 12 = 48
        .0f, .5f, .0f,   -.5f, -.5f,  .5f,    .5f, -.5f,  .5f,    // front
        .0f, .5f, .0f,    .5f, -.5f,  .5f,    .5f, -.5f, -.5f,    // right
        .0f, .5f, .0f,    .5f, -.5f, -.5f,   -.5f, -.5f, -.5f,   // back
        .0f, .5f, .0f,   -.5f, -.5f, -.5f,   -.5f, -.5f,  .5f,   // left
       -.5f,-.5f, .5f,   -.5f, -.5f, -.5f,    .5f, -.5f, -.5f,  .5f, -.5f, .5f // bottom
    };
    
    // color array
    GLfloat colors[64] = { // (4 faces x 3 vertices x 4) + (4 vertices x 4) = 48 + 16 = 64
        1, 0, 0, 1,  1, 0, 0, 1,  1, 0, 0, 1,    // front: red
        0, 1, 0, 1,  0, 1, 0, 1,  0, 1, 0, 1,    // right: green
        0, 0, 1, 1,  0, 0, 1, 1,  0, 0, 1, 1,    // back: blue
        1, 1, 0, 1,  1, 1, 0, 1,  1, 1, 0, 1,    // left: yellow
        1, 0, 1, 1,  1, 0, 1, 1,  1, 0, 1, 1,  1, 0, 1, 1  // bottom: magenta
    };

    GLuint Indices[18] = { // (6 triangles x 3 indices)
       0, 1, 2,      // front
       3, 4, 5,      // right
       6, 7, 8,      // back
       9, 10, 11,   // left
       12, 13, 15,  // bottom 1
       15, 14, 13   // bottom 2
    };

    GLfloat TexCoords[32];

    //default normals
    GLfloat Normals[48] = { 
        0, 1, 2,   0, 1, 2,   0, 1, 2,                  // a,b,c (front)
        2, 1, 0,   2, 1, 0,   2, 1, 0,                  // a,c,d (right)
        0, 1,-2,  0, 1,-2,  0, 1,-2,                  // a,d,e (back)
       -2, 1, 0, -2, 1, 0, -2, 1, 0,                  // a,e,b (left)
        0,-1, 0,   0,-1, 0,   0,-1, 0,   0,-1, 0                                  // b,e,d,c (bottom)
    };

    //for smothing normals
    GLfloat smoothNormals[48];

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    unsigned int vSize = sizeof(vertices);
    unsigned int cSize = sizeof(colors);
    unsigned int tSize = sizeof(TexCoords);
    unsigned int nSize = sizeof(Normals);
    
    QuadPyra() {

        smootihng();

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);
        
        glBindVertexArray(VAO);
        
        // copy vertex attrib data to VBO
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vSize+cSize+tSize+nSize, 0, GL_STATIC_DRAW);   // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, vertices);                            // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, colors);                         // copy cols after positions
        glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize, tSize, TexCoords); 
        glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize + tSize, nSize, Normals);

        // copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
        
        // attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*) vSize); //color attrib
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(vSize + cSize)); //texture
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vSize + cSize + tSize)); // normal

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);
        glEnableVertexAttribArray(3);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    };
    
    void draw(Shader *shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

    //smooth made change function
    void isSmooth(bool smooth) {
        if (smooth) {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize + tSize, nSize, smoothNormals);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        else {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, vSize + cSize + tSize, nSize, Normals);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
    }

    //generate smoothing normal
    void smootihng() {
        glm::vec3 a, b, c, d, e;
        a = (glm::vec3(0, 1, 2) + glm::vec3(2, 1, 0) + glm::vec3(0, 1, -2) + glm::vec3(-2, 1, 0)) / glm::vec3(4,4,4);
        b = (glm::vec3(0, 1, 2) + glm::vec3(-2, 1, 0) + glm::vec3(0, -2.2360, 0)) / glm::vec3(3, 3, 3);
        c = (glm::vec3(0, 1, 2) + glm::vec3(2, 1, 0) + glm::vec3(0, -2.2360, 0)) / glm::vec3(3, 3, 3);
        d = (glm::vec3(2, 1, 0) + glm::vec3(0, 1, -2) + glm::vec3(0, -2.2360, 0)) / glm::vec3(3, 3, 3);
        e = (glm::vec3(0, 1, -2) + glm::vec3(-2, 1, 0) + glm::vec3(0, -2.2360, 0)) / glm::vec3(3, 3, 3);
        int i = 0;

        //a,b,c
        smoothNormals[i++] = a.x;
        smoothNormals[i++] = a.y;
        smoothNormals[i++] = a.z;
        smoothNormals[i++] = b.x;
        smoothNormals[i++] = b.y;
        smoothNormals[i++] = b.z;
        smoothNormals[i++] = c.x;
        smoothNormals[i++] = c.y;
        smoothNormals[i++] = c.z;
        //a,c,d
        smoothNormals[i++] = a.x;
        smoothNormals[i++] = a.y;
        smoothNormals[i++] = a.z;
        smoothNormals[i++] = c.x;
        smoothNormals[i++] = c.y;
        smoothNormals[i++] = c.z;
        smoothNormals[i++] = d.x;
        smoothNormals[i++] = d.y;
        smoothNormals[i++] = d.z;
        //a,d,e
        smoothNormals[i++] = a.x;
        smoothNormals[i++] = a.y;
        smoothNormals[i++] = a.z;
        smoothNormals[i++] = d.x;
        smoothNormals[i++] = d.y;
        smoothNormals[i++] = d.z;
        smoothNormals[i++] = e.x;
        smoothNormals[i++] = e.y;
        smoothNormals[i++] = e.z;
        //a,e,b
        smoothNormals[i++] = a.x;
        smoothNormals[i++] = a.y;
        smoothNormals[i++] = a.z;
        smoothNormals[i++] = e.x;
        smoothNormals[i++] = e.y;
        smoothNormals[i++] = e.z;
        smoothNormals[i++] = b.x;
        smoothNormals[i++] = b.y;
        smoothNormals[i++] = b.z;
        //b,e,d,c (bottom)
        smoothNormals[i++] = b.x;
        smoothNormals[i++] = b.y;
        smoothNormals[i++] = b.z;
        smoothNormals[i++] = e.x;
        smoothNormals[i++] = e.y;
        smoothNormals[i++] = e.z;
        smoothNormals[i++] = d.x;
        smoothNormals[i++] = d.y;
        smoothNormals[i++] = d.z;
        smoothNormals[i++] = c.x;
        smoothNormals[i++] = c.y;
        smoothNormals[i++] = c.z;
    }
};


#endif
