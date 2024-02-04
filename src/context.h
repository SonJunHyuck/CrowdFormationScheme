#pragma once

#include "common.h"
#include "shader.h"
#include "program.h"
#include "buffer.h"
#include "vertex_layout.h"
#include "texture.h"
#include "mesh.h"
#include "model.h"
#include "framebuffer.h"
#include "camera.h"

CLASS_PTR(Context)
class Context
{
public:
    static ContextUPtr Create();
    void Render();
    void ProcessInput(GLFWwindow* window);
    void Reshape(int width, int height);
    void MouseMove(double x, double y);
    void MouseButton(int button, int action, double x, double y);

private:
    Context() {}
    bool Init();
    ProgramUPtr m_program;
    ProgramUPtr m_simpleProgram;
    ProgramUPtr m_textureProgram;
    ProgramUPtr m_postProgram;
    ProgramUPtr m_skyboxProgram;
    ProgramUPtr m_envMapProgram;

    float m_gamma {1.0f};

    MeshUPtr m_box;
    MeshUPtr m_plane;

    MaterialPtr m_planeMaterial;
    MaterialPtr m_box1Material;
    MaterialPtr m_box2Material;
    // ModelUPtr m_model; // Backpack
    TexturePtr m_windowTexture;
    CubeTextureUPtr m_cubeTexture;

    // animation
    bool m_animation { true };

    // clear color
    glm::vec4 m_clearColor { glm::vec4(0.0f, 0.0f, 0.0f, 0.0f) };

    // light parameter
    struct Light
    {
        glm::vec3 position{glm::vec3(1.0f, 4.0f, 4.0f)};
        glm::vec3 direction{glm::vec3(-1.0f, -1.0f, -1.0f)};
        glm::vec2 cutoff{glm::vec2(120.0f, 5.0f)};
        float distance{128.0f};
        glm::vec3 ambient{glm::vec3(0.1f, 0.1f, 0.1f)};
        glm::vec3 diffuse{glm::vec3(0.8f, 0.8f, 0.8f)};
        glm::vec3 specular{glm::vec3(1.0f, 1.0f, 1.0f)};
    };
    Light m_light;
    bool m_flashLightMode { false };

    // light parameter
    glm::vec3 m_lightPos{glm::vec3(3.0f, 3.0f, 3.0f)};
    glm::vec3 m_lightColor{glm::vec3(1.0f, 1.0f, 1.0f)};
    glm::vec3 m_objectColor{glm::vec3(1.0f, 0.5f, 0.0f)};
    float m_ambientStrength{0.1f};
    float m_specularStrength { 0.5f };
    float m_specularShininess { 32.0f };

    // camera
    Camera m_camera;

    // framebuffer
    FramebufferUPtr m_framebuffer;

    // window
    int m_width { WINDOW_WIDTH };
    int m_height { WINDOW_HEIGHT };
};