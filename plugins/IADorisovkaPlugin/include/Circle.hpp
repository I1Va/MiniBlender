#pragma once
#include "pp/shape.hpp"
#include "pp/canvas.hpp"

class Circle : public ::pp::Shape {
private:
    ::pp::Canvas* cvs_;

    bool selected_;

    const dr4::Color kBorderColor = {dr4::Color::ColorMaxValue, 0, 0};
    const float kBorderThickness = 5;

    dr4::Circle* circle_;
    dr4::Vec2f center_;
    dr4::Rectangle* border_;

public:
    Circle(dr4::Circle* circle, ::pp::Canvas* cvs) {
        cvs_ = cvs;
        circle_ = circle;
        circle_->SetBorderColor(kBorderColor);
        circle_->SetBorderThickness(kBorderThickness);
        circle_->SetFillColor({0, 0, 0, 0});
        border_ = cvs->GetWindow()->CreateRectangle();
        border_->SetFillColor({0, 0, 0, 0});
        border_->SetBorderThickness(kBorderThickness);
        selected_ = false;
    }

    ~Circle() override {
        delete circle_;
        delete border_;
    }

    virtual bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseMove(const dr4::Event::MouseMove &evt) override;

    virtual void DrawOn(dr4::Texture& texture) const override {
        if (selected_) {
            circle_->SetBorderColor(cvs_->GetControlsTheme().handleActiveColor);
        } else {
            circle_->SetBorderColor(cvs_->GetControlsTheme().shapeBorderColor);
        }

        circle_->DrawOn(texture);

        if (selected_) {
            float radius = circle_->GetRadius().x;
            border_->SetPos(center_ - dr4::Vec2f(radius, radius));
            border_->SetSize({2 * radius, 2 * radius});
            border_->DrawOn(texture);
        }
    }

    virtual void SetPos(dr4::Vec2f pos) override {
        center_ = pos;
        circle_->SetCenter(pos);
    }

    virtual dr4::Vec2f GetPos() const override {
        return circle_->GetPos();
    }

    virtual void OnSelect() override { selected_ = true; }
    virtual void OnDeselect() override { selected_ = false; }

    void SetTheme(const ::pp::ControlsTheme& theme) {
        circle_->SetFillColor(theme.shapeFillColor);
        circle_->SetBorderColor(theme.shapeBorderColor);
        border_->SetBorderColor(theme.handleColor);
    }
};
