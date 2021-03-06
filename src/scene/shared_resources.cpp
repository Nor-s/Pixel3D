#include "shared_resources.h"
#include "glcpp/cubemap.h"
#include "glcpp/shader.h"
#include "glcpp/model.h"
#include "glcpp/component/transform_component.h"
#include "glcpp/animator.h"
#include "glcpp/utility.hpp"
#include "glcpp/importer.h"

SharedResources::SharedResources()
{
    init_skybox();
    init_animator();
}

SharedResources::~SharedResources() = default;

void SharedResources::init_skybox()
{
    skybox_ = std::make_unique<glcpp::Cubemap>(skybox_faces_[0],
                                               "./resources/shaders/skybox.vs",
                                               "./resources/shaders/skybox.fs");
}

void SharedResources::init_animator()
{
    animator_ = std::make_unique<glcpp::Animator>();
}

glcpp::Cubemap *SharedResources::get_mutable_skybox()
{
    return skybox_.get();
}

glcpp::Animator *SharedResources::get_mutable_animator()
{
    return animator_.get();
}
std::shared_ptr<glcpp::Model> SharedResources::get_mutable_model(int idx)
{
    return models_[idx];
}
std::shared_ptr<glcpp::Animation> SharedResources::get_mutable_animation(int idx)
{
    return animations_[idx];
}
std::shared_ptr<glcpp::Model> SharedResources::back_mutable_model()
{

    return models_.back();
}

std::shared_ptr<glcpp::Animation> SharedResources::back_mutable_animation()
{
    return animations_.back();
}

std::shared_ptr<glcpp::Shader> SharedResources::get_mutable_shader(const std::string &name)
{
    if (shaders_.find(name) == shaders_.end())
    {
        return nullptr;
    }
    return shaders_[name];
}
const std::vector<std::shared_ptr<glcpp::Model>> &SharedResources::get_models() const
{
    return models_;
}
const std::vector<std::shared_ptr<glcpp::Animation>> &SharedResources::get_animations() const
{
    return animations_;
}

int SharedResources::get_models_size() const
{
    return models_.size();
}
int SharedResources::get_animations_size() const
{
    return animations_.size();
}

std::pair<bool, bool> SharedResources::add_model_or_animation_by_path(const char *path)
{
    bool is_imported_model = false;
    bool is_imported_animation = false;

    glcpp::Importer importer{};
    auto [sp_model, sp_animations] = importer.import(path);
    if (sp_model)
    {
        models_.push_back(std::move(sp_model));
        is_imported_model = true;
    }
    if (sp_animations.size() > 0)
    {
        animations_.insert(animations_.end(), std::make_move_iterator(sp_animations.begin()), std::make_move_iterator(sp_animations.end()));
        is_imported_animation = true;
    }
    return {is_imported_model, is_imported_animation};
}

void SharedResources::add_shader(const std::string &name, const char *vs_path, const char *fs_path)
{
    shaders_[name] = std::make_shared<glcpp::Shader>(vs_path, fs_path);
}
