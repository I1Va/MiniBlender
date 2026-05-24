#pragma once

#include <memory>
#include <cmath>

#include "dr4/math/vec2.hpp"
#include "dr4/texture.hpp"

#include "pp/canvas.hpp"
#include "pp/shape.hpp"

#include "Global.hpp"

class Arrow final : public ::pp::Shape {

public:

    Arrow(::pp::Canvas* cvs);

    virtual bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseMove(const dr4::Event::MouseMove &evt) override;

    virtual void OnSelect() override;
    virtual void OnDeselect() override;

    virtual void DrawOn(::dr4::Texture& texture) const override;

    virtual void SetPos(::dr4::Vec2f pos) override;
    virtual ::dr4::Vec2f GetPos() const override;

    void SetStart(::dr4::Vec2f start);
    void SetEnd(::dr4::Vec2f end);
    ::dr4::Vec2f GetStart() const;
    ::dr4::Vec2f GetEnd() const;

private:

    std::unique_ptr<dr4::Line> line_;
    std::unique_ptr<dr4::Line> leftTip_;
    std::unique_ptr<dr4::Line> rightTip_;
    std::unique_ptr<dr4::Rectangle> selectRect_;

    ::pp::Canvas* const cvs_;

    bool isResized_;
    Side activeSide_;

    bool isDragged_;
    bool isStartPointDragged_;
    bool isEndPointDragged_;

private:

    bool OnMe(dr4::Vec2f relCoord) const;
    bool OnOutline(dr4::Vec2f relCoord) const;
    Side ClosestSide(dr4::Vec2f relCoord) const;

    void UpdateSelectRect();
    void UpdateTips();

    void ResizeBySide(dr4::Vec2f offset);

    static dr4::Vec2f GetNormal(dr4::Vec2f vec);

};
