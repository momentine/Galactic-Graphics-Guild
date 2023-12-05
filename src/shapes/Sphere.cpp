#include "Sphere.h"

void Sphere::updateParams(int param1, int param2) {
   if (param1 != m_param1 || param2 != m_param2) {
       m_vertexData = std::vector<float>();
       m_param1 = param1 < 2? 2 : param1;
       m_param2 = param2 < 3? 3 : param2;
       setVertexData();
   }
}

glm::vec2 uvOfPoint(glm::vec3 inter, float theta) {
    auto v = asin(inter[1]/0.5f)/M_PI + 0.5f;

//    auto theta = atan2(inter[2], inter[0]);
//    float u = theta < 0? (-theta/(2.0f*M_PI)): 1 - (theta/(2.0f*M_PI));
    float u = (theta/(2*M_PI));

    u = v == 0? 0.5f : u;
    u = v == 1? 0.5f : u;

    return glm::vec2(u, v);
}

void Sphere::makeTriangle(glm::vec3 p1, float p1_theta, glm::vec3 p2, float p2_theta, glm::vec3 p3, float p3_theta) {
   insertVec3(m_vertexData, p1);
   insertVec3(m_vertexData, glm::normalize(p1)); // Normals
   insertVec2(m_vertexData, uvOfPoint(p1, p1_theta));
   insertVec3(m_vertexData, p2);
   insertVec3(m_vertexData, glm::normalize(p2)); // Normals
   insertVec2(m_vertexData, uvOfPoint(p2, p2_theta));
   insertVec3(m_vertexData, p3);
   insertVec3(m_vertexData, glm::normalize(p3)); // Normals
   insertVec2(m_vertexData, uvOfPoint(p3, p3_theta));
}

void Sphere::makeTile(glm::vec3 topLeft, float theta1,
                     glm::vec3 topRight, float theta2,
                     glm::vec3 bottomLeft, float theta3,
                     glm::vec3 bottomRight, float theta4) {
   // Task 5: Implement the makeTile() function for a Sphere
   // Note: this function is very similar to the makeTile() function for Cube,
   //       but the normals are calculated in a different way!
   makeTriangle(topLeft, theta1, bottomLeft, theta3, bottomRight, theta4);
   makeTriangle(topLeft, theta1, bottomRight, theta4, topRight, theta2);
}

glm::vec3 angToCoord(float phi, float theta) {
   return glm::vec3(0.5*glm::sin(phi)*glm::sin(theta), 0.5*glm::cos(phi), 0.5*glm::sin(phi)*glm::cos(theta));
}

void Sphere::makeWedge(float currentTheta, float nextTheta) {
   // Task 6: create a single wedge of the sphere using the
   //         makeTile() function you implemented in Task 5
   // Note: think about how param 1 comes into play here!
   float d_phi = M_PI * (1.0f/m_param1);
   for (int i = 0; i < m_param1; i++) {
       glm::vec3 bottomRight_t = angToCoord((float) (i+1)*d_phi, nextTheta);
       glm::vec3 topRight_t = angToCoord((float) (i)*d_phi, nextTheta);
       glm::vec3 bottomLeft_t = angToCoord((float) (i+1)*d_phi, currentTheta);
       glm::vec3 topLeft_t = angToCoord((float) (i)*d_phi, currentTheta);
       makeTile(topLeft_t, currentTheta, topRight_t, nextTheta, bottomLeft_t, currentTheta, bottomRight_t, nextTheta);
   }
}

void Sphere::makeSphere() {
   // Task 7: create a full sphere using the makeWedge() function you
   //         implemented in Task 6
   // Note: think about how param 2 comes into play here!
   float thetaStep = glm::radians(360.f / m_param2);
   for (int i = 0; i < m_param2; i++) {
       makeWedge((float)i * thetaStep, (float)(i+1) * thetaStep);
   }
}

void Sphere::setVertexData() {
   // Uncomment these lines to make a wedge for Task 6, then comment them out for Task 7:

   // float thetaStep = glm::radians(360.f / m_param2);
   // float currentTheta = 0 * thetaStep;
   // float nextTheta = 1 * thetaStep;
   // makeWedge(currentTheta, nextTheta);

   // Uncomment these lines to make sphere for Task 7:

    makeSphere();
}

// Inserts a glm::vec3 into a vector of floats.
// This will come in handy if you want to take advantage of vectors to build your shape!
void Sphere::insertVec3(std::vector<float> &data, glm::vec3 v) {
   data.push_back(v.x);
   data.push_back(v.y);
   data.push_back(v.z);
}

void Sphere::insertVec2(std::vector<float> &data, glm::vec2 v) {
   data.push_back(v.x);
   data.push_back(v.y);
}
