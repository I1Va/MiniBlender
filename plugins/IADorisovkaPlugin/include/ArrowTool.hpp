#pragma once

#include "dr4/math/vec2.hpp"
#include "Arrow.hpp"
#include "pp/canvas.hpp"
#include "pp/tool.hpp"

class ArrowTool final : public ::pp::Tool {

public:

    ArrowTool(::pp::Canvas* cvs);

    virtual std::string_view Icon() const override;
    virtual std::string_view Name() const override;
    virtual bool IsCurrentlyDrawing() const override;

    virtual void OnStart() override;
    virtual void OnBreak() override;
    virtual void OnEnd() override;

    virtual bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseMove(const dr4::Event::MouseMove &evt) override;

private:

    ::pp::Canvas* cvs_;

    bool isDrawing_;

    Arrow* arrow_;

    ::dr4::Vec2f startPos_;

};
