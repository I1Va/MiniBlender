#pragma once

#include "dr4/math/vec2.hpp"
#include "Rectangle.hpp"
#include "pp/canvas.hpp"
#include "pp/tool.hpp"

class RectangleTool : public ::pp::Tool {
private:
    ::pp::Canvas* cvs_;

    bool is_drawing_;

    Rectangle* rect_;

    const std::string kIcon = "R";
    const std::string kName = "RectangleTool";

public:
    RectangleTool(::pp::Canvas* cvs)
        : cvs_(cvs), is_drawing_(false), rect_(NULL) {};

    std::string_view Icon() const override { return kIcon; };
    std::string_view Name() const override { return kName; };
    bool IsCurrentlyDrawing() const override { return is_drawing_; };

    void OnStart() override {
        is_drawing_ = false;
        rect_ = new Rectangle(cvs_->GetWindow()->CreateRectangle(), cvs_);
        rect_->SetTheme(cvs_->GetControlsTheme());
        cvs_->AddShape(rect_);
        cvs_->SetSelectedShape(rect_);
    };

    void OnBreak() override {
        if (!is_drawing_) return;
        cvs_->DelShape(rect_);
        OnEnd();
    };

    void OnEnd() override {
        rect_ = NULL;
        is_drawing_ = false;
        cvs_->SetSelectedShape(NULL);
    };

    bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    bool OnMouseMove(const dr4::Event::MouseMove &evt) override;
};
