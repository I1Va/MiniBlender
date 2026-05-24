#include <cassert>
#include <cmath>

#include "ArrowTool.hpp"
#include "dr4/math/vec2.hpp"
#include "Arrow.hpp"
#include "Global.hpp"

ArrowTool::ArrowTool(::pp::Canvas* cvs)
    :   cvs_{cvs},
        isDrawing_(false),
        arrow_{nullptr},
        startPos_{}
{}

std::string_view ArrowTool::Icon() const {
    return "ArrowToolIcon";
}

std::string_view ArrowTool::Name() const {
    return "ArrowTool";
}

bool ArrowTool::IsCurrentlyDrawing() const {
    return isDrawing_;
}

void ArrowTool::OnStart() {
}

void ArrowTool::OnBreak() {
    if (isDrawing_) {
        assert(arrow_);
        isDrawing_ = false;
        cvs_->DelShape(arrow_);
    }
}

void ArrowTool::OnEnd() {
    if (isDrawing_) {
        isDrawing_ = false;
        arrow_->OnSelect();
    }
}

bool ArrowTool::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if (evt.button != CREATE_BUTTON) {
        return false;
    }

    if (!isDrawing_) {
        isDrawing_ = true;

        arrow_ = new Arrow(cvs_);
        cvs_->AddShape(arrow_);
        arrow_->SetPos(evt.pos);
        startPos_ = evt.pos;
        return true;
    } else {
        isDrawing_ = false;
        arrow_->OnSelect();
        return true;
    }

    return false;
}

bool ArrowTool::OnMouseUp(const dr4::Event::MouseButton &) {
    return false;
}

bool ArrowTool::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (!isDrawing_) {
        return false;
    }

    assert(arrow_);

    arrow_->SetEnd(evt.pos);

    return true;
}