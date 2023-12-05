#include "Cylinder.h"
#include "glm/gtc/constants.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include <iostream>

void Cylinder::updateParams(int param1, int param2) {
    if (param1 != m_param1 || param2 != m_param2) {
        m_vertexData = std::vector<float>();
        m_param1 = param1 < 1? 1 : param1;
        m_param2 = param2 < 3? 3 : param2;
        setVertexData();
    }
}

glm::vec2 Cylinder::uvOfPointSide(glm::vec3 v, float theta) {
//    auto theta = atan2(v[2], v[0]);
//    float u = theta < 0? (-theta/(2.0f*M_PI)): 1 - (theta/(2.0f*M_PI));
    float u = (theta/(2*M_PI));

    return glm::vec2(glm::clamp(u, 0.0f, 1.0f), v[1]+0.5f);
}

void Cylinder::makeTriangle(glm::vec3 p1, float theta1, glm::vec3 p2, float theta2, glm::vec3 p3, float theta3) {
    insertVec3(m_vertexData, p1);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(p1.x, 0, p1.z))); // Normals
    insertVec2(m_vertexData, uvOfPointSide(p1, theta1));
    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(p2.x, 0, p2.z))); // Normals
    insertVec2(m_vertexData, uvOfPointSide(p2, theta2));
    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(p3.x, 0, p3.z))); // Normals
    insertVec2(m_vertexData, uvOfPointSide(p3, theta3));
}

glm::vec2 uvOfPointCap(glm::vec3 v) {
    if (v[1] > 0) {
        return glm::vec2(v[0]+0.5, -v[2]+0.5);
    } else {
        return glm::vec2(v[0]+0.5, v[2]+0.5);
    }
}

void Cylinder::makeTriangleCap(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, bool top) {
    glm::vec3 n = top? glm::vec3(0,1,0): glm::vec3(0,-1,0);
    insertVec3(m_vertexData, p1);
    insertVec3(m_vertexData, n);
    insertVec2(m_vertexData, uvOfPointCap(p1));

    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, n);
    insertVec2(m_vertexData, uvOfPointCap(p2));

    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, n);
    insertVec2(m_vertexData, uvOfPointCap(p3));

}

void Cylinder::makeCap(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, bool top) {
    auto d_len = glm::length(p2 - p1)/(float)m_param1;

    // Create Top Mini Triangle
    auto p2_dir = glm::normalize(p2 - p1);
    auto p3_dir = glm::normalize(p3 - p1);
    makeTriangleCap(p1, p1 + p2_dir * d_len, p1 + p3_dir * d_len, top);

    // Create Trapezoids and The Two Triangles Corresponding to Each One
    for (int i = 1; i < m_param1; i++) {
        makeTriangleCap(p1 + p2_dir * (float)i * d_len, p1 + p2_dir * (float)(i+1) * d_len, p1 + p3_dir * (float)(i+1) * d_len, top);
        makeTriangleCap(p1 + p3_dir * (float)(i+1) * d_len, p1 + p3_dir * (float)i * d_len, p1 + p2_dir * (float)i * d_len, top);
    }
}

void Cylinder::makeTile(glm::vec3 topLeft, float theta1,
                        glm::vec3 topRight, float theta2,
                        glm::vec3 bottomLeft, float theta3,
                        glm::vec3 bottomRight, float theta4) {

    makeTriangle(topLeft, theta1, bottomLeft, theta3, bottomRight, theta4);
    makeTriangle(bottomRight, theta4, topRight, theta2, topLeft, theta1);
}

glm::vec3 Cylinder::angToCoord(float y, float theta) {
    return glm::vec3(0.5*glm::sin(theta), y, 0.5*glm::cos(theta));
}

void Cylinder::makeWedge(float currentTheta, float nextTheta) {
    float initial_y = -0.5f;
    float d_y = 1.0f/m_param1;
    for (int i = 0; i < m_param1; i++) {
        glm::vec3 bottomLeft_t = angToCoord(initial_y + (float) i*d_y, currentTheta);
        glm::vec3 topLeft_t = angToCoord(initial_y + (float) (i+1)*d_y, currentTheta);
        glm::vec3 bottomRight_t = angToCoord(initial_y + (float) i*d_y, nextTheta);
        glm::vec3 topRight_t = angToCoord(initial_y + (float) (i+1)*d_y, nextTheta);
        makeTile(topLeft_t, currentTheta, topRight_t, nextTheta, bottomLeft_t, currentTheta, bottomRight_t, nextTheta);
    }
}

void Cylinder::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        makeWedge((float)i * thetaStep, (float)(i+1) * thetaStep);
        makeCap(glm::vec3(0.0f, 0.5f, 0.0f), angToCoord(0.5, (float)i * thetaStep), angToCoord(0.5, (float)(i+1) * thetaStep), true);
        makeCap(glm::vec3(0.0f, -0.5f, 0.0f), angToCoord(-0.5, (float)(i+1) * thetaStep), angToCoord(-0.5, (float)(i) * thetaStep), false);
    }
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cylinder::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cylinder::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
