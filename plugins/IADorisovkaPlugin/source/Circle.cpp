#include <cmath>

#include "Circle.hpp"

bool Circle::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if (selected_) {
        center_ = evt.pos;
        circle_->SetCenter(evt.pos);
        return true;
    }

    float radius_circle = circle_->GetRadius().x;
    dr4::Vec2f vec = evt.pos - center_;
    float cur_radius = sqrt(vec.x * vec.x + vec.y * vec.y);
    if ((cur_radius > radius_circle - kBorderThickness)
        && (cur_radius < radius_circle + kBorderThickness)) {
        cvs_->SetSelectedShape(this);
        selected_ = true;
        return true;
    }

    return false;
}

bool Circle::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (!selected_) {
        return false;
    }

    dr4::Vec2f vec = evt.pos - center_;
    float radius = sqrt(vec.x * vec.x + vec.y * vec.y);
    circle_->SetRadius(radius);
    circle_->SetCenter(center_);
    cvs_->SetSelectedShape(NULL);
    selected_ = false;
    return true;
}

bool Circle::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (!selected_) {
        return false;
    }

    dr4::Vec2f vec = evt.pos - center_;
    float radius = sqrt(vec.x * vec.x + vec.y * vec.y);
    circle_->SetRadius(radius);
    circle_->SetCenter(center_);
    return true;
}