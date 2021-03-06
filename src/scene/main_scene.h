#ifndef SRC_SCENE_MAIN_SCENE_H
#define SRC_SCENE_MAIN_SCENE_H

#include <vector>
#include "scene.hpp"

class PixelateFramebuffer;
namespace glcpp
{
    class Entity;
}
class MainScene : public Scene
{
public:
    MainScene() = delete;
    MainScene(uint32_t width, uint32_t height, std::shared_ptr<SharedResources> resources = nullptr);
    virtual ~MainScene() = default;
    virtual void init_framebuffer(uint32_t width, uint32_t height) override;
    virtual void pre_draw() override;
    virtual void draw() override;
    virtual glcpp::Entity *get_mutable_selected_entity() override;

private:
    void init_shader();
    void init_camera();
    void init_pixelate_framebuffer(uint32_t width, uint32_t height);
    void update_framebuffer();
    void set_view_and_projection();
    void draw_to_framebuffer();

    glm::vec4 background_color_{0.4f, 0.4f, 0.4f, 1.0f};

    std::shared_ptr<glcpp::Framebuffer> skybox_framebuffer_;
    std::shared_ptr<glcpp::Framebuffer> grid_framebuffer_;
    std::shared_ptr<PixelateFramebuffer> pixelate_framebuffer_;

    std::shared_ptr<glcpp::Entity> selected_entity_;
};

#endif