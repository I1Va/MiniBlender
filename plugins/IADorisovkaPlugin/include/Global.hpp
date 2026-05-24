#pragma once

#include "dr4/math/vec2.hpp"
#include "dr4/mouse_buttons.hpp"

enum class Side {
    UNKNOWN = -1,
    TOP,
    BOTTOM,
    LEFT,
    RIGHT,
};

inline static constexpr float OutlineThickness = 5.0f;

inline static const dr4::Vec2f OutlineThicknessVec = {
    OutlineThickness,
    OutlineThickness
};

inline static constexpr dr4::MouseButtonType CREATE_BUTTON = dr4::MouseButtonType::LEFT;
