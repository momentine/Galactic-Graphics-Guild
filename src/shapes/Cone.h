#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cone
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    void setVertexData();
    void makeTriangleTip(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, float curTheta, float nextTheta);
    void makeTriangle(glm::vec3 p1, float theta1, glm::vec3 p2, float theta2, glm::vec3 p3, float theta3);
    glm::vec3 angToCoord(float phi, float theta);
    void makeWedge(glm::vec3 p1, float curTheta, float nextTheta);

    void makeTriangleCap(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
    void makeCap(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);

    std::vector<float> m_vertexData;
    int m_param1;
    int m_param2;
    float m_radius = 0.5;
};
