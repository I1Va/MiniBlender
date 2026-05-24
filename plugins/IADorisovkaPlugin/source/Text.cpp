#include <cmath>
#include <cassert>
#include <cstdio>
#include <memory>
#include <iostream>

#include "Text.hpp"
#include "dr4/math/color.hpp"
#include "dr4/math/rect.hpp"
#include "dr4/math/vec2.hpp"
#include "dr4/texture.hpp"
#include "Global.hpp"

static dr4::Vec2f GetTextSizeSafe(const dr4::Text* t) {
    const dr4::Vec2f bounds = t->GetBounds();
    const dr4::Vec2f pos = t->GetPos();
    dr4::Vec2f size = bounds;
    if (bounds.x > pos.x && bounds.y > pos.y) {
        size = bounds - pos;
    }
    if (size.x < 0) size.x = 0;
    if (size.y < 0) size.y = 0;
    return size;
}

Text::Text(const dr4::Font* font, ::pp::Canvas* cvs)
    : text_{cvs->GetWindow()->CreateText()},
      textStr_("Enter text"),
      tempText_(cvs->GetWindow()->CreateText()),
      selectRect_(cvs->GetWindow()->CreateRectangle()),
      cvs_{cvs},
      isResized_(false),
      activeSide_{Side::UNKNOWN},
      isDragged_(false),
      isCreating_(false),
      caret_{cvs->GetWindow()->CreateLine()},
      caretBlinkPeriod_(0.5),
      caretPrevBlinkTime_(cvs->GetWindow()->GetTime()),
      caretIsHide_(false),
      caretPos_(textStr_.size()),
      inSelectMode_(false),
      selectPos_(0),
      isSelectedSmth_(false),
      selectedTextRect_(cvs->GetWindow()->CreateRectangle())
{
    text_->SetText(textStr_);
    text_->SetColor(cvs->GetControlsTheme().textColor);
    text_->SetFontSize(cvs->GetControlsTheme().baseFontSize);
    text_->SetFont(font);
    tempText_->SetFont(font);
    caret_->SetColor(cvs->GetControlsTheme().textColor);
    caret_->SetThickness(OutlineThickness);
    caret_->SetStart({0, 0});
    caret_->SetEnd({0, 0.7f * text_->GetBounds().y});
    const dr4::Color lineColor = cvs->GetControlsTheme().shapeBorderColor;
    selectedTextRect_->SetFillColor({lineColor.r, lineColor.g, lineColor.b, 100});
    selectedTextRect_->SetSize({1, 1});
    selectRect_->SetBorderThickness(OutlineThickness);
    selectRect_->SetBorderColor(cvs->GetControlsTheme().shapeBorderColor);
    selectRect_->SetFillColor({0, 0, 0, 0});
    caret_->SetStart({0,0});
    caret_->SetEnd({0, std::max(1.0f, cvs_->GetControlsTheme().baseFontSize * 0.8f)});
    caret_->SetThickness(std::max(1.0f, OutlineThickness));
    UpdateCaret();
    UpdateSelectRect();
    UpdateSelectedTextRect();
}

bool Text::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if (evt.button == dr4::MouseButtonType::LEFT) {
        OnSelect();
        if (OnOutline(evt.pos)) {
            activeSide_ = ClosestSide(evt.pos);
            isResized_ = true;
            return true;
        }
        if (OnMe(evt.pos)) {
            isDragged_ = true;
            dragOffset_ = evt.pos - text_->GetPos();
            return true;
        }
    }
    return false;
}

bool Text::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (evt.button == dr4::MouseButtonType::LEFT) {
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

bool Text::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (isDragged_) {
        SetPos(evt.pos - dragOffset_);
        return true;
    }
    if (isResized_) {
        ResizeBySide(evt.rel);
        return true;
    }
    return false;
}

void Text::OnSelect() {
    if (cvs_->GetSelectedShape() != nullptr) {
        cvs_->GetSelectedShape()->OnDeselect();
    }
    cvs_->SetSelectedShape(this);
}

void Text::OnDeselect() {
    if (isCreating_) SetIsCreating(false);
    SetIsSelectedSmth(false);
    SetInSelectMode(false);
}

void Text::DrawOn(::dr4::Texture& texture) const {
    text_->DrawOn(texture);
    if ((inSelectMode_ || isSelectedSmth_) && selectPos_ != caretPos_) selectedTextRect_->DrawOn(texture);
    if (cvs_->GetSelectedShape() == this || isCreating_) selectRect_->DrawOn(texture);
    if (isCreating_) {
        double now = cvs_->GetWindow()->GetTime();
        if (now - caretPrevBlinkTime_ > caretBlinkPeriod_) {
            caretPrevBlinkTime_ = now;
            caretIsHide_ = !caretIsHide_;
        }
        if (!caretIsHide_) caret_->DrawOn(texture);
    }
}

void Text::SetPos(::dr4::Vec2f pos) {
    text_->SetPos(pos);
    UpdateCaret();
    UpdateSelectRect();
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

::dr4::Vec2f Text::GetPos() const { return text_->GetPos(); }

void Text::EraseLeftText() {
    if (caretPos_ == 0) return;
    const size_t minPos = std::min(caretPos_, selectPos_);
    const size_t maxPos = std::max(caretPos_, selectPos_);
    if (isSelectedSmth_ && minPos != maxPos) {
        textStr_.erase(minPos, maxPos - minPos);
        SetCaretPos(minPos);
        SetIsSelectedSmth(false);
        SetInSelectMode(false);
    } else {
        textStr_.erase(caretPos_ - 1, 1);
        --caretPos_;
    }
    text_->SetText(textStr_);
    UpdateCaret();
    UpdateSelectRect();
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

void Text::EraseRightText() {
    if (caretPos_ >= textStr_.size()) return;
    const size_t minPos = std::min(caretPos_, selectPos_);
    const size_t maxPos = std::max(caretPos_, selectPos_);
    if (isSelectedSmth_ && minPos != maxPos) {
        textStr_.erase(minPos, maxPos - minPos);
        SetCaretPos(minPos);
        SetIsSelectedSmth(false);
        SetInSelectMode(false);
    } else {
        textStr_.erase(caretPos_, 1);
    }
    text_->SetText(textStr_);
    UpdateCaret();
    UpdateSelectRect();
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

void Text::EraseSelectedText() {
    if (caretPos_ == selectPos_) return;
    const size_t minPos = std::min(caretPos_, selectPos_);
    const size_t maxPos = std::max(caretPos_, selectPos_);
    textStr_.erase(minPos, maxPos - minPos);
    text_->SetText(textStr_);
    SetCaretPos(minPos);
    UpdateCaret();
    UpdateSelectRect();
    SetIsSelectedSmth(false);
    SetInSelectMode(false);
}

void Text::InsertText(const std::string& addedText) {
    if (isSelectedSmth_ && selectPos_ != caretPos_) {
        const size_t minPos = std::min(caretPos_, selectPos_);
        const size_t maxPos = std::max(caretPos_, selectPos_);
        textStr_.erase(minPos, maxPos - minPos);
        caretPos_ = minPos;
        isSelectedSmth_ = false;
        inSelectMode_ = false;
    }
    textStr_.insert(caretPos_, addedText);
    caretPos_ += addedText.size();
    text_->SetText(textStr_);
    UpdateCaret();
    UpdateSelectRect();
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

size_t Text::GetCaretPos() const { return caretPos_; }

void Text::SetCaretPos(size_t pos) {
    caretPos_ = std::min(pos, textStr_.size());
    UpdateCaret();
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

void Text::SetIsCreating(bool isCreating) {
    isCreating_ = isCreating;
    if (!isCreating_) {
        isSelectedSmth_ = false;
        inSelectMode_ = false;
    }
    caretPrevBlinkTime_ = cvs_->GetWindow()->GetTime();
    caretIsHide_ = false;
}

void Text::SetInSelectMode(bool inSelectMode) { inSelectMode_ = inSelectMode; }
void Text::SetIsSelectedSmth(bool isSelectedSmth) { isSelectedSmth_ = isSelectedSmth; }
void Text::SetSelectPos(size_t selectPos) {
    selectPos_ = std::min(selectPos, textStr_.size());
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

bool Text::GetInSelectMode() { return inSelectMode_; }
bool Text::GetIsSelectedSmth() { return isSelectedSmth_; }
size_t Text::GetSelectPos() { return selectPos_; }
const dr4::Text* Text::GetText() const { return text_.get(); }

bool Text::OnMe(dr4::Vec2f relCoord) const {
    const dr4::Vec2f pos = text_->GetPos();
    const dr4::Vec2f size = text_->GetBounds();
    return dr4::Rect2f(pos, size).Contains(relCoord);
}

bool Text::OnOutline(dr4::Vec2f relCoord) const {
    const dr4::Vec2f posIn = selectRect_->GetPos();
    const dr4::Vec2f sizeIn = selectRect_->GetSize();
    const dr4::Vec2f posOut = posIn - dr4::Vec2f(selectRect_->GetBorderThickness(), selectRect_->GetBorderThickness());
    const dr4::Vec2f sizeOut = sizeIn + 2 * dr4::Vec2f(selectRect_->GetBorderThickness(), selectRect_->GetBorderThickness());
    return dr4::Rect2f(posOut, sizeOut).Contains(relCoord) && !dr4::Rect2f(posIn, sizeIn).Contains(relCoord);
}

Side Text::ClosestSide(dr4::Vec2f relCoord) const {
    const dr4::Vec2f pos  = text_->GetPos();
    const dr4::Vec2f size = text_->GetBounds();
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
    if (distBottom < minDist) { minDist = distBottom; minSide = Side::BOTTOM; }
    if (distLeft < minDist) { minDist = distLeft; minSide = Side::LEFT; }
    if (distRight < minDist) { minDist = distRight; minSide = Side::RIGHT; }
    return minSide;
}

void Text::ResizeBySide(dr4::Vec2f offset) {
    dr4::Vec2f pos  = text_->GetPos();
    dr4::Vec2f prevSize = GetTextSizeSafe(text_.get());
    float scale = 1.0f;
    bool needChangePos = false;
    switch (activeSide_) {
        case Side::LEFT: { float newWidth = prevSize.x - offset.x; if (newWidth > 0.0f) { scale = newWidth / prevSize.x; needChangePos = true; } break; }
        case Side::RIGHT: { float newWidth = prevSize.x + offset.x; if (newWidth > 0.0f) scale = newWidth / prevSize.x; break; }
        case Side::TOP: { float newHeight = prevSize.y - offset.y; if (newHeight > 0.0f) { scale = newHeight / prevSize.y; needChangePos = true; } break; }
        case Side::BOTTOM: { float newHeight = prevSize.y + offset.y; if (newHeight > 0.0f) scale = newHeight / prevSize.y; break; }
        default: return;
    }
    text_->SetFontSize(scale * text_->GetFontSize());
    if (needChangePos) {
        dr4::Vec2f newSize = GetTextSizeSafe(text_.get());
        if (activeSide_ == Side::LEFT) text_->SetPos(pos - (newSize - prevSize));
        else if (activeSide_ == Side::TOP) text_->SetPos(pos - dr4::Vec2f{0, newSize.y - prevSize.y});
    }
    UpdateCaret();
    UpdateSelectRect();
    if (isSelectedSmth_ || inSelectMode_) UpdateSelectedTextRect();
}

void Text::UpdateSelectRect() {
    const dr4::Vec2f textPos = text_->GetPos();
    const dr4::Vec2f textSize = GetTextSizeSafe(text_.get());
    const float minW = std::max(8.0f, cvs_->GetControlsTheme().baseFontSize * 0.4f);
    const float minH = std::max(1.0f, cvs_->GetControlsTheme().baseFontSize * 0.9f);
    const float width = std::max(textSize.x, minW);
    const float height = std::max(textSize.y, minH);
    const float extraW = OutlineThickness + std::max(1.0f, caret_->GetThickness());
    const float extraH = OutlineThickness;
    selectRect_->SetPos(textPos - OutlineThicknessVec);
    selectRect_->SetSize({ width + extraW, height + extraH + OutlineThickness });
}

void Text::UpdateCaret() {
    tempText_->SetFontSize(text_->GetFontSize());
    if (caretPos_ > 0) tempText_->SetText(textStr_.substr(0, caretPos_));
    else tempText_->SetText(std::string());
    const float prefixW = GetTextSizeSafe(tempText_.get()).x;
    const dr4::Vec2f textPos = text_->GetPos();
    const dr4::Vec2f textSize = GetTextSizeSafe(text_.get());
    const float caretThickness = 1.0f;
    const float caretX = std::round(textPos.x + prefixW + caretThickness);
    const float caretY = textPos.y;
    const float caretH = std::max(textSize.y, 1.0f);
    caret_->SetStart({caretX, caretY});
    caret_->SetEnd({caretX, caretY + caretH});
    caret_->SetThickness(caretThickness);
}

void Text::UpdateSelectedTextRect() {
    if (caretPos_ == selectPos_) return;
    tempText_->SetFontSize(text_->GetFontSize());
    size_t start = std::min(caretPos_, selectPos_);
    size_t end   = std::max(caretPos_, selectPos_);
    tempText_->SetText(textStr_.substr(0, start));
    float leftOffset = GetTextSizeSafe(tempText_.get()).x;
    tempText_->SetText(textStr_.substr(0, end));
    float rightOffset = GetTextSizeSafe(tempText_.get()).x;
    if (leftOffset > rightOffset) std::swap(leftOffset, rightOffset);
    const dr4::Vec2f textPos = text_->GetPos();
    const dr4::Vec2f textSize = GetTextSizeSafe(text_.get());
    const float verticalPadding = 2.0f;
    selectedTextRect_->SetPos(textPos + dr4::Vec2f(leftOffset, 0));
    selectedTextRect_->SetSize({ rightOffset - leftOffset, std::max(textSize.y, 1.0f) + verticalPadding });
}
