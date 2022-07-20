#ifndef GLCPP_HEIGHTMAP_H
#define GLCPP_HEIGHTMAP_H

#include <glad/glad.h>

#include "stb/stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "shader.h"

#include <iostream>
#include <vector>
#include <memory>

namespace glcpp
{
    // TODO: FIX hardcoding, cpu(current ver: Assume only receive textures of the same size)
    class Heightmap
    {
    public:
        Heightmap(const char *path);
        ~Heightmap();
        void set_target(const char *path);
        void set_texture(const char *path);
        void draw(Shader *shader, const glm::mat4 &projection, const glm::mat4 &view);

    private:
        void update_vert(float y_factor);
        void init_indices();
        void init_buffers();

        std::shared_ptr<Shader> shader_;
        std::vector<float> current_vertices_;
        std::vector<float> target_vertices_;
        std::vector<float> new_vec_;
        std::vector<float> uv_;
        std::vector<unsigned int> indices_;
        unsigned int texture_ = 0U;
        unsigned int terrain_VAO_ = 0U;
        unsigned int terrain_IBO_ = 0U;
        unsigned int terrain_VBO_ = 0U;
        unsigned int uv_VBO_ = 0U;
        unsigned int num_strips_ = 0;
        unsigned int num_tris_per_stripe_ = 0;
        int width_ = 0u;
        int height_ = 0u;
        int nr_channels_ = 0u;
        int rez_ = 1;
    };

}

#endif
