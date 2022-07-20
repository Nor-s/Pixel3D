#ifndef UI_HEIGHTMAP_LAYER_H
#define UI_HEIGHTMAP_LAYER_H

#include <string>

namespace glcpp
{
    class TransformComponent;
}

class Scene;
namespace ui
{
    struct HeightmapContext
    {
    };
    class HeightmapLayer
    {
    public:
        HeightmapLayer();
        ~HeightmapLayer();
        void draw(Scene *scene);

    private:
        void draw_transform_slider(glcpp::TransformComponent &transform);
    };
}

#endif