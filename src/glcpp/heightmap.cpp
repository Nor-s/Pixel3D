#include "heightmap.h"

namespace glcpp
{
    Heightmap::Heightmap(const char *path)
        : heightmap_path_(path)
    {
        set_rgba("./result.png");
    }

    Heightmap::~Heightmap()
    {
        delete_buff();
    }

    void Heightmap::delete_buff()
    {
        glDeleteVertexArrays(1, &terrain_VAO_);
        glDeleteBuffers(1, &terrain_VBO_);
        glDeleteBuffers(1, &terrain_IBO_);
        glDeleteBuffers(1, &uv_VBO_);
    }
    void Heightmap::init_mesh()
    {
        std::cout << "INIT_MESH\n";
        current_vertices_.clear();
        uv_.clear();
        for (int i = 0; i < height_; i++)
        {
            for (int j = 0; j < width_; j++)
            {
                // vertex
                current_vertices_.push_back(-height_ / 2.0f + height_ * i / (float)height_); // vx
                current_vertices_.push_back(0);                                              // vy
                current_vertices_.push_back(-width_ / 2.0f + width_ * j / (float)width_);    // vz

                // uv
                uv_.push_back(j / static_cast<float>(width_));
                uv_.push_back(i / static_cast<float>(height_));
            }
        }
    }
    void Heightmap::reload()
    {
        current_vertices_ = target_vertices_;
        set_rgba("./result.png");
        delta_time_ = 0.0f;
    }
    void Heightmap::reload_animation()
    {
        init_mesh();
        delta_time_ = 0.0f;
    }
    void Heightmap::set_rgba(const char *path)
    {
        static unsigned int call_counter = 0u;
        call_counter++;
        current_texture_id_ = (current_texture_id_ + 1) % 2;
        target_vertices_.clear();
        std::vector<unsigned char> texture;

        stbi_set_flip_vertically_on_load(true);
        int width, height;
        unsigned char *data = stbi_load(path, &width, &height, &nr_channels_, 0);
        bool is_updated = false;
        if (width != width_ || height != height_)
        {
            std::cout << "width: " << width << " , height: " << height << " " << nr_channels_ << std::endl;
            width_ = width;
            height_ = height;
            if (call_counter > 0u)
            {
                delete_buff();
            }
            init_mesh();
            is_updated = true;
        }
        width_ = width;
        height_ = height;
        float yScale = (64.0f / 256.0f), yShift = 16.0f;
        unsigned bytePerPixel = nr_channels_;
        assert(nr_channels_ == 4);

        for (int i = 0; i < height; i++)
        {
            for (int j = 0; j < width; j++)
            {
                unsigned char *pixelOffset = data + (j + width * i) * bytePerPixel;
                unsigned char r = pixelOffset[0];
                unsigned char g = pixelOffset[1];
                unsigned char b = pixelOffset[2];
                unsigned char a = pixelOffset[3];

                // vertex
                target_vertices_.push_back(-height / 2.0f + height * i / (float)height); // vx
                target_vertices_.push_back((int)a * yScale - yShift);                    // vy
                target_vertices_.push_back(-width / 2.0f + width * j / (float)width);    // vz

                texture.push_back(r);
                texture.push_back(g);
                texture.push_back(b);
            }
        }
        set_texture(texture);
        if (is_updated)
        {
            init_indices();
            init_buffers();
        }
    }

    void Heightmap::update_vert(float y_factor)
    {
        std::vector<float> new_v = current_vertices_;
        for (int i = 0; i < current_vertices_.size(); i += 3)
        {
            int idx = i + 1;
            new_v[idx] = (1 - y_factor) * current_vertices_[idx] + y_factor * target_vertices_[idx];
            // current_vertices_[idx] = (1 - y_factor) * current_vertices_[idx] + y_factor * target_vertices_[idx];
        }
        glBindBuffer(GL_ARRAY_BUFFER, terrain_VBO_);
        // glBufferSubData(GL_ARRAY_BUFFER, 0, current_vertices_.size() * sizeof(float), &current_vertices_[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, new_v.size() * sizeof(float), &new_v[0]);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void Heightmap::init_indices()
    {
        for (unsigned i = 0; i < height_ - 1; i += rez_)
        {
            for (unsigned j = 0; j < width_; j += rez_)
            {
                for (unsigned k = 0; k < 2; k++)
                {
                    indices_.push_back(j + width_ * (i + k * rez_));
                }
            }
        }
        std::cout << "Loaded " << indices_.size() << " indices_" << std::endl;
        num_strips_ = (height_ - 1) / rez_;
        num_tris_per_stripe_ = (width_ / rez_) * 2; // - 2;
        std::cout << "Created lattice of " << num_strips_ << " strips with " << num_tris_per_stripe_ << " triangles each" << std::endl;
        std::cout << "Created " << num_strips_ * num_tris_per_stripe_ << " triangles total" << std::endl;
    }

    void Heightmap::init_buffers()
    {
        std::cout << "init buffer\n";
        glGenVertexArrays(1, &terrain_VAO_);
        glGenBuffers(1, &terrain_VBO_);
        glGenBuffers(1, &terrain_IBO_);
        glGenBuffers(1, &uv_VBO_);

        glBindVertexArray(terrain_VAO_);

        // position attribute
        glBindBuffer(GL_ARRAY_BUFFER, terrain_VBO_);
        glBufferData(GL_ARRAY_BUFFER, current_vertices_.size() * sizeof(float), &current_vertices_[0], GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        // uv
        glBindBuffer(GL_ARRAY_BUFFER, uv_VBO_);
        glBufferData(GL_ARRAY_BUFFER, uv_.size() * sizeof(float), &uv_[0], GL_STATIC_DRAW);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(2);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrain_IBO_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

    void Heightmap::set_texture(const char *path)
    {
        texture_path_ = std::string(path);
        glGenTextures(1, &texture_[current_texture_id_]);
        glBindTexture(GL_TEXTURE_2D, texture_[current_texture_id_]);

        // set the texture wrapping/filtering options (on the currently bound texture object)

        // load and generate the texture
        int width, height, nr_channels_;
        unsigned char *data = stbi_load(path, &width, &height, &nr_channels_, STBI_rgb);
        std::cout << "Texture: " << nr_channels_ << "\n";
        if (data)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use "GL_LINEAR" instead
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D); // Remove
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        stbi_image_free(data);
    }
    void Heightmap::set_texture(std::vector<unsigned char> &tex)
    {
        static unsigned int call_count = 0;
        call_count++;
        if (call_count >= 2)
        {
            glDeleteTextures(1, &texture_[current_texture_id_]);
        }
        glGenTextures(1, &texture_[current_texture_id_]);
        glBindTexture(GL_TEXTURE_2D, texture_[current_texture_id_]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Use "GL_LINEAR" instead
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width_, height_, 0, GL_RGB, GL_UNSIGNED_BYTE, &tex[0]);
        glGenerateMipmap(GL_TEXTURE_2D); // Remove
    }

    TransformComponent &Heightmap::get_mutable_transform()
    {
        return transform_component_;
    }
    bool &Heightmap::get_mutable_is_wireframe()
    {
        return is_wireframe_;
    }
    bool &Heightmap::get_mutable_is_greyscale()
    {
        return is_greyscale_;
    }
    float &Heightmap::get_mutable_y_scale()
    {
        return y_scale_factor_;
    }
    float &Heightmap::get_mutable_animation_speed()
    {
        return animation_speed_;
    }
    void Heightmap::draw(Shader *shader, const glm::mat4 &projection, const glm::mat4 &view)
    {
        static float vec = 1.0f;

        if (delta_time_ < 1.0)
        {
            delta_time_ += vec * animation_speed_;
        }
        else
        {
            delta_time_ = 1.0f;
        }
        update_vert(delta_time_);
        shader->use();
        shader->set_int("Texture1", 0);
        shader->set_int("Texture2", 1);
        if (current_texture_id_ == 1u)
        {
            shader->set_float("factor", delta_time_);
        }
        else
        {
            shader->set_float("factor", -delta_time_);
        }
        shader->set_bool("IsGreyScale", is_greyscale_);
        shader->set_float("yScale", y_scale_factor_);

        shader->set_mat4("projection", projection);
        shader->set_mat4("view", view);
        shader->set_mat4("model", transform_component_.get_mat4());
        for (int i = 0; i < texture_.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture_[i]);
        }
        glBindVertexArray(terrain_VAO_);
        if (is_wireframe_)
        {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        for (unsigned int strip = 0; strip < num_strips_; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,                                              // primitive type
                           num_tris_per_stripe_,                                           // number of indices_ to render
                           GL_UNSIGNED_INT,                                                // index data type
                           (void *)(sizeof(unsigned int) * num_tris_per_stripe_ * strip)); // offset to starting index
        }
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

}