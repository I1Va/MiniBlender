#include <cmath>
#include <cassert>
#include "Arrow.hpp"
#include "dr4/math/vec2.hpp"
#include "dr4/math/rect.hpp"

#include "Global.hpp"

Arrow::Arrow(::pp::Canvas* cvs)
    : line_{cvs->GetWindow()->CreateLine()},
      leftTip_{cvs->GetWindow()->CreateLine()},
      rightTip_{cvs->GetWindow()->CreateLine()},
      selectRect_{cvs->GetWindow()->CreateRectangle()},
      cvs_{cvs},
      isResized_(false),
      activeSide_(Side::UNKNOWN),
      isDragged_(false),
      isStartPointDragged_(false),
      isEndPointDragged_(false)
{
    line_->SetStart({0, 0});
    line_->SetEnd({1, 1});
    line_->SetColor(cvs->GetControlsTheme().shapeFillColor);
    line_->SetThickness(10);

    leftTip_->SetColor(cvs->GetControlsTheme().shapeFillColor);
    rightTip_->SetColor(cvs->GetControlsTheme().shapeFillColor);
    leftTip_->SetThickness(10);
    rightTip_->SetThickness(10);

    selectRect_->SetBorderThickness(OutlineThickness);
    selectRect_->SetBorderColor(cvs->GetControlsTheme().shapeBorderColor);
    selectRect_->SetFillColor({0, 0, 0, 0});

    UpdateTips();
    UpdateSelectRect();
}

bool Arrow::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if (evt.button == dr4::MouseButtonType::LEFT) {
        OnSelect();

        if (OnOutline(evt.pos)) {
            activeSide_ = ClosestSide(evt.pos);
            isResized_ = true;
            return true;
        }

        if (OnMe(evt.pos)) {
            isDragged_ = true;
            return true;
        }
    }

    return false;
}

bool Arrow::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (evt.button == dr4::MouseButtonType::LEFT) {
        if (isStartPointDragged_) {
            isStartPointDragged_ = false;
            return true;
        }

        if (isEndPointDragged_) {
            isEndPointDragged_ = false;
            return true;
        }

        if (isResized_) {
            isResized_ = false;
            return true;
        }

        if (isDragged_) {
            isDragged_ = false;
            return true;
        }
    }

    return false;
}

bool Arrow::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (isStartPointDragged_) {
        SetStart(GetStart() + evt.rel);
        return true;
    }

    if (isEndPointDragged_) {
        SetEnd(GetEnd() + evt.rel);
        return true;
    }

    if (isDragged_) {
        SetPos(GetPos() + evt.rel);
        return true;
    }

    if (isResized_) {
        ResizeBySide(evt.rel);
        return true;
    }

    return false;
}

void Arrow::OnSelect() {
    if (cvs_->GetSelectedShape() != nullptr) {
        cvs_->GetSelectedShape()->OnDeselect();
    }
    cvs_->SetSelectedShape(this);
}

void Arrow::OnDeselect() {
}

void Arrow::DrawOn(::dr4::Texture& texture) const {
    line_->DrawOn(texture);
    leftTip_->DrawOn(texture);
    rightTip_->DrawOn(texture);

    if (cvs_->GetSelectedShape() == this) {
        selectRect_->DrawOn(texture);
    }
}

void Arrow::SetPos(::dr4::Vec2f pos) {
    dr4::Vec2f currentPos = GetPos();
    dr4::Vec2f offset = pos - currentPos;
    
    SetStart(GetStart() + offset);
    SetEnd(GetEnd() + offset);
}

::dr4::Vec2f Arrow::GetPos() const {
    return line_->GetPos();
}

void Arrow::SetStart(::dr4::Vec2f start) {
    line_->SetStart(start);
    UpdateTips();
    UpdateSelectRect();
}

void Arrow::SetEnd(::dr4::Vec2f end) {
    line_->SetEnd(end);
    UpdateTips();
    UpdateSelectRect();
}

::dr4::Vec2f Arrow::GetStart() const {
    return line_->GetStart();
}

::dr4::Vec2f Arrow::GetEnd() const {
    return line_->GetEnd();
}

bool Arrow::OnMe(dr4::Vec2f relCoord) const {
    const dr4::Vec2f pos = selectRect_->GetPos();
    const dr4::Vec2f size = selectRect_->GetSize();
    return dr4::Rect2f(pos, size).Contains(relCoord);
}

bool Arrow::OnOutline(dr4::Vec2f relCoord) const {
    const dr4::Vec2f pos = selectRect_->GetPos();
    const dr4::Vec2f size = selectRect_->GetSize();

    const dr4::Vec2f posOut = pos - dr4::Vec2f(selectRect_->GetBorderThickness(), selectRect_->GetBorderThickness());
    const dr4::Vec2f sizeOut = size + 2 * dr4::Vec2f(selectRect_->GetBorderThickness(), selectRect_->GetBorderThickness());
    
    const dr4::Vec2f posIn = pos + dr4::Vec2f(selectRect_->GetBorderThickness(), selectRect_->GetBorderThickness());
    const dr4::Vec2f sizeIn = size - 2 * dr4::Vec2f(selectRect_->GetBorderThickness(), selectRect_->GetBorderThickness());
    
    return dr4::Rect2f(posOut, sizeOut).Contains(relCoord) 
       && !dr4::Rect2f(posIn,  sizeIn) .Contains(relCoord);
}

Side Arrow::ClosestSide(dr4::Vec2f relCoord) const {
    const dr4::Vec2f pos  = selectRect_->GetPos();
    const dr4::Vec2f size = selectRect_->GetSize();

    const float left   = pos.x;
    const float right  = pos.x + size.x;
    const float top    = pos.y;
    const float bottom = pos.y + size.y;

    const float x = relCoord.x;
    const float y = relCoord.y;

    float distTop    = std::fabs(y - top);
    float distBottom = std::fabs(y - bottom);
    float distLeft   = std::fabs(x - left);
    float distRight  = std::fabs(x - right);

    float minDist = distTop;
    Side minSide = Side::TOP;

    if (distBottom < minDist) {
        minDist = distBottom;
        minSide = Side::BOTTOM;
    }
    if (distLeft < minDist) {
        minDist = distLeft;
        minSide = Side::LEFT;
    }
    if (distRight < minDist) {
        minDist = distRight;
        minSide = Side::RIGHT;
    }

    return minSide;
}

void Arrow::UpdateSelectRect() {
    dr4::Vec2f start = GetStart();
    dr4::Vec2f end = GetEnd();
    
    dr4::Vec2f leftTipStart = leftTip_->GetStart();
    dr4::Vec2f rightTipStart = rightTip_->GetStart();
    
    dr4::Vec2f minPos = {
        std::min({start.x, end.x, leftTipStart.x, rightTipStart.x}),
        std::min({start.y, end.y, leftTipStart.y, rightTipStart.y})
    };
    
    dr4::Vec2f maxPos = {
        std::max({start.x, end.x, leftTipStart.x, rightTipStart.x}),
        std::max({start.y, end.y, leftTipStart.y, rightTipStart.y})
    };
    
    const float padding = 5.0f;
    minPos -= dr4::Vec2f(padding, padding);
    maxPos += dr4::Vec2f(padding, padding);
    
    selectRect_->SetPos(minPos);
    selectRect_->SetSize(maxPos - minPos);
}

void Arrow::ResizeBySide(dr4::Vec2f offset) { //FIXME
    dr4::Vec2f start = GetStart();
    dr4::Vec2f end = GetEnd();

    switch (activeSide_) {
        case Side::LEFT: {
            start.x += offset.x;
            break;
        }
        case Side::RIGHT: {
            end.x += offset.x;
            break;
        }
        case Side::TOP: {
            start.y += offset.y;
            break;
        }
        case Side::BOTTOM: {
            end.y += offset.y;
            break;
        }
        case Side::UNKNOWN:
        default:
            return;
    }

    SetStart(start);
    SetEnd(end);
}

dr4::Vec2f Arrow::GetNormal(dr4::Vec2f vec) {
    return {vec.y, -vec.x};
}

void Arrow::UpdateTips() {
    const dr4::Vec2f start = GetStart();
    const dr4::Vec2f end = GetEnd();
    const dr4::Vec2f vector = end - start;

    leftTip_->SetEnd(end);
    rightTip_->SetEnd(end);

    constexpr double TIP_SCALE = 0.1;

    const dr4::Vec2f ScaledTendril(-vector * TIP_SCALE);
    const dr4::Vec2f NormalTendril(GetNormal(ScaledTendril));
    const dr4::Vec2f    TanTendril(vector + 2.f * ScaledTendril);

    leftTip_->SetStart(TanTendril + NormalTendril);
    rightTip_->SetStart(TanTendril - NormalTendril);

    leftTip_->SetStart(start + leftTip_->GetStart());
    rightTip_->SetStart(start + rightTip_->GetStart());
}
