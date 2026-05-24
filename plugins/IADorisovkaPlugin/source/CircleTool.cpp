#include "CircleTool.hpp"

bool CircleTool::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if ((!is_drawing_ && (cvs_->GetSelectedShape() == NULL))
        || (cvs_->GetSelectedShape() != circle_)) {
        OnStart();
        return true;
    }

    is_drawing_ = true;
    circle_->SetPos(evt.pos);
    circle_->OnSelect();
    return true;
}

bool CircleTool::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (!is_drawing_) {
        return false;
    }

    circle_->OnMouseUp(evt);
    circle_->OnDeselect();
    OnEnd();
    return true;
}

bool CircleTool::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (!is_drawing_) {
        return false;
    }

    circle_->OnMouseMove(evt);
    return true;
}
