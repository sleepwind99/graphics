#ifndef QUADPYRA_H
#define QUADPYRA_H

#include "shader.h"

class QuadPyra {
public:

    // vertex position array
    GLfloat pyraVertices[54] = { // initialized as size = 1 for each dimension, 72 elements
        .0f, .5f, .0f,  -.5f, -.5f, .5f,  .5f,-.5f, .5f, // a, b, c (front)
        .0f, .5f, .0f,   .5f,-.5f, .5f,   .5f,-.5f,-.5f, // a, c, d (right)
        .0f, .5f, .0f,  -.5f, -.5f,-.5f,  -.5f,-.5f, .5f, // a, e, b (left)
        .0f, .5f, .0f,   .5f,-.5f,-.5f,  -.5f, -.5f,-.5f,  // a, d, e (back)
        -.5f,-.5f, .5f,  -.5f,-.5f,-.5f,  .5f,-.5f, .5f, // b, e, c (bottom1)
        -.5f,-.5f,-.5f,   .5f,-.5f, -.5f,  .5f,-.5f, .5f, // e, d, c (bottom2)
    };

    // colour array
    GLfloat pyraColors[72] = { // initialized as RGBA sollid color for each face, 96 elements
        1, 0, 0, 1,   1, 0, 0, 1,   1, 0, 0, 1,  //  a, b, c (front)
        0, 1, 0, 1,   0, 1, 0, 1,   0, 1, 0, 1,  //  a, c, d (right)
        1, 1, 0, 1,   1, 1, 0, 1,   1, 1, 0, 1,  //  a, e, b (left)
        0, 0, 1, 1,   0, 0, 1, 1,   0, 0, 1, 1,  //  a, d, e (back)
        1, 0, 1, 1,   1, 0, 1, 1,   1, 0, 1, 1,  //  b, e, c (bottom1)
        1, 0, 1, 1,   1, 0, 1, 1,   1, 0, 1, 1,  // e, d, c (bottom2)
    };

    GLuint pyraIndices[18] = { 
        0, 1, 2,   // a, b, c (front)
        3, 4, 5,   // a, c, d (right)
        6, 7, 8,     // a, e, b (left)
        9,10,11,   // a, d, e (back)
        12,13,14,   // b, e, c (bottom1)
        15,16,17,   // e, d, c (bottom2)
    };

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    unsigned int vSize = sizeof(pyraVertices);
    unsigned int cSize = sizeof(pyraColors);

    QuadPyra() {
        initBuffers();
    };

    QuadPyra(float sx, float sy, float sz) {
        scale2(sx, sy, sz);
        initBuffers();
    };

    QuadPyra(float dx, float dy, float dz, float s) {
        scale(s);
        translate(dx, dy, dz);
        initBuffers();
    };

    QuadPyra(float dx, float dy, float dz, float sx, float sy, float sz) {
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
        glBufferData(GL_ARRAY_BUFFER, vSize + cSize, 0, GL_STATIC_DRAW); // reserve space
        glBufferSubData(GL_ARRAY_BUFFER, 0, vSize, pyraVertices);                  // copy verts at offset 0
        glBufferSubData(GL_ARRAY_BUFFER, vSize, cSize, pyraColors);          

        // copy index data to EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(pyraIndices), pyraIndices, GL_STATIC_DRAW);

        // attribute position initialization
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);  // position attrib
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(vSize)); //color attrib
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

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
        for (int i = 0; i < 54; i++) {
            if (i % 3 == 0) pyraVertices[i] += dx;
            else if (i % 3 == 1) pyraVertices[i] += dy;
            else if (i % 3 == 2) pyraVertices[i] += dz;
        }
    };

    void scale(float s) {
        for (int i = 0; i < 54; i++)
            pyraVertices[i] *= s;
    };

    void scale2(float sx, float sy, float sz) {
        for (int i = 0; i < 54; i++) {
            if (i % 3 == 0) pyraVertices[i] *= sx;
            else if (i % 3 == 1) pyraVertices[i] *= sy;
            else if (i % 3 == 2) pyraVertices[i] *= sz;
        }
    }
};
#endif
