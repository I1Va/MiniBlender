#include <cassert>
#include <cmath>
#include <memory>
#include <string_view>
#include <iostream>
#include <vector>

#include "TextTool.hpp"
#include "dr4/keycodes.hpp"
#include "dr4/math/vec2.hpp"
#include "dr4/texture.hpp"
#include "Global.hpp"
#include "Text.hpp"

TextTool::TextTool(const dr4::Font* font, ::pp::Canvas* cvs)
    : cvs_{cvs},
      isDrawing_(false),
      text_{nullptr},
      keyHandled_(false),
      font_(font),
      tempText_(cvs_->GetWindow()->CreateText())
{
    tempText_->SetFont(font);
}

std::string_view TextTool::Icon() const { return "TextToolIcon"; }
std::string_view TextTool::Name() const { return "TextTool"; }
bool TextTool::IsCurrentlyDrawing() const { return isDrawing_; }

void TextTool::OnStart() {}
void TextTool::OnBreak() {
    if (isDrawing_) {
        assert(text_);
        isDrawing_ = false;
        text_->SetIsCreating(false);
        cvs_->DelShape(text_);
        text_ = nullptr;
    }
}
void TextTool::OnEnd() {
    if (isDrawing_) {
        isDrawing_ = false;
        if (text_) {
            text_->SetIsCreating(false);
            text_->OnSelect();
            text_ = nullptr;
        }
    }
}

bool TextTool::OnMouseDown(const dr4::Event::MouseButton &evt) {
    if (!isDrawing_ && evt.button == CREATE_BUTTON) {
        isDrawing_ = true;
        text_ = new Text(font_, cvs_);
        cvs_->AddShape(text_);
        text_->SetPos(evt.pos);
        text_->SetIsCreating(true);
        text_->OnSelect();
        text_->SetCaretPos(text_->GetText()->GetText().size());
        return true;
    }

    if (isDrawing_) {
        text_->SetIsSelectedSmth(false);
    }

    if (isDrawing_ && evt.button == CREATE_BUTTON && text_->OnMe(evt.pos)) {
        size_t caretPos = FindLetterPos(evt.pos.x);
        text_->SetCaretPos(caretPos);
        text_->SetSelectPos(caretPos);
        text_->SetInSelectMode(true);
        return true;
    }

    return false;
}

bool TextTool::OnMouseUp(const dr4::Event::MouseButton &evt) {
    if (!isDrawing_) return false;

    if (text_->GetInSelectMode() && evt.button == CREATE_BUTTON) {
        text_->SetInSelectMode(false);
        if (text_->GetSelectPos() != text_->GetCaretPos()) {
            text_->SetIsSelectedSmth(true);
        }
    }

    return false;
}

bool TextTool::OnMouseMove(const dr4::Event::MouseMove &evt) {
    if (!isDrawing_) return false;

    if (text_->GetInSelectMode()) {
        size_t caretPos = FindLetterPos(evt.pos.x);
        text_->SetCaretPos(caretPos);
    }

    return false;
}

bool TextTool::OnKeyDown(const dr4::Event::KeyEvent &evt) {
    if (!isDrawing_) return false;

    keyHandled_ = true;

    if (evt.sym == dr4::KEYCODE_ENTER && (evt.mods & dr4::KEYMOD_SHIFT)) {
        isDrawing_ = false;
        text_->SetIsCreating(false);
        text_->OnSelect();
        return true;
    }

    if (evt.sym == dr4::KEYCODE_BACKSPACE) {
        if (text_->GetIsSelectedSmth()) text_->EraseSelectedText();
        else text_->EraseLeftText();
        return true;
    }

    if (evt.sym == dr4::KEYCODE_DELETE) {
        if (text_->GetIsSelectedSmth()) text_->EraseSelectedText();
        else text_->EraseRightText();
        return true;
    }

    const size_t curCaretPos = text_->GetCaretPos();

    if (evt.sym == dr4::KEYCODE_LEFT) {
        if (curCaretPos == 0) { keyHandled_ = false; return true; }

        if (evt.mods & dr4::KEYMOD_SHIFT) {
            if (!text_->GetIsSelectedSmth()) {
                text_->SetIsSelectedSmth(true);
                text_->SetSelectPos(curCaretPos);
            }
        } else {
            text_->SetIsSelectedSmth(false);
        }
        text_->SetCaretPos(curCaretPos - 1);
        return true;
    }

    if (evt.sym == dr4::KEYCODE_RIGHT) {
        if (curCaretPos >= text_->GetText()->GetText().size()) { keyHandled_ = false; return true; }

        if (evt.mods & dr4::KEYMOD_SHIFT) {
            if (!text_->GetIsSelectedSmth()) {
                text_->SetIsSelectedSmth(true);
                text_->SetSelectPos(curCaretPos);
            }
        } else {
            text_->SetIsSelectedSmth(false);
        }
        text_->SetCaretPos(curCaretPos + 1);
        return true;
    }

    keyHandled_ = false;
    return false;
}

bool TextTool::OnText(const dr4::Event::TextEvent &evt) {
    if (!isDrawing_) return false;

    if (!keyHandled_) {
        if (text_->GetIsSelectedSmth() || text_->GetInSelectMode()) {
            text_->EraseSelectedText();
        }
        text_->InsertText(evt.unicode);
        return true;
    } else {
        keyHandled_ = false;
        return false;
    }
}

size_t TextTool::FindLetterPos(float mousePosX) const {
    const float textPosX = text_->GetPos().x;
    const std::string& s = text_->GetText()->GetText();
    const size_t n = s.size();

    tempText_->SetFontSize(text_->GetText()->GetFontSize());

    std::vector<float> widths;
    widths.reserve(n + 1);
    for (size_t i = 0; i <= n; ++i) {
        if (i == 0) tempText_->SetText(std::string());
        else tempText_->SetText(std::string(s.begin(), s.begin() + static_cast<ptrdiff_t>(i)));
        widths.push_back(tempText_->GetBounds().x);
    }

    if (mousePosX <= textPosX + widths[0]) return 0;
    for (size_t i = 0; i < n; ++i) {
        float mid = textPosX + (widths[i] + widths[i+1]) * 0.5f;
        if (mousePosX < mid) return i;
    }
    return n;
}
