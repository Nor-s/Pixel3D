#ifndef GLCPP_ANIM_ASSIMP_ANIMATION_HPP
#define GLCPP_ANIM_ASSIMP_ANIMATION_HPP

#include "animation.hpp"
#include "../utility.hpp"
#include <glm/glm.hpp>
#include <assimp/scene.h>

#include <functional>
#include <vector>
#include <map>
#include <iostream>
#include <string>
#include <set>

namespace fs = std::filesystem;

namespace glcpp
{
    class AssimpAnimation : public Animation
    {
    public:
        AssimpAnimation() = default;

        AssimpAnimation(const char *path)
        {
            Assimp::Importer importer;
            unsigned int assimp_read_flag =
                aiProcess_SortByPType |
                aiProcess_ValidateDataStructure |
                aiProcess_LimitBoneWeights;

            importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);
            const aiScene *scene = importer.ReadFile(path, assimp_read_flag);

            if (!scene || !scene->mRootNode || !scene->HasAnimations())
            {
#ifndef NDEBUG
                std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
#endif
            }
            else
            {
                auto animation = scene->mAnimations[0];
                init_animation(animation, scene, path);
            }
        }
        AssimpAnimation(const aiAnimation *animation, const aiScene *scene, const char *path)
        {
            init_animation(animation, scene, path);
        }

        virtual ~AssimpAnimation()
        {
        }

    private:
        void init_animation(const aiAnimation *animation, const aiScene *scene, const char *path)
        {
            type = AnimationType::Assimp;

            path_ = std::string(path);
            fs::path anim_path = fs::u8path(path_);
            name_ = anim_path.filename().string();
            duration_ = animation->mDuration;
            ticks_per_second_ = animation->mTicksPerSecond;
            process_bones(animation, scene->mRootNode);
        }
        void process_bones(const aiAnimation *animation, const aiNode *root_node)
        {
            int size = animation->mNumChannels;

            // reading channels(bones engaged in an animation and their keyframes)
            for (int i = 0; i < size; i++)
            {
                auto channel = animation->mChannels[i];
                std::string bone_name = channel->mNodeName.C_Str();
                bone_name = bone_name.substr(bone_name.find_last_of(':') + 1);
                auto find_mixamorig = bone_name.find("mixamorig");
                if (find_mixamorig != std::string::npos)
                {
                    bone_name = bone_name.substr(find_mixamorig + 9);
                }
                const aiNode *node = root_node->FindNode(channel->mNodeName);
                if (node)
                {
                    bone_inverse_transform_map_[bone_name] = glm::inverse(AiMatToGlmMat(node->mTransformation));
                    name_bone_map_[bone_name] = std::make_unique<Bone>(bone_name, channel, bone_inverse_transform_map_[bone_name]);
                }
            }
        }

    private:
        std::map<std::string, glm::mat4> bone_inverse_transform_map_;
    };

}

#endif