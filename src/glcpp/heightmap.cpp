#include "heightmap.h"

namespace glcpp
{
    Heightmap::Heightmap(const char *path)
    {
        set_target(path);
        init_indices();
        init_buffers();
        set_texture("./resources/textures/heightmap/rgba.png");
    }

    Heightmap::~Heightmap()
    {
        glDeleteVertexArrays(1, &terrain_VAO_);
        glDeleteBuffers(1, &terrain_VBO_);
        glDeleteBuffers(1, &terrain_IBO_);
    }

    void Heightmap::set_target(const char *path)
    {
        stbi_set_flip_vertically_on_load(true);
        unsigned char *data = stbi_load(path, &width_, &height_, &nr_channels_, 0);
        std::cout << "channel: " << nr_channels_ << "\n";
        if (data)
        {
            std::cout << "Loaded heightmap of size " << height_ << " x " << width_ << std::endl;
        }
        else
        {
            std::cout << "Failed to load texture" << std::endl;
        }

        // set up vertex data (and buffer(s)) and configure vertex attributes
        // ------------------------------------------------------------------
        float yScale = (64.0f / 256.0f) * 2.0f, yShift = 16.0f;
        unsigned bytePerPixel = nr_channels_;
        for (int i = 0; i < height_; i++)
        {
            for (int j = 0; j < width_; j++)
            {
                unsigned char *pixelOffset = data + (j + width_ * i) * bytePerPixel;
                unsigned char y = pixelOffset[0];

                // vertex
                target_vertices_.push_back(-height_ / 2.0f + height_ * i / (float)height_); // vx
                target_vertices_.push_back((int)y * yScale - yShift);                       // vy
                target_vertices_.push_back(-width_ / 2.0f + width_ * j / (float)width_);    // vz

                current_vertices_.push_back(-height_ / 2.0f + height_ * i / (float)height_); // vx
                current_vertices_.push_back((int)y * (yScale / 3.0f) - yShift);              // vy
                current_vertices_.push_back(-width_ / 2.0f + width_ * j / (float)width_);    // vz

                // uv
                uv_.push_back(j / static_cast<float>(width_));
                uv_.push_back(i / static_cast<float>(height_));
            }
        }
        new_vec_ = current_vertices_;
        std::cout << "Loaded " << current_vertices_.size() / 3 << " current_vertices_" << std::endl;
        stbi_image_free(data);
    }
    void Heightmap::update_vert(float y_factor)
    {
        for (int i = 0; i < current_vertices_.size(); i += 3)
        {
            int idx = i + 1;
            new_vec_[idx] = (1 - y_factor) * current_vertices_[idx] + y_factor * target_vertices_[idx];
        }
        glBindBuffer(GL_ARRAY_BUFFER, terrain_VBO_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, new_vec_.size() * sizeof(float), &new_vec_[0]);
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
        glGenTextures(1, &texture_);
        glBindTexture(GL_TEXTURE_2D, texture_);

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

    void Heightmap::draw(Shader *shader, const glm::mat4 &projection, const glm::mat4 &view)
    {
        static float factor = 0.0f;
        static float vec = 1.0f;
        static float speed = 0.002f;

        factor += vec * speed;

        if (factor >= 1.0)
        {
            vec = -1.0f;
        }
        else if (factor <= 0.0)
        {
            vec = 1.0f;
        }
        update_vert(factor);
        shader->use();
        shader->set_int("Texture1", 0); // or with shader class

        shader->set_mat4("projection", projection);
        shader->set_mat4("view", view);
        glm::mat4 model = glm::mat4(1.0f);
        shader->set_mat4("model", model);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture_);
        glBindVertexArray(terrain_VAO_);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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