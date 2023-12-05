#include "Cone.h"

void Cone::updateParams(int param1, int param2) {
    if (param1 != m_param1 || param2 != m_param2) {
        m_vertexData = std::vector<float>();
        m_param1 = param1 < 1? 1 : param1;
        m_param2 = param2 < 3? 3 : param2;
        setVertexData();
    }
}

glm::vec2 uvOfPointSideCone(glm::vec3 v, float theta) {
//    auto theta = atan2(v[2], v[0]);
//    auto u = theta < 0? (-theta/(2.0f*M_PI)): 1 - (theta/(2.0f*M_PI));
    float u = (theta/(2*M_PI));

    return glm::vec2(u, v[1]+0.5f);
}

void Cone::makeTriangleTip(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float curTheta, float nextTheta) {
    insertVec3(m_vertexData, p1);
    auto c = angToCoord(-0.5, ((curTheta + nextTheta) / 2.0f));
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2.0f*c.x, 0.25f - c.y/2.0f, 2.0f*c.z)));
    insertVec2(m_vertexData, uvOfPointSideCone(p1, 0));

    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2.0f*p2.x, 0.25f - p2.y/2.0f, 2.0f*p2.z)));
    insertVec2(m_vertexData, uvOfPointSideCone(p2, curTheta));
    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2.0f*p3.x, 0.25f - p3.y/2.0f, 2.0f*p3.z)));
    insertVec2(m_vertexData, uvOfPointSideCone(p3, nextTheta));
}


void Cone::makeTriangle(glm::vec3 p1, float theta1, glm::vec3 p2, float theta2, glm::vec3 p3, float theta3) {
    insertVec3(m_vertexData, p1);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2.0f*p1.x, 0.25f - p1.y/2.0f, 2.0f*p1.z)));
    insertVec2(m_vertexData, uvOfPointSideCone(p1, theta1));
    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2.0f*p2.x, 0.25f - p2.y/2.0f, 2.0f*p2.z)));
    insertVec2(m_vertexData, uvOfPointSideCone(p2, theta2));
    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, glm::normalize(glm::vec3(2.0f*p3.x, 0.25f - p3.y/2.0f, 2.0f*p3.z)));
    insertVec2(m_vertexData, uvOfPointSideCone(p3, theta3));
}

glm::vec2 uvOfPointCapCone(glm::vec3 v) {
    return glm::vec2(v[0]+0.5, v[2]+0.5);
}

void Cone::makeTriangleCap(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    glm::vec3 n = glm::vec3(0,-1,0);
    insertVec3(m_vertexData, p1);
    insertVec3(m_vertexData, n);
    insertVec2(m_vertexData, uvOfPointCapCone(p1));

    insertVec3(m_vertexData, p2);
    insertVec3(m_vertexData, n);
    insertVec2(m_vertexData, uvOfPointCapCone(p2));

    insertVec3(m_vertexData, p3);
    insertVec3(m_vertexData, n);
    insertVec2(m_vertexData, uvOfPointCapCone(p3));
}

void Cone::makeCap(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3) {
    auto d_len = glm::length(p2 - p1)/(float)m_param1;

    // Create Top Mini Triangle
    auto p2_dir = glm::normalize(p2 - p1);
    auto p3_dir = glm::normalize(p3 - p1);
    makeTriangleCap(p1, p1 + p2_dir * d_len, p1 + p3_dir * d_len);

    // Create Trapezoids and The Two Triangles Corresponding to Each One
    for (int i = 1; i < m_param1; i++) {
        makeTriangleCap(p1 + p2_dir * (float)i * d_len, p1 + p2_dir * (float)(i+1) * d_len, p1 + p3_dir * (float)(i+1) * d_len);
        makeTriangleCap(p1 + p3_dir * (float)(i+1) * d_len, p1 + p3_dir * (float)i * d_len, p1 + p2_dir * (float)i * d_len);
    }
}

glm::vec3 Cone::angToCoord(float y, float theta) {
    return glm::vec3(0.5*glm::sin(theta), y, 0.5*glm::cos(theta));
}

void Cone::makeWedge(glm::vec3 p1, float curTheta, float nextTheta) {
    auto p2 = angToCoord(-0.5, curTheta);
    auto p3 = angToCoord(-0.5, nextTheta);
    auto d_len = glm::length(p2 - p1)/(float)m_param1;

    // Create Top Mini Triangle
    auto p2_dir = glm::normalize(p2 - p1);
    auto p3_dir = glm::normalize(p3 - p1);
    makeTriangleTip(p1, p1 + p2_dir * d_len, p1 + p3_dir * d_len, curTheta, nextTheta);

    // Create Trapezoids and The Two Triangles Corresponding to Each One
    for (int i = 1; i < m_param1; i++) {
        makeTriangle(p1 + p2_dir * (float)i * d_len, curTheta, p1 + p2_dir * (float)(i+1) * d_len, curTheta, p1 + p3_dir * (float)(i+1) * d_len, nextTheta);
        makeTriangle(p1 + p3_dir * (float)(i+1) * d_len, nextTheta, p1 + p3_dir * (float)i * d_len, nextTheta, p1 + p2_dir * (float)i * d_len, curTheta);
    }
}

void Cone::setVertexData() {
    // TODO for Project 5: Lights, Camera
    float thetaStep = glm::radians(360.f / m_param2);
    for (int i = 0; i < m_param2; i++) {
        makeWedge(glm::vec3(0.0f, 0.5f, 0.0f), (float)i * thetaStep, (float)(i+1) * thetaStep);
        makeCap(glm::vec3(0.0f, -0.5f, 0.0f), angToCoord(-0.5, (float)(i+1) * thetaStep), angToCoord(-0.5, (float)(i) * thetaStep));
    }
}


// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Cone::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

void Cone::insertVec2(std::vector<float> &data, glm::vec2 v) {
    data.push_back(v.x);
    data.push_back(v.y);
}
