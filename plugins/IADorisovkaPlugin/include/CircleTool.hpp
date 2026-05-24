#pragma once

#include "Circle.hpp"

class CircleTool : public ::pp::Tool {
private:
    ::pp::Canvas* cvs_;

    bool is_drawing_;

    Circle* circle_;

    const std::string kIcon = "C";
    const std::string kName = "CircleTool";

public:
    CircleTool(::pp::Canvas* cvs)
        : cvs_(cvs), is_drawing_(false), circle_(nullptr) {}

    ~CircleTool() = default;

    virtual std::string_view Icon() const override { return kIcon; }
    virtual std::string_view Name() const override { return kName; }
    virtual bool IsCurrentlyDrawing() const override { return is_drawing_; }

    virtual void OnStart() override {
        is_drawing_ = false;
        circle_ = new Circle(cvs_->GetWindow()->CreateCircle(), cvs_);
        circle_->SetTheme(cvs_->GetControlsTheme());
        cvs_->AddShape(circle_);
        cvs_->SetSelectedShape(circle_);
    }

    virtual void OnBreak() override {
        if (!is_drawing_) {
            return;
        }
        cvs_->DelShape(circle_);
        OnEnd();
    }

    virtual void OnEnd() override {
        circle_ = nullptr;
        is_drawing_ = false;
        cvs_->SetSelectedShape(nullptr);
    }

    virtual bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseMove(const dr4::Event::MouseMove &evt) override;
};
