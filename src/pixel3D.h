#ifndef SRC_PIXEL3D_H
#define SRC_PIXEL3D_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "glcpp/application.hpp"
#include <stb/stb_image.h>
#include "glcpp/camera.h"
#include "glcpp/window.h"
#include "scene/scene.hpp"
namespace fs = std::filesystem;

class Pixel3D : public glcpp::Application<Pixel3D>
{
    template <typename T>
    friend class glcpp::Application;

private:
    Pixel3D()
    {
    }

public:
    virtual ~Pixel3D()
    {
        scenes_.clear();
    }
    virtual void init(int width, int height, const std::string &title) override
    {
        stbi_set_flip_vertically_on_load(true);
        init_window(width, height, title);
        init_ui();
    }
    void init_window(int width, int height, const std::string &title)
    {
        window_ = std::make_unique<glcpp::Window>(width, height, title);
        window_->set_factor();
        window_->set_user_pointer(this);
        init_callback();
    }
    void init_callback()
    {
        window_->set_framebuffer_size_callback(framebuffer_size_callback);
        window_->set_scroll_callback(scroll_callback);
        window_->set_mouse_button_callback(mouse_btn_callback);
        window_->set_cursor_pos_callback(mouse_callback);
    }
    void init_ui()
    {
    }
    virtual void loop() override
    {
        glfwSwapInterval(0);
        float last_time = static_cast<float>(glfwGetTime());

        while (!window_->should_close())
        {
            float current_time = static_cast<float>(glfwGetTime());
            frames_++;
            if (current_time - last_time >= 1.0)
            {
                fps_ = frames_ / (current_time - last_time);
                frames_ = 0;
                last_time = current_time;
            }
            delta_frame_ = current_time - last_frame_;
            last_frame_ = current_time;

            processInput(window_->get_handle());

            pre_draw();
            {
            }
            post_draw();
        }
    }
    void pre_draw()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, window_->get_width(), window_->get_height());
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    void post_draw()
    {
        // glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glfwSwapBuffers(window_->get_handle());
        glfwPollEvents();
    }

    void processInput(GLFWwindow *window)
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            scenes_[current_scene_idx_]->get_camera()->process_keyboard(glcpp::FORWARD, delta_frame_);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            scenes_[current_scene_idx_]->get_camera()->process_keyboard(glcpp::BACKWARD, delta_frame_);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            scenes_[current_scene_idx_]->get_camera()->process_keyboard(glcpp::LEFT, delta_frame_);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            scenes_[current_scene_idx_]->get_camera()->process_keyboard(glcpp::RIGHT, delta_frame_);
    }

    static void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));

        app->window_->update_window();
    }

    static void mouse_callback(GLFWwindow *window, double xposIn, double yposIn)
    {
        auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
        if (app->is_pressed_)
        {
            app->scenes_[app->current_scene_idx_]->get_camera()->process_mouse_movement((static_cast<float>(yposIn) - app->prev_mouse_.y) / 3.6f, (static_cast<float>(xposIn) - app->prev_mouse_.x) / 3.6f);
            app->prev_mouse_.x = xposIn;
            app->prev_mouse_.y = yposIn;
        }
        if (app->is_pressed_scroll_)
        {
            app->scenes_[app->current_scene_idx_]->get_camera()->process_mouse_scroll_press((static_cast<float>(yposIn) - app->prev_mouse_.y), (static_cast<float>(xposIn) - app->prev_mouse_.x), app->delta_frame_);
            app->prev_mouse_.x = xposIn;
            app->prev_mouse_.y = yposIn;
        }
        app->cur_mouse_.x = xposIn;
        app->cur_mouse_.y = yposIn;
    }
    static void mouse_btn_callback(GLFWwindow *window, int button, int action, int mods)
    {
        // auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
        // if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && app->imgui_->is_window_hovered("scene" + std::to_string(app->current_scene_idx_ + 1)))
        // {
        //     app->prev_mouse_.x = app->cur_mouse_.x;
        //     app->prev_mouse_.y = app->cur_mouse_.y;
        //     app->is_pressed_ = true;
        // }
        // else
        // {
        //     app->is_pressed_ = false;
        // }
        // if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && app->imgui_->is_window_hovered("scene" + std::to_string(app->current_scene_idx_ + 1)))
        // {
        //     app->prev_mouse_.x = app->cur_mouse_.x;
        //     app->prev_mouse_.y = app->cur_mouse_.y;
        //     app->is_pressed_scroll_ = true;
        // }
        // else
        // {
        //     app->is_pressed_scroll_ = false;
        // }
    }

    static void scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
    {
        // auto app = reinterpret_cast<Pixel3D *>(glfwGetWindowUserPointer(window));
        // if (app->scenes_[app->current_scene_idx_] && app->imgui_ && app->imgui_->is_window_hovered("scene" + std::to_string(app->current_scene_idx_ + 1)))
        //     app->scenes_[app->current_scene_idx_]->get_camera()->process_mouse_scroll(yoffset);
    }

private:
    // timing
    float delta_frame_ = 0.0f;
    float last_frame_ = 0.0f;
    float fps_ = 0.0f;
    int frames_ = 0;
    // mouse event
    bool is_pressed_ = false;
    bool is_pressed_scroll_ = false;
    glm::vec2 prev_mouse_{-1.0f, -1.0f}, cur_mouse_{-1.0f, -1.0f};
    std::vector<std::shared_ptr<Scene>> scenes_;
    int current_scene_idx_ = 0;
};

#endif
