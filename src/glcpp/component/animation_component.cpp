#include "animation_component.h"

#include "anim/animation.hpp"

namespace glcpp
{
    AnimationComponent::AnimationComponent(std::shared_ptr<Animation> animation)
    {
        set_animation(animation);
    }
    void AnimationComponent::init_animation()
    {
        if (animation_->get_ticks_per_second() >= 120)
        {
            custom_ticks_per_second_ = 24.0f;
        }
        else
        {
            custom_ticks_per_second_ = animation_->get_ticks_per_second();
        }
        current_time_ = 0.0f;
        fps_ = custom_ticks_per_second_;
        is_stop_ = false;
    }
    void AnimationComponent::play()
    {
        is_stop_ = false;
    }
    void AnimationComponent::stop()
    {
        is_stop_ = true;
    }
    void AnimationComponent::reload()
    {
        if (animation_ && animation_->get_type() == AnimationType::Json)
        {
            animation_->reload();
            init_animation();
        }
    }

    void AnimationComponent::set_animation(std::shared_ptr<Animation> animation)
    {
        animation_.reset();
        animation_ = std::move(animation);
        init_animation();
    }

    void AnimationComponent::set_current_frame_num_to_time(uint32_t frame)
    {
        current_time_ = static_cast<float>(frame);
    }

    void AnimationComponent::set_custom_tick_per_second(float tick_per_second)
    {
        if (tick_per_second > 0.0f)
        {
            custom_ticks_per_second_ = tick_per_second;
        }
    }

    void AnimationComponent::set_fps(float fps)
    {
        if (fps > 0.0f)
        {
            fps_ = fps;
        }
    }

    const Animation *AnimationComponent::get_animation() const
    {
        return animation_.get();
    }

    Animation *AnimationComponent::get_mutable_animation()
    {
        return animation_.get();
    }

    float AnimationComponent::get_origin_current_time(float time)
    {
        auto origin_ticks_per_second = animation_->get_ticks_per_second();
        return time * origin_ticks_per_second / get_mutable_custom_tick_per_second();
    }

    bool *AnimationComponent::get_mutable_pointer_is_loop()
    {
        return &is_loop_;
    }
    bool &AnimationComponent::get_mutable_is_loop()
    {
        return is_loop_;
    }

    bool &AnimationComponent::get_mutable_is_stop()
    {
        return is_stop_;
    }
    const uint32_t AnimationComponent::get_current_frame_num() const
    {
        return static_cast<uint32_t>(roundf(current_time_));
    }

    float AnimationComponent::get_ticks_per_second_factor() const
    {
        return custom_ticks_per_second_ / animation_->get_ticks_per_second();
    }

    const uint32_t AnimationComponent::get_custom_duration() const
    {
        auto duration = animation_->get_duration();
        return static_cast<uint32_t>(duration * get_ticks_per_second_factor());
    }

    float &AnimationComponent::get_mutable_custom_tick_per_second()
    {
        return custom_ticks_per_second_;
    }

    float &AnimationComponent::get_mutable_current_time()
    {
        return current_time_;
    }
    float &AnimationComponent::get_mutable_fps()
    {
        return fps_;
    }
    float AnimationComponent::get_fps() const
    {
        return fps_;
    }
    float AnimationComponent::get_tps() const
    {
        return custom_ticks_per_second_;
    }
}