#include "heightmap_layer.h"
#include "glcpp/heightmap.h"
#include "scene/scene.hpp"
#include "scene/shared_resources.h"
#include <imgui/imgui.h>
#include "scene/redis_db.h"

namespace ui
{
    HeightmapLayer::HeightmapLayer() = default;
    HeightmapLayer::~HeightmapLayer()
    {
    }
    void HeightmapLayer::draw(Scene *scene)
    {
        static int grid_size = 1;
        static float alpha = 1.0f;

        glcpp::Heightmap *terrain = scene->get_mutable_ref_shared_resources()->get_mutable_heightmap();
        RedisDB *redis = scene->get_mutable_ref_shared_resources()->get_mutable_redis();

        if (ImGui::Begin("Properties"))
        {
            if (ImGui::CollapsingHeader("Transform"))
            {
                draw_transform_slider(terrain->get_mutable_transform());
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("Render"))
            {
                bool wireframe = terrain->get_mutable_is_wireframe();
                bool greyscale = terrain->get_mutable_is_greyscale();
                float y_scale = terrain->get_mutable_y_scale();
                float speed = terrain->get_mutable_animation_speed();
                ImGui::Text("wireframe");
                ImGui::Checkbox("##wireframe", &wireframe);
                terrain->get_mutable_is_wireframe() = wireframe;
                ImGui::Text("greyscale");
                ImGui::Checkbox("##greyscale", &greyscale);
                terrain->get_mutable_is_greyscale() = greyscale;

                ImGui::Text("y scale");
                ImGui::DragFloat("##y scale", &y_scale, 0.01f, 0.0f, 0.0f);
                terrain->get_mutable_y_scale() = y_scale;

                ImGui::Text("anim speed");
                ImGui::DragFloat("##animation speed", &speed, 0.00001f, 0.000001f, 1.0f, "%.6f");
                terrain->get_mutable_animation_speed() = speed;

                if (ImGui::Button("Reload"))
                {
                    terrain->reload_animation();
                }
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("CGANs"))
            {
                std::string &python_path = redis->get_mutable_python_path();
                std::string &script_path = redis->get_mutable_script_path();
                std::string &models_dir_path = redis->get_mutable_models_dir_path();

                ImGui::Text("python path");
                ImGui::InputText("##python path", &python_path[0], python_path.size());
                ImGui::Text("script path");
                ImGui::InputText("##script path", &script_path[0], script_path.size());
                ImGui::Text("model output path");
                ImGui::InputText("##models dir", &models_dir_path[0], models_dir_path.size());
                ImGui::Text("grid size");
                ImGui::InputInt("##num", &grid_size);
                ImGui::Text("alpha");
                ImGui::DragFloat("##alpha", &alpha, 0.001f, 0.001f, 2.0f, "%.3f");
                if (ImGui::Button("Ursus arctos Linnaeus"))
                {
                    std::string command = "{ \"CGAN\": \"0\", \"alpha\":  \"" +
                                          std::to_string(alpha) +
                                          "\", \"label\": \"0\", \"grid_size\": \"" +
                                          std::to_string(grid_size) + "\" }";
                    redis->push(command);
                }
                if (ImGui::Button("Theropithecus gelada"))
                {
                    std::string command = "{ \"CGAN\": \"0\", \"alpha\":  \"" +
                                          std::to_string(alpha) +
                                          "\", \"label\": \"2\", \"grid_size\": \"" +
                                          std::to_string(grid_size) + "\" }";
                    redis->push(command);
                }
                if (ImGui::Button("Yucca brevifolia"))
                {
                    std::string command = "{ \"CGAN\": \"0\", \"alpha\":  \"" +
                                          std::to_string(alpha) +
                                          "\", \"label\": \"1\", \"grid_size\": \"" +
                                          std::to_string(grid_size) + "\" }";
                    redis->push(command);
                }
                ImGui::Separator();
            }
            if (ImGui::CollapsingHeader("GANs"))
            {
                if (ImGui::Button("generate"))
                {
                }
            }
        }
        ImGui::End();
    }
    void HeightmapLayer::draw_transform_slider(glcpp::TransformComponent &transform)
    {
        if (ImGui::Button("reset"))
        {
            transform.set_translation({0.0f, 0.0f, 0.0f}).set_rotation({0.0f, 0.0f, 0.0f}).set_scale({1.0f, 1.0f, 1.0f});
        }
        auto &rotation = transform.get_rotation();
        glm::vec3 r = rotation;
        ImGui::Text("Rotate");
        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::DragFloat("##rotate.x", &r.x, 0.01f, -6.5f, 6.5f);
        ImGui::Text("y:");
        ImGui::SameLine();
        ImGui::DragFloat("##rotate.y", &r.y, 0.01f, -6.5f, 6.5f);
        ImGui::Text("z:");
        ImGui::SameLine();
        ImGui::DragFloat("##rotate.z", &r.z, 0.01f, -6.5f, 6.5f);
        transform.set_rotation({r.x, r.y, r.z});
        ImGui::Separator();

        auto &scale = transform.get_scale();
        r = scale;
        ImGui::Text("Scale");
        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::DragFloat("##scale", &r.x, 0.05f, 0.1f, 100.0f);
        transform.set_scale({r.x, r.x, r.x});
        ImGui::Separator();

        auto &translation = transform.get_translation();
        r = translation;
        ImGui::Text("Translate");
        ImGui::Text("x:");
        ImGui::SameLine();
        ImGui::DragFloat("##translation.x", &r.x, 1.0f, -1000.0f, 1000.0f);
        ImGui::Text("y:");
        ImGui::SameLine();
        ImGui::DragFloat("##translation.y", &r.y, 1.0f, -1000.0f, 1000.0f);
        ImGui::Text("z:");
        ImGui::SameLine();
        ImGui::DragFloat("##translation.z", &r.z, 1.0f, -1000.0f, 1000.0f);
        transform.set_translation({r.x, r.y, r.z});
    }
}