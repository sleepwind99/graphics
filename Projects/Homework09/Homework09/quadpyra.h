
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

    GLfloat TexCoords[32]{
        0.5f, 1,   0, 0,  1, 0,
        0.5f, 1,   0, 0,  1, 0,
        0.5f, 1,   0, 0,  1, 0,
        0.5f, 1,   0, 0,  1, 0,
        0, 0, 1, 0,   1, 1,   0, 1
    };

    //default normals
    GLfloat Normals[48];

    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;
    
    unsigned int vSize = sizeof(vertices);
    unsigned int cSize = sizeof(colors);
    unsigned int tSize = sizeof(TexCoords);
    unsigned int nSize = sizeof(Normals);
    
    QuadPyra() {

        computeNormals();

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
    
    void computeNormals() {
        
        // face normals
        for (int i = 0; i < 5; i++) {
            float nx, ny, nz;
            int k = i * 9;
            triangleNormal(vertices[k], vertices[k+1], vertices[k+2],
                           vertices[k+3], vertices[k+4], vertices[k+5],
                           vertices[k+6], vertices[k+7], vertices[k+8],
                           nx, ny, nz);
            Normals[k] = Normals[k+3] = Normals[k+6] = nx;
            Normals[k+1] = Normals[k+4] = Normals[k+7] = ny;
            Normals[k+2] = Normals[k+5] = Normals[k+8] = nz;
        }
        
        Normals[45] = Normals[42];
        Normals[46] = Normals[43];
        Normals[47] = Normals[44];
        
    }
    
    // triangleNormal: compute the normal vector of a triangle with three given vertices

    void triangleNormal(float v1x, float v1y, float v1z,
                        float v2x, float v2y, float v2z,
                        float v3x, float v3y, float v3z,
                        float &nx, float &ny, float &nz) {
        
        float A[3], B[3];
        
        A[0] = v3x - v2x; A[1] = v3y - v2y;  A[2] = v3z - v2z;
        B[0] = v1x - v2x; B[1] = v1y - v2y;  B[2] = v1z - v2z;
        
        nx = A[1] * B[2] - A[2] * B[1];
        ny = A[2] * B[0] - A[0] * B[2];
        nz = A[0] * B[1] - A[1] * B[0];
        
        float size = sqrt(nx * nx + ny * ny + nz * nz);
        nx = nx / size;
        ny = ny / size;
        nz = nz / size;
    }

    void draw(Shader *shader) {
        shader->use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    };

};


#endif
