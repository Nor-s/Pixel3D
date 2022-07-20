#ifndef GLCPP_HEIGHTMAP_H
#define GLCPP_HEIGHTMAP_H

#include <glad/glad.h>

#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"
#include "component/transform_component.h"

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <array>

namespace glcpp
{
    // TODO: FIX hardcoding, cpu(current ver: Assume only receive textures of the same size)
    class Heightmap
    {
    public:
        Heightmap(const char *path);
        ~Heightmap();
        void set_rgba(const char *path);
        void set_target(const char *path);
        void set_texture(const char *path);

        TransformComponent &get_mutable_transform();
        bool &get_mutable_is_wireframe();
        bool &get_mutable_is_greyscale();
        float &get_mutable_y_scale();
        float &get_mutable_animation_speed();
        void draw(Shader *shader, const glm::mat4 &projection, const glm::mat4 &view);
        void reload();

    private:
        void init_mesh();
        void update_vert(float y_factor);
        void init_indices();
        void init_buffers();
        void set_texture(std::vector<unsigned char> &tex);

        std::string heightmap_path_;
        std::string texture_path_;
        float animation_speed_ = 0.0001f;
        float y_scale_factor_ = 1.0f;
        float delta_time_ = 0.0f;
        unsigned int current_texture_id_ = 0u;
        bool is_wireframe_ = false;
        bool is_greyscale_ = false;

        TransformComponent transform_component_{};
        std::shared_ptr<Shader> shader_;
        std::vector<float> current_vertices_;
        std::vector<float> target_vertices_;
        std::vector<float> uv_;
        std::vector<unsigned int> indices_;
        std::array<unsigned int, 2> texture_ = {0u, 0u};
        unsigned int terrain_VAO_ = 0U;
        unsigned int terrain_IBO_ = 0U;
        unsigned int terrain_VBO_ = 0U;
        unsigned int uv_VBO_ = 0U;
        unsigned int num_strips_ = 0;
        unsigned int num_tris_per_stripe_ = 0;
        int width_ = 256u;
        int height_ = 256u;
        int nr_channels_ = 0u;
        int rez_ = 1;
    };

}

#endif
