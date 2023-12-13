#include "realtime.h"
#include <glm/gtc/type_ptr.hpp>
#include <QCoreApplication>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>
#include "settings.h"

// ================== Project 5: Lights, Camera

Realtime::Realtime(QWidget *parent)
    : QOpenGLWidget(parent), camera(Camera(m_scenedata.cameraData, settings.nearPlane, settings.farPlane, ((float)size().width())/((float)size().height())))
{
    m_prev_mouse_pos = glm::vec2(size().width()/2, size().height()/2);
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);

    m_keyMap[Qt::Key_W]       = false;
    m_keyMap[Qt::Key_A]       = false;
    m_keyMap[Qt::Key_S]       = false;
    m_keyMap[Qt::Key_D]       = false;
    m_keyMap[Qt::Key_Control] = false;
    m_keyMap[Qt::Key_Space]   = false;

    // If you must use this function, do not edit anything above this
    m_cube = new Cube();
    m_sphere = new Sphere();
    m_cyl = new Cylinder();
    m_cone = new Cone();
    updateShapeParams(settings.shapeParameter1, settings.shapeParameter2);
}

void Realtime::loadSkyboxTextures() {

    std::string facesCubemap[6] = {
        ":/resources/skybox/skybox_right.png",  // Right (+X)
        ":/resources/skybox/skybox_left.png",   // Left (-X)
        ":/resources/skybox/skybox_up.png",     // Top (+Y)
        ":/resources/skybox/skybox_down.png",   // Bottom (-Y)
        ":/resources/skybox/skybox_front.png",  // Front (+Z)
        ":/resources/skybox/skybox_back.png"    // Back (-Z)
    };

    // Generate and bind the texture object
    glGenTextures(1, &cubemapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);

    // Load each texture face
    for (unsigned int i = 0; i < 6; i++) {
        QImage image;
        if (image.load(QString::fromStdString(facesCubemap[i]))) {
            // Convert the image to the appropriate format and load it into the cubemap
            image = image.convertToFormat(QImage::Format_RGB888).mirrored(true, false);
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, image.width(), image.height(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.bits());
        } else {
            std::cerr << "Failed to load texture: " << facesCubemap[i] << std::endl;
        }
    }


    // Set texture parameters
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // Unbind the texture
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void Realtime::setupSkybox() {
    float skyboxVertices[] = {

        //   Coordinates
        -1.0f, -1.0f,  1.0f,//        7--------6
        1.0f, -1.0f,  1.0f,//       /|       /|
        1.0f, -1.0f, -1.0f,//      4--------5 |
        -1.0f, -1.0f, -1.0f,//      | |      | |
        -1.0f,  1.0f,  1.0f,//      | 3------|-2
        1.0f,  1.0f,  1.0f,//      |/       |/
        1.0f,  1.0f, -1.0f,//      0--------1
        -1.0f,  1.0f, -1.0f
    };


    unsigned int skyboxIndices[] = {
        // indices for each triangle of each face
        1, 5, 6, 6, 2, 1, // right face
        0, 3, 7, 7, 4, 0, // left face
        4, 7, 6, 6, 5, 4, // top face
        0, 1, 2, 2, 3, 0, // bottom face
        3, 2, 6, 6, 7, 3, // front face
        0, 4, 5, 5, 1, 0, // back face
    };

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), skyboxIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0); // Unbind VAO
}

void Realtime::renderSkybox() {
    glDepthFunc(GL_LEQUAL);
    glUseProgram(m_skyboxShader); // Use m_skyboxShader for rendering skybox

    glm::mat4 view = glm::mat4(glm::mat3(camera.getViewMatrix()));
    glm::mat4 projection = camera.getProjMatrix();

    // Set uniforms for view and projection matrices
    GLuint viewLoc = glGetUniformLocation(m_skyboxShader, "view");
    GLuint projLoc = glGetUniformLocation(m_skyboxShader, "projection");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

    glBindVertexArray(skyboxVAO);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glUseProgram(0); // Unbind shader program
    glDepthFunc(GL_LESS);
}


void Realtime::updateShapeParams(int param1, int param2) {
    if (m_tess_numObj) {
        int numShapes = m_scenedata.shapes.size();
        param1 = std::max((m_tess_numObj_startParam - numShapes), 0) + param1;
        param2 = std::max((m_tess_numObj_startParam - numShapes), 0) + param2;
    }

    m_cube->updateParams(param1);
    m_sphere->updateParams(param1, param2);
    m_cyl->updateParams(param1, param2);
    m_cone->updateParams(param1, param2);
}


void Realtime::finish() {
    killTimer(m_timer);
    this->makeCurrent();

    // Students: anything requiring OpenGL calls when the program exits should be done here
    glDeleteBuffers(1, &m_cube_s.vbo);
    glDeleteVertexArrays(1, &m_cube_s.vao);
    glDeleteBuffers(1, &m_sphere_s.vbo);
    glDeleteVertexArrays(1, &m_sphere_s.vao);
    glDeleteBuffers(1, &m_cone_s.vbo);
    glDeleteVertexArrays(1, &m_cone_s.vao);
    glDeleteBuffers(1, &m_cyl_s.vbo);
    glDeleteVertexArrays(1, &m_cyl_s.vao);
    glDeleteProgram(m_shader);

    // Framebuffers
    glDeleteProgram(m_texture_shader);
    glDeleteBuffers(1, &m_fullscreen_vbo);
    glDeleteVertexArrays(1, &m_fullscreen_vao);
    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    //skybox
    glDeleteProgram(m_skyboxShader);

    this->doneCurrent();
}

void Realtime::createVBOVAO(GLuint &vbo, GLuint &vao, std::vector<GLfloat> data) {

    // Generate and bind VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Send data to VBO
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

    // Generate, and bind vao
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Enable and define attribute 0 to store vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLuint), reinterpret_cast<void*>(0));

    // Enable and define attribute 1 to store vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GLuint), reinterpret_cast<void*>(3*sizeof(GLfloat)));

    // Enable and define attribute 2 to store vertex uv
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GLuint), reinterpret_cast<void*>(6*sizeof(GLfloat)));

    // Clean-up bindings
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Realtime::initializeGL() {
    m_devicePixelRatio = this->devicePixelRatio();

    m_timer = startTimer(1000/60);
    m_elapsedTimer.start();

    m_defaultFBO = 2; // default framebuffer value
    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;

    // Initializing GL.
    // GLEW (GL Extension Wrangler) provides access to OpenGL functions.
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        std::cerr << "Error while initializing GL: " << glewGetErrorString(err) << std::endl;
    }
    std::cout << "Initialized GL: Version " << glewGetString(GLEW_VERSION) << std::endl;

    // Allows OpenGL to draw objects appropriately on top of one another
    glEnable(GL_DEPTH_TEST);
    // Tells OpenGL to only draw the front face
    glEnable(GL_CULL_FACE);
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    m_skyboxShader = ShaderLoader::createShaderProgram(":/resources/shaders/skybox.vert", ":/resources/shaders/skybox.frag");
    loadSkyboxTextures();
    setupSkybox();
    if (!m_tess_dist_shader) {
        m_shader = ShaderLoader::createShaderProgram(":/resources/shaders/default.vert", ":/resources/shaders/default.frag");
    } else {
        m_shader = ShaderLoader::createShaderProgramEC(":/resources/shaders/default_tes.vert", ":/resources/shaders/default_tes.tcs", ":/resources/shaders/default_tes.tes", ":/resources/shaders/default.frag");
        glPatchParameteri(GL_PATCH_VERTICES, 3);
    }
    m_texture_shader = ShaderLoader::createShaderProgram(":/resources/shaders/texture.vert",":/resources/shaders/texture.frag");
    Debug::glErrorCheck();

    std::vector<float> cube = m_cube->generateShape();
    m_cube_s.size = cube.size();
    createVBOVAO(m_cube_s.vbo, m_cube_s.vao, cube);

    std::vector<float> sphere = m_sphere->generateShape();
    m_sphere_s.size = sphere.size();
    createVBOVAO(m_sphere_s.vbo, m_sphere_s.vao, sphere);

    std::vector<float> cyl = m_cyl->generateShape();
    m_cyl_s.size = cyl.size();
    createVBOVAO(m_cyl_s.vbo, m_cyl_s.vao, cyl);

    std::vector<float> cone = m_cone->generateShape();
    m_cone_s.size = cone.size();
    createVBOVAO(m_cone_s.vbo, m_cone_s.vao, cone);

    // Generate image texture
    glGenTextures(1, &m_image_texture);

    glUseProgram(m_shader);
    glUniform1i(glGetUniformLocation(m_shader, "tex"), 0);
    glUseProgram(0);

    glUseProgram(m_texture_shader);
    glUniform1i(glGetUniformLocation(m_texture_shader, "tex"), 0);
    glUseProgram(0);

    std::vector<GLfloat> fullscreen_quad_data =
        { //     POSITIONS    //
         -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
         -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         };

    glGenBuffers(1, &m_fullscreen_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_fullscreen_vbo);
    glBufferData(GL_ARRAY_BUFFER, fullscreen_quad_data.size()*sizeof(GLfloat), fullscreen_quad_data.data(), GL_STATIC_DRAW);
    glGenVertexArrays(1, &m_fullscreen_vao);
    glBindVertexArray(m_fullscreen_vao);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), reinterpret_cast<void*>(3 * sizeof(GLfloat)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    makeFBO();
}

void Realtime::makeFBO(){
    // Generate and bind an empty texture, set its min/mag filter interpolation, then unbind
    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &m_fbo_texture);
    glBindTexture(GL_TEXTURE_2D, m_fbo_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_fbo_width, m_fbo_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    // Generate and bind a renderbuffer of the right size, set its format, then unbind
    glGenRenderbuffers(1, &m_fbo_renderbuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, m_fbo_renderbuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8 , m_fbo_width, m_fbo_height);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);

    // Generate and bind an FBO
    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    // Add our texture as a color attachment, and our renderbuffer as a depth+stencil attachment, to our FBO
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fbo_texture, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_fbo_renderbuffer);

    // Unbind the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
}

void Realtime::paintGL() {
    // Students: anything requiring OpenGL calls every frame should be done here

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glViewport(0,0, m_fbo_width, m_fbo_height);

    // Clear screen color and depth before painting
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render the skybox first
    glDepthMask(GL_FALSE);  // Disable depth write
    renderSkybox();
    glDepthMask(GL_TRUE);   // Re-enable depth write

    // activate the shader program by calling glUseProgram with `m_shader`
    glUseProgram(m_shader);

    // Pass in view and projection matrix
    auto camera_pos = camera.getInvViewMatrix() * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "view_matrix"), 1, GL_FALSE, &camera.getViewMatrix()[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(m_shader, "proj_matrix"), 1, GL_FALSE, &camera.getProjMatrix()[0][0]);
    glUniform4fv(glGetUniformLocation(m_shader, "cam_pos"), 1, &camera_pos[0]);

    // Pass in extra credit
    if (m_tess_dist_shader) {
        glUniform1f(glGetUniformLocation(m_shader, "tesParam"), 2.0f);
    }

    Debug::glErrorCheck();

    // Pass in lighting constants
    glUniform1f(glGetUniformLocation(m_shader, "ka"), m_scenedata.globalData.ka);
    glUniform1f(glGetUniformLocation(m_shader, "kd"), m_scenedata.globalData.kd);
    glUniform1f(glGetUniformLocation(m_shader, "ks"), m_scenedata.globalData.ks);
    int num_lights = m_scenedata.lights.size() > 8 ? 8 : m_scenedata.lights.size();
    glUniform1i(glGetUniformLocation(m_shader, "num_lights"), num_lights);
    glUniform1i(glGetUniformLocation(m_shader, "textureMapping"), m_textureMapping);

    // Pass Lights to Shader
    for (int i = 0; i < num_lights; i++){
        auto light = m_scenedata.lights[i];

        glUniform1i(glGetUniformLocation(m_shader, ("lightType[" + std::to_string(i) + "]").c_str()), (int) light.type);
        glUniform4fv(glGetUniformLocation(m_shader, ("lightColor[" + std::to_string(i) + "]").c_str()),1, &light.color[0]);
        glUniform3fv(glGetUniformLocation(m_shader, ("lightFunction[" + std::to_string(i) + "]").c_str()),1, &light.function[0]);
        glUniform4fv(glGetUniformLocation(m_shader, ("lightPos[" + std::to_string(i) + "]").c_str()), 1, &light.pos[0]);
        glUniform4fv(glGetUniformLocation(m_shader, ("lightDir[" + std::to_string(i) + "]").c_str()), 1, &light.dir[0]);
        glUniform1f(glGetUniformLocation(m_shader, ("lightPenumbra[" + std::to_string(i) + "]").c_str()), light.penumbra);
        glUniform1f(glGetUniformLocation(m_shader, ("lightAngle[" + std::to_string(i) + "]").c_str()), light.angle);
    }

    Debug::glErrorCheck();

    // Pass Shapes to Shader
    for (int i = 0; i < m_scenedata.shapes.size(); i++) {
        auto shape = m_scenedata.shapes[i];

        if (m_tess_dist) {
            auto centerObject = shape.ctm * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // find the center of object in world space
            auto dist = glm::length(camera_pos - centerObject);

            updateShapeParams(std::max((int) floor(m_tess_dist_startParam - dist), 0) + settings.shapeParameter1, std::max((int) floor(m_tess_dist_startParam - dist), 0) + settings.shapeParameter2);
            updateVBO();
        }

        glUniformMatrix4fv(glGetUniformLocation(m_shader, "model_matrix"), 1, GL_FALSE, &shape.ctm[0][0]);
        auto convertONormaltoWNormal = glm::inverse(glm::transpose(glm::mat3(shape.ctm)));
        glUniformMatrix3fv(glGetUniformLocation(m_shader, "inv_t_model_matrix"), 1, GL_FALSE, &convertONormaltoWNormal[0][0]);

        glUniform4fv(glGetUniformLocation(m_shader, "ambient_c"), 1, &(shape.primitive.material.cAmbient[0]));
        glUniform4fv(glGetUniformLocation(m_shader, "diffuse_c"), 1, &(shape.primitive.material.cDiffuse[0]));
        glUniform4fv(glGetUniformLocation(m_shader, "specular_c"), 1, &(shape.primitive.material.cSpecular[0]));
        glUniform1f(glGetUniformLocation(m_shader, "n"), shape.primitive.material.shininess);
        glUniform1i(glGetUniformLocation(m_shader, "objTextureMap"), shape.primitive.material.textureMap.isUsed);
        glUniform1f(glGetUniformLocation(m_shader, "blend"), shape.primitive.material.blend);

        if (m_textureMapping && shape.primitive.material.textureMap.isUsed) {
            QImage img;
            if (!img.load(QString::fromStdString(shape.primitive.material.textureMap.filename))) {
                throw std::runtime_error("texture file could not be loaded");
            }
            img = img.convertToFormat(QImage::Format_RGBX8888).mirrored();
            // Set the active texture slot to texture slot 0
            glActiveTexture(GL_TEXTURE0);
            // Bind image texture
            glBindTexture(GL_TEXTURE_2D, m_image_texture);
            // Load image into image texture
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.width(), img.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, img.bits());
            // Set min and mag filters' interpolation mode to linear
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        Debug::glErrorCheck();

        switch(shape.primitive.type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            glBindVertexArray(m_cube_s.vao);
            Debug::glErrorCheck();
            if (!m_tess_dist_shader) {
                glDrawArrays(GL_TRIANGLES, 0, m_cube_s.size / 8);
            } else {
                glDrawArrays(GL_PATCHES, 0, m_cube_s.size / 8);
            }
            Debug::glErrorCheck();
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            glBindVertexArray(m_sphere_s.vao);
            Debug::glErrorCheck();
            if (!m_tess_dist_shader) {
                glDrawArrays(GL_TRIANGLES, 0, m_sphere_s.size / 8);
            } else {
                glDrawArrays(GL_PATCHES, 0, m_sphere_s.size / 8);
            }
            Debug::glErrorCheck();
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            glBindVertexArray(m_cyl_s.vao);
            Debug::glErrorCheck();
            if (!m_tess_dist_shader) {
                glDrawArrays(GL_TRIANGLES, 0, m_cyl_s.size / 8);
            } else {
                glDrawArrays(GL_PATCHES, 0, m_cyl_s.size / 8);
            }
            Debug::glErrorCheck();
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            glBindVertexArray(m_cone_s.vao);
            Debug::glErrorCheck();
            if (!m_tess_dist_shader) {
                glDrawArrays(GL_TRIANGLES, 0, m_cone_s.size / 8);
            } else {
                glDrawArrays(GL_PATCHES, 0, m_cone_s.size / 8);
            }
            Debug::glErrorCheck();
            break;
        default:
            break;
        }

        if (m_textureMapping && shape.primitive.material.textureMap.isUsed) {
            // Unbind image texture
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        // Unbind Vertex Array
        glBindVertexArray(0);
    }

    // deactivate the shader program
    glUseProgram(0);

    glBindFramebuffer(GL_FRAMEBUFFER, m_defaultFBO);
    glViewport(0,0, m_screen_width, m_screen_height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintTexture(m_fbo_texture);
}

void Realtime::paintTexture(GLuint texture){
    glUseProgram(m_texture_shader);

    glUniform1i(glGetUniformLocation(m_texture_shader, "inv"), m_invert);
    glUniform1i(glGetUniformLocation(m_texture_shader, "gray"), m_gray);
    glUniform1i(glGetUniformLocation(m_texture_shader, "hueRotation"), m_hueRotation);
    glUniform1i(glGetUniformLocation(m_texture_shader, "blur"), m_blur);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sharpen"), m_sharpen);
    glUniform1i(glGetUniformLocation(m_texture_shader, "sobel"), m_sobel);
    glUniform1f(glGetUniformLocation(m_texture_shader, "height"), float(m_screen_height));
    glUniform1f(glGetUniformLocation(m_texture_shader, "width"), float(m_screen_width));

    glBindVertexArray(m_fullscreen_vao);
    glBindTexture(GL_TEXTURE_2D, texture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
}

void Realtime::resizeGL(int w, int h) {
    // Tells OpenGL how big the screen is
    glViewport(0, 0, size().width() * m_devicePixelRatio, size().height() * m_devicePixelRatio);

    // Students: anything requiring OpenGL calls when the program starts should be done here
    camera.setAspectRatio(((float)size().width())/((float)size().height()));

    glDeleteTextures(1, &m_fbo_texture);
    glDeleteRenderbuffers(1, &m_fbo_renderbuffer);
    glDeleteFramebuffers(1, &m_fbo);

    m_screen_width = size().width() * m_devicePixelRatio;
    m_screen_height = size().height() * m_devicePixelRatio;
    m_fbo_width = m_screen_width;
    m_fbo_height = m_screen_height;
    makeFBO();
}

void Realtime:: updateVBO() {
    if (m_cube_s.vbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_cube_s.vbo);
        std::vector<GLfloat> cube = m_cube->generateShape();
        m_cube_s.size = cube.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cube.size(), cube.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_sphere_s.vbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_sphere_s.vbo);
        std::vector<GLfloat> sphere = m_sphere->generateShape();
        m_sphere_s.size = sphere.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * sphere.size(), sphere.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_cyl_s.vbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_cyl_s.vbo);
        std::vector<GLfloat> cyl = m_cyl->generateShape();
        m_cyl_s.size = cyl.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cyl.size(), cyl.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (m_cone_s.vbo != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, m_cone_s.vbo);
        std::vector<GLfloat> cone = m_cone->generateShape();
        m_cone_s.size = cone.size();
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * cone.size(), cone.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

void Realtime::sceneChanged() {
    if (!SceneParser::parse(settings.sceneFilePath, m_scenedata)) {
        std::cerr << "Error Loading Scene: " << settings.sceneFilePath << std::endl;
    }

    camera = Camera(m_scenedata.cameraData, settings.nearPlane, settings.farPlane, ((float)size().width())/((float)size().height()));
    updateShapeParams(settings.shapeParameter1, settings.shapeParameter2);
    updateVBO();
    update(); // asks for a PaintGL() call to occur
}

void Realtime::settingsChanged() {
//    m_tess_numObj = settings.extraCredit1;
//    m_tess_dist = settings.extraCredit2;
//    m_invert = settings.perPixelFilter;
//    m_gray = settings.extraCredit1;
//    m_hueRotation = settings.extraCredit2;
//    m_blur = settings.kernelBasedFilter;
//    m_sharpen = settings.extraCredit3;
//    m_sobel = settings.extraCredit3;
//    m_textureMapping = settings.extraCredit4;

    camera.setNearAndFar(settings.nearPlane, settings.farPlane);
    camera.setAspectRatio(((float)size().width())/((float)size().height()));
    updateShapeParams(settings.shapeParameter1, settings.shapeParameter2);
    updateVBO();

    update(); // asks for a PaintGL() call to occur
}

// ================== Project 6: Action!

void Realtime::keyPressEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = true;
}

void Realtime::keyReleaseEvent(QKeyEvent *event) {
    m_keyMap[Qt::Key(event->key())] = false;
}

void Realtime::mousePressEvent(QMouseEvent *event) {
    if (event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = true;
        m_prev_mouse_pos = glm::vec2(event->position().x(), event->position().y());
    }
}

void Realtime::mouseReleaseEvent(QMouseEvent *event) {
    if (!event->buttons().testFlag(Qt::LeftButton)) {
        m_mouseDown = false;
    }
}

void Realtime::mouseMoveEvent(QMouseEvent *event) {
    if (m_mouseDown) {
        int posX = event->position().x();
        int posY = event->position().y();
        int deltaX = posX - m_prev_mouse_pos.x;
        int deltaY = posY - m_prev_mouse_pos.y;
        m_prev_mouse_pos = glm::vec2(posX, posY);

        // Use deltaX and deltaY here to rotate
        if(deltaX != 0){
            camera.rotateCameraWorldUp(-float(deltaX)/10.0f);
        }
        if(deltaY != 0){
            camera.rotateCameraPerpLookUp(-float(deltaY)/10.0f);
        }

        update(); // asks for a PaintGL() call to occur
    }
}

void Realtime::timerEvent(QTimerEvent *event) {
    int elapsedms   = m_elapsedTimer.elapsed();
    float deltaTime = elapsedms * 0.001f;
    m_elapsedTimer.restart();

    // Use deltaTime and m_keyMap here to move around
    if(m_keyMap[Qt::Key_W] == true) {
        camera.translateCameraLook(5*deltaTime, true);
    }
    if(m_keyMap[Qt::Key_S] == true) {
        camera.translateCameraLook(5*deltaTime, false);
    }
    if(m_keyMap[Qt::Key_A] == true) {
        camera.translateCameraRight(5*deltaTime, false);
    }
    if(m_keyMap[Qt::Key_D] == true) {
        camera.translateCameraRight(5*deltaTime, true);
    }
    if(m_keyMap[Qt::Key_Space] == true) {
        camera.translateCameraWorldUp(5*deltaTime, true);
    }
    if(m_keyMap[Qt::Key_Control] == true) {
        camera.translateCameraWorldUp(5*deltaTime, false);
    }

    update(); // asks for a PaintGL() call to occur
}

// DO NOT EDIT
void Realtime::saveViewportImage(std::string filePath) {
    // Make sure we have the right context and everything has been drawn
    makeCurrent();

    int fixedWidth = 1024;
    int fixedHeight = 768;

    // Create Frame Buffer
    GLuint fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create a color attachment texture
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fixedWidth, fixedHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texture, 0);

    // Optional: Create a depth buffer if your rendering uses depth testing
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, fixedWidth, fixedHeight);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Error: Framebuffer is not complete!" << std::endl;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return;
    }

    // Render to the FBO
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, fixedWidth, fixedHeight);

    // Clear and render your scene here
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    paintGL();

    // Read pixels from framebuffer
    std::vector<unsigned char> pixels(fixedWidth * fixedHeight * 3);
    glReadPixels(0, 0, fixedWidth, fixedHeight, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    // Unbind the framebuffer to return to default rendering to the screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Convert to QImage
    QImage image(pixels.data(), fixedWidth, fixedHeight, QImage::Format_RGB888);
    QImage flippedImage = image.mirrored(); // Flip the image vertically

    // Save to file using Qt
    QString qFilePath = QString::fromStdString(filePath);
    if (!flippedImage.save(qFilePath)) {
        std::cerr << "Failed to save image to " << filePath << std::endl;
    }

    // Clean up
    glDeleteTextures(1, &texture);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteFramebuffers(1, &fbo);
}
