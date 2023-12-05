#pragma once

#include <vector>
#include <glm/glm.hpp>

class Sphere
{
public:
    void updateParams(int param1, int param2);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    void setVertexData();
    void makeTriangle(glm::vec3 p1, float p1_theta, glm::vec3 p2, float p2_theta, glm::vec3 p3, float p3_theta);
    void makeTile(glm::vec3 topLeft, float theta1,
                  glm::vec3 topRight, float theta2,
                  glm::vec3 bottomLeft, float theta3,
                  glm::vec3 bottomRight, float theta4);
    void makeWedge(float currTheta, float nextTheta);
    void makeSphere();

    std::vector<float> m_vertexData;
    float m_radius = 0.5;
    int m_param1;
    int m_param2;
};
