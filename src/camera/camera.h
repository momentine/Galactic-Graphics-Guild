#ifndef CAMERA_H
#define CAMERA_H

#include "utils/scenedata.h"

class Camera
{
public:
   Camera(SceneCameraData cameraData, float near, float far, float aspectRatio);

   glm::mat4 getViewMatrix() const;
   glm::mat4 getInvViewMatrix() const;

   glm::mat4 getProjMatrix() const;
   void setNearAndFar(float near, float far);
   void setAspectRatio(float aspectRatio);

   void translateCameraLook(float d, bool dir);
   void translateCameraRight(float d, bool dir);
   void translateCameraWorldUp(float d, bool dir);
   void rotateCameraWorldUp(float d);
   void rotateCameraPerpLookUp(float d);

protected:
   glm::vec4 c_position;
   glm::vec4 c_look;
   glm::vec4 c_up;
   float c_heightAngle;

   float c_far;
   float c_near;
   float c_aspectRatio;

   glm::mat4 c_viewMatrix;
   glm::mat4 c_invViewMatrix;

   glm::mat4 c_projMatrix;

   glm::vec3 translate_c_position;

   glm::mat4 TranslationMatrix();
   glm::mat4 RotationMatrix();
   void ViewMatrix();

   glm::mat4 ScaleMatrix();
   glm::mat4 ParallelizationMatrix();
   void ProjectionMatrix();
};

#endif // CAMERA_H
