#include <cmath>
#include "Rectangle.hpp"

bool Rectangle::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if (selected_) {
        rect_info_.pos = evt.pos;
        rect_->SetPos(rect_info_.pos);

        resize_dot_->SetCenter(evt.pos);

        return true;
    }

    dr4::Rect2f external_rect = rect_info_;
    external_rect.size = {external_rect.size.x + kBorderThickness, external_rect.size.y + kBorderThickness};

    dr4::Rect2f internal_rect = rect_info_;
    internal_rect.size = {internal_rect.size.x - kBorderThickness, internal_rect.size.y - kBorderThickness};

    if ((external_rect.Contains(evt.pos)) && (!internal_rect.Contains(evt.pos))) {
        cvs_->SetSelectedShape(this);
        selected_ = true;

        resize_dot_->SetCenter(evt.pos);

        return true;
    }

    return false;
}

bool Rectangle::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (!selected_) {
        return false;
    }

    rect_info_.size = evt.pos - rect_info_.pos;
    if (rect_info_.size.x < 0) {
        if (rect_info_.size.y < 0) {
            rect_info_.pos = rect_info_.pos + rect_info_.size;
        } else {
            rect_info_.pos = {rect_info_.pos.x + rect_info_.size.x, rect_info_.pos.y};
        }
        rect_->SetPos(rect_info_.pos);
    } else {
        if (rect_info_.size.y < 0) {
            rect_info_.pos = {rect_info_.pos.x, rect_info_.pos.y + rect_info_.size.y};
            rect_->SetPos(rect_info_.pos);
        }
    }
    rect_info_.size = {std::fabs(rect_info_.size.x), std::fabs(rect_info_.size.y)};
    rect_->SetSize(rect_info_.size);

    cvs_->SetSelectedShape(NULL);
    selected_ = false;

    return true;
}

bool Rectangle::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (!selected_) {
        return false;
    }

    rect_info_.size = evt.pos - rect_info_.pos;
    if (rect_info_.size.x < 0) {
        if (rect_info_.size.y < 0) {
            rect_->SetPos(rect_info_.pos + rect_info_.size);
        } else {
            rect_->SetPos({rect_info_.pos.x + rect_info_.size.x, rect_info_.pos.y});
        }
    } else {
        if (rect_info_.size.y < 0) {
            rect_->SetPos({rect_info_.pos.x, rect_info_.pos.y + rect_info_.size.y});
        }
    }
    rect_info_.size = {std::fabs(rect_info_.size.x), std::fabs(rect_info_.size.y)};

    rect_->SetSize(rect_info_.size);

    resize_dot_->SetCenter(evt.pos);

    return true;
}
