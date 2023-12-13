#pragma once

// Defined before including GLEW to suppress deprecation messages on macOS
#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION
#endif
#include <GL/glew.h>
#include <glm/glm.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QOpenGLWidget>
#include <QTime>
#include <QTimer>

#include "utils/sceneparser.h"
#include "utils/shaderloader.h"
#include "camera/camera.h"
//#include "shapes/Triangle.h"
#include "shapes/Cube.h"
#include "shapes/Sphere.h"
#include "shapes/Cylinder.h"
#include "shapes/Cone.h"
#include <algorithm>
#include "debug.h"

struct Shape {
    GLuint vao = 0;
    GLuint vbo = 0;
    int size = 0;
};

class Realtime : public QOpenGLWidget
{
public:
    Realtime(QWidget *parent = nullptr);
    void finish();                                      // Called on program exit
    void sceneChanged();
    void settingsChanged();
    void saveViewportImage(std::string filePath);

    // Skybox members
    GLuint skyboxVBO, skyboxVAO, skyboxEBO;
    GLuint cubemapTexture;
    void loadSkyboxTextures();
    void setupSkybox();
    void renderSkybox();
    GLuint m_skyboxShader;

public slots:
    void tick(QTimerEvent* event);                      // Called once per tick of m_timer

protected:
    void initializeGL() override;                       // Called once at the start of the program
    void paintGL() override;                            // Called whenever the OpenGL context changes or by an update() request
    void resizeGL(int width, int height) override;      // Called when window size changes

private:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void timerEvent(QTimerEvent *event) override;

    // Tick Related Variables
    int m_timer;                                        // Stores timer which attempts to run ~60 times per second
    QElapsedTimer m_elapsedTimer;                       // Stores timer which keeps track of actual time between frames

    // Input Related Variables
    bool m_mouseDown = false;                           // Stores state of left mouse button
    glm::vec2 m_prev_mouse_pos;                         // Stores mouse position
    std::unordered_map<Qt::Key, bool> m_keyMap;         // Stores whether keys are pressed or not

    // Device Correction Variables
    int m_devicePixelRatio;

    // Scene Data
    RenderData m_scenedata;

    // Camera
    Camera camera;

    // Shapes
    Cube *m_cube;
    Shape m_cube_s;
    Sphere *m_sphere;
    Shape m_sphere_s;
    Cylinder *m_cyl;
    Shape m_cyl_s;
    Cone *m_cone;
    Shape m_cone_s;

    void updateShapeParams(int param1, int param2);
    void createVBOVAO(GLuint &vbo, GLuint &vao, std::vector<GLfloat> data);
    void updateVBO();

    // Shaders
    GLuint m_shader;

    // Framebuffers
    GLuint m_texture_shader;
    GLuint m_fullscreen_vbo;
    GLuint m_fullscreen_vao;
    GLuint m_fbo;
    GLuint m_fbo_texture;
    GLuint m_fbo_renderbuffer;
    GLuint m_defaultFBO;
    int m_fbo_width;
    int m_fbo_height;
    int m_screen_width;
    int m_screen_height;
    void makeFBO();
    void paintTexture(GLuint texture);

    // Extra Credit
    bool m_tess_numObj = false;
    int m_tess_numObj_startParam = 5;
    bool m_tess_dist = false;
    int m_tess_dist_startParam = 10;
    bool m_tess_dist_shader = false;

    bool m_invert = false;
    bool m_gray = false;
    bool m_hueRotation = false;
    bool m_blur = false;
    bool m_sharpen = false;
    bool m_sobel = false;

    bool m_textureMapping = false;
    GLuint m_image_texture;
};
