#include "camera.h"

Camera::Camera(SceneCameraData scd, float near, float far, float aspectRatio)
{
   c_position = glm::vec4(glm::vec3(scd.pos), 1.0f);
   c_look = scd.look;
   c_up = scd.up;
   c_heightAngle = scd.heightAngle;

   c_near = near;
   c_far = far;
   c_aspectRatio = aspectRatio;

   // Initial Camera Position Translation
   translate_c_position = glm::vec3(0.0f);

   // Initialize the View Matrix
   ViewMatrix();
   ProjectionMatrix();
}

glm::vec3 Camera::getLook() const {
   return glm::vec3(c_look.x, c_look.y, c_look.z);
}

glm::mat4 Camera::getViewMatrix() const {
   return c_viewMatrix;
}

glm::mat4 Camera::getInvViewMatrix() const {
   return c_invViewMatrix;
}

void Camera::setNearAndFar(float near, float far) {
   if (c_near != near || c_far != far) {
       c_near = near;
       c_far = far;
       ProjectionMatrix();
   }
}

void Camera::setAspectRatio(float aspectRatio) {
   if (c_aspectRatio != aspectRatio) {
       c_aspectRatio = aspectRatio;
       ProjectionMatrix();
   }
}

glm::mat4 Camera::getProjMatrix() const {
   return c_projMatrix;
}

// ==========VIEW MATRIX==========
// Changes When Camera Position Changes
glm::mat4 Camera::TranslationMatrix() {
   // Calculate Translation Matrix
   glm::vec4 M_translate_v1 = glm::vec4(1,0,0,0);
   glm::vec4 M_translate_v2 = glm::vec4(0,1,0,0);
   glm::vec4 M_translate_v3 = glm::vec4(0,0,1,0);
   auto pos = glm::vec3(c_position) + translate_c_position;
   glm::vec4 M_translate_v4 = glm::vec4(-pos[0], -pos[1], -pos[2], 1);
   glm::mat4 M_translate(M_translate_v1,M_translate_v2,M_translate_v3,M_translate_v4);
   return M_translate;
}

// Changes when c_look or c_up change
glm::mat4 Camera::RotationMatrix() {
   // Calculate Axis
   glm::vec3 w = -glm::normalize(c_look);
   glm::vec3 v = glm::normalize(glm::vec3(c_up) - (glm::dot(glm::vec3(c_up), w) * w));
   glm::vec3 u = glm::cross(v, w);

   // Calculate Rotation Matrix
   glm::vec4 M_rotate_v1 = glm::vec4(u[0], v[0], w[0], 0.f);
   glm::vec4 M_rotate_v2 = glm::vec4(u[1], v[1], w[1], 0.f);
   glm::vec4 M_rotate_v3 = glm::vec4(u[2], v[2], w[2], 0.f);
   glm::vec4 M_rotate_v4 = glm::vec4(0.f, 0.f, 0.f, 1.f);
   glm::mat4 M_rotate(M_rotate_v1,M_rotate_v2, M_rotate_v3, M_rotate_v4);

   return M_rotate;
}

// Changes when translation or rotation matrix change
void Camera::ViewMatrix() {
   // View Matrix and Inverse View Matrix
   c_viewMatrix =  RotationMatrix()*TranslationMatrix();
   c_invViewMatrix = glm::inverse(c_viewMatrix);
}

// ==========PROJECTION MATRIX==========
// Changes when c_far and c_aspectRatio changes
glm::mat4 Camera::ScaleMatrix() {
   float tanHeight = glm::tan(c_heightAngle/2.0f);
   float tanWidth = c_aspectRatio* tanHeight;

   glm::vec4 col1(1.0f / (c_far * tanWidth), 0.0f, 0.0f, 0.0f);
   glm::vec4 col2(0.0f, 1.0f / (c_far * tanHeight), 0.0f, 0.0f);
   glm::vec4 col3(0.0f, 0.0f, 1.0f / c_far, 0.0f);
   glm::vec4 col4(0.0f, 0.0f, 0.0f, 1.0f);

   return glm::mat4(col1,col2,col3,col4);
}

// Changes when c_near and c_far change
glm::mat4 Camera::ParallelizationMatrix() {
   float c = -c_near/c_far;
   glm::vec4 col1(1.0f, 0.0f,0.0f, 0.0f);
   glm::vec4 col2(0.0f, 1.0f, 0.0f, 0.0f);
   glm::vec4 col3(0.0f, 0.0f, 1.0f/(1.0f+c), -1.0f);
   glm::vec4 col4(0.0f, 0.0f, -c/(1.0f+c), 0.0f);

   return glm::mat4x4(col1,col2,col3,col4);
}

// Changes when c_near, c_far or c_aspectRatio changes
void Camera::ProjectionMatrix() {
   glm::vec4 col1(1.0f, 0.0f, 0.0f, 0.0f);
   glm::vec4 col2(0.0f, 1.0f, 0.0f, 0.0f);
   glm::vec4 col3(0.0f, 0.0f, -2.0f, 0.0f);
   glm::vec4 col4(0.0f, 0.0f, -1.0f, 1.0f);
   glm::mat4 convertOpenGL(col1, col2,col3,col4);

   c_projMatrix = convertOpenGL * ParallelizationMatrix() * ScaleMatrix();
}

// ==========TRANSLATE CAMERA==========
void Camera::translateCameraLook(float d, bool dir) {
   if (dir) {
       translate_c_position += d * glm::vec3(glm::normalize(c_look));
   } else {
       translate_c_position += -d * glm::vec3(glm::normalize(c_look));
   }
   ViewMatrix();
}

void Camera::translateCameraRight(float d, bool dir) {
   auto perp_look_up = glm::normalize(glm::cross(glm::vec3(c_look), glm::vec3(c_up)));
   if (dir) {
       translate_c_position += d * perp_look_up;
   } else {
       translate_c_position += -d * perp_look_up;
   }
   ViewMatrix();
}

void Camera::translateCameraWorldUp(float d, bool dir) {
   if (dir) {
       translate_c_position += glm::vec3(0.0f, d, 0.0f);
   } else {
       translate_c_position += glm::vec3(0.0f, -d, 0.0f);
   }
   ViewMatrix();
}

// ==========ROTATE CAMERA==========
glm::mat4 calculateRotationMatrixAxisAngle(float angle, glm::vec3 axis) {
   double cA = cos(angle);
   double sA = sin(angle);

   glm::vec4 vec1 = glm::vec4(cA + axis.x*axis.x*(1-cA), axis.x*axis.y*(1-cA)+axis.z*sA, axis.x*axis.z*(1-cA)-axis.y*sA, 0.0f);
   glm::vec4 vec2 = glm::vec4(axis.x*axis.y*(1-cA)-axis.z*sA, cA + axis.y*axis.y*(1-cA), axis.y*axis.z*(1-cA)+axis.x*sA, 0.0f);
   glm::vec4 vec3 = glm::vec4(axis.x*axis.z*(1-cA)+axis.y*sA, axis.y*axis.z*(1-cA)-axis.x*sA, cA + axis.z*axis.z*(1-cA), 0.0f);
   glm::vec4 vec4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

   return glm::mat4(vec1, vec2, vec3, vec4);
}

void Camera::rotateCameraWorldUp(float d) {
   c_look = calculateRotationMatrixAxisAngle(glm::radians(d), glm::vec3(0, 1, 0)) * c_look;
   ViewMatrix();
}

void Camera::rotateCameraPerpLookUp(float d) {
   auto perp_look_up = glm::normalize(glm::cross(glm::vec3(c_look), glm::vec3(c_up)));
   auto rot = calculateRotationMatrixAxisAngle(glm::radians(d), perp_look_up);
   c_look = rot * c_look;
//   c_up = rot * c_up;
   ViewMatrix();
}
