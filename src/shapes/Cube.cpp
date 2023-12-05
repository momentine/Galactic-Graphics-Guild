#include "Cube.h"

void Cube::updateParams(int param1) {
    if (param1 != m_param1) {
        m_vertexData = std::vector<float>();
        m_param1 = param1 < 1? 1 : param1;
        setVertexData();
    }
}

glm::vec2 uvOfFace(glm::vec3 v, int faceNum) {
    if (faceNum == 3) {
        return glm::vec2(v[2]+0.5, v[1]+0.5);
    } else if (faceNum == 4) {
        return glm::vec2(-v[2]+0.5, v[1]+0.5);
    } else if (faceNum == 6) {
        return glm::vec2(v[0]+0.5, v[2]+0.5);
    } else if (faceNum == 5) {
        return glm::vec2(v[0]+0.5, -v[2]+0.5);
    } else if (faceNum == 2) {
        return glm::vec2(-v[0]+0.5, v[1]+0.5);
    } else if (faceNum == 1) {
        return glm::vec2(v[0]+0.5, v[1]+0.5);
    }
}

void Cube::makeTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int faceNum) {
    insertVec3(m_vertexData, p1);
    insertVec3(m_vertexData, glm::normalize(glm::cross(p2 - p1, p3 - p1))); // Normals
    insertVec2(m_vertexData, uvOfFace(p1, faceNum));
    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, glm::normalize(glm::cross(p3 - p2, p1 - p2))); // Normals
    insertVec2(m_vertexData, uvOfFace(p2, faceNum));
    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, glm::normalize(glm::cross(p1 - p3, p2 - p3))); // Normals
    insertVec2(m_vertexData, uvOfFace(p3, faceNum));
}

void Cube::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, int faceNum) {
    // Task 2: create a tile (i.e. 2 triangles) based on 4 given points.
    makeTriangle(topLeft, bottomLeft, bottomRight, faceNum);
    makeTriangle(bottomRight, topRight, topLeft, faceNum);
}

void Cube::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, int faceNum) {
    // Task 3: create a single side of the cube out of the 4
    //         given points and makeTile()
    // Note: think about how param 1 affects the number of triangles on
    //       the face of the cube
    glm::vec3 dx = (bottomRight-bottomLeft) * (1.0f/m_param1);
    glm::vec3 dy = (topLeft-bottomLeft) * (1.0f/m_param1);
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            glm::vec3 bottomLeft_t = bottomLeft+(dx*(float) i)+(dy*(float) j);
            glm::vec3 topLeft_t = bottomLeft_t+dy;
            glm::vec3 bottomRight_t = bottomLeft_t+dx;
            glm::vec3 topRight_t = topLeft_t+dx;
            makeTile(topLeft_t, topRight_t, bottomLeft_t, bottomRight_t, faceNum);
        }
    }
}

void Cube::setVertexData() {
    // Uncomment these lines for Task 2, then comment them out for Task 3:

    // makeTile(glm::vec3(-0.5f,  0.5f, 0.5f),
    //          glm::vec3( 0.5f,  0.5f, 0.5f),
    //          glm::vec3(-0.5f, -0.5f, 0.5f),
    //          glm::vec3( 0.5f, -0.5f, 0.5f));

    // Uncomment these lines for Task 3:

     makeFace(glm::vec3(-0.5f,  0.5f, 0.5f), // 1
              glm::vec3( 0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f), 1);

    // Task 4: Use the makeFace() function to make all 6 sides of the cube
     makeFace(glm::vec3( 0.5f,  0.5f, -0.5f), // 2
              glm::vec3(-0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f), 2);

     makeFace(glm::vec3(-0.5f,  0.5f, -0.5f), // 3
              glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, 0.5f), 3);

     makeFace(glm::vec3( 0.5f,  0.5f, 0.5f), // 4
              glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, 0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f), 4);

     makeFace(glm::vec3(-0.5f,  0.5f, -0.5f), // 5
              glm::vec3( 0.5f,  0.5f, -0.5f),
              glm::vec3(-0.5f,  0.5f, 0.5f),
              glm::vec3( 0.5f,  0.5f, 0.5f), 5);

     makeFace(glm::vec3(-0.5f, -0.5f, 0.5f), // 6
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3(-0.5f, -0.5f, -0.5f),
              glm::vec3( 0.5f, -0.5f, -0.5f), 6);
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cube::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cube::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
