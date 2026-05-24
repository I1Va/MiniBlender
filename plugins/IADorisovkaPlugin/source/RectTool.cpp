
#include "RectangleTool.hpp"

bool RectangleTool::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if ((!is_drawing_ && (cvs_->GetSelectedShape() == NULL))
        || (cvs_->GetSelectedShape() != rect_)) {
        OnStart();
        return true;
    }

    is_drawing_ = true;
    rect_->SetPos(evt.pos);
    rect_->OnSelect();
    return true;
}

bool RectangleTool::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (!is_drawing_) {
        return false;
    }

    rect_->OnMouseUp(evt);
    rect_->OnDeselect();
    OnEnd();
    return true;
}

bool RectangleTool::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (!is_drawing_) {
        return false;
    }

    rect_->OnMouseMove(evt);
    return true;
}
