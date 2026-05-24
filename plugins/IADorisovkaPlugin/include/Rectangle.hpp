#pragma once

#include <memory>

#include "dr4/math/vec2.hpp"
#include "dr4/texture.hpp"

#include "pp/canvas.hpp"
#include "pp/shape.hpp"

#include "Global.hpp"

class Rectangle : public ::pp::Shape {
private:
    ::pp::Canvas* cvs_;

    bool selected_;

    const dr4::Color kBorderColor = {dr4::Color::ColorMaxValue, 0, 0};
    const float kBorderThickness = 5;

    dr4::Rectangle* rect_;
    dr4::Rect2f rect_info_;
    dr4::Circle* resize_dot_;

public:
    Rectangle(dr4::Rectangle* rect, ::pp::Canvas* cvs) {
        cvs_ = cvs;
        rect_ = rect;
        rect_info_ = {};
        rect_->SetBorderColor(kBorderColor);
        rect_->SetBorderThickness(kBorderThickness);
        rect_->SetFillColor({0, 0, 0, 0});
        selected_ = false;
        resize_dot_ = cvs->GetWindow()->CreateCircle();
        resize_dot_->SetRadius(kBorderThickness);
    };

    ~Rectangle() {
        delete rect_;
        delete resize_dot_;
    };

    bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    bool OnMouseMove(const dr4::Event::MouseMove &evt) override;

    void DrawOn(dr4::Texture& texture) const override {
        if (selected_) {
            rect_->SetBorderColor(cvs_->GetControlsTheme().handleColor);
        } else {
            rect_->SetBorderColor(cvs_->GetControlsTheme().shapeBorderColor);
        }

        rect_->DrawOn(texture);

        if (selected_) {
            resize_dot_->DrawOn(texture);
        }
    };

    void SetPos(dr4::Vec2f pos) override {
        rect_info_.pos = pos;
        rect_->SetPos(pos);
    };

    dr4::Vec2f GetPos() const override {
        return rect_info_.pos;
    };

    void OnSelect() override { selected_ = true; };
    void OnDeselect() override { selected_ = false; };

    void SetTheme(const ::pp::ControlsTheme& theme) {
        rect_->SetFillColor(theme.shapeFillColor);
        rect_->SetBorderColor(theme.shapeBorderColor);
        resize_dot_->SetFillColor(theme.handleHoverColor);
    };
};
