#pragma once

#include <vector>
#include <glm/glm.hpp>

class Cube
{
public:
    void updateParams(int param1);
    std::vector<float> generateShape() { return m_vertexData; }

private:
    void insertVec3(std::vector<float> &data, glm::vec3 v);
    void insertVec2(std::vector<float> &data, glm::vec2 v);
    void setVertexData();
    void makeTriangle(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, int faceNum);
    void makeTile(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, int faceNum);
    void makeFace(glm::vec3 topLeft,
                  glm::vec3 topRight,
                  glm::vec3 bottomLeft,
                  glm::vec3 bottomRight, int faceNum);

    std::vector<float> m_vertexData;
    int m_param1;
};
