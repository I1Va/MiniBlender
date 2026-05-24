#pragma once

#include <memory>
#include <string>

#include "dr4/math/vec2.hpp"
#include "dr4/texture.hpp"

#include "pp/canvas.hpp"
#include "pp/shape.hpp"

#include "Global.hpp"

class TextTool;

class Text final : public ::pp::Shape {
public:
    friend TextTool;

public:
    Text(const dr4::Font* font, ::pp::Canvas* cvs);

    virtual bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseMove(const dr4::Event::MouseMove &evt) override;

    virtual void OnSelect() override;
    virtual void OnDeselect() override;

    virtual void DrawOn(::dr4::Texture& texture) const override;

    virtual void SetPos(::dr4::Vec2f pos) override;
    virtual ::dr4::Vec2f GetPos() const override;

    void EraseLeftText();
    void EraseRightText();
    void EraseSelectedText();
    void InsertText(const std::string& addedText);

    size_t GetCaretPos() const;
    void SetCaretPos(size_t pos);

    void SetIsCreating(bool isCreating);

    void SetInSelectMode(bool inSelectMode);
    void SetIsSelectedSmth(bool isSelectedSmth);
    void SetSelectPos(size_t selectPos);

    bool   GetInSelectMode();
    bool   GetIsSelectedSmth();
    size_t GetSelectPos();

    const dr4::Text* GetText() const;

private:
    std::unique_ptr<dr4::Text> text_;
    std::string textStr_;
    std::unique_ptr<dr4::Text> tempText_;
    std::unique_ptr<dr4::Rectangle> selectRect_;

    ::pp::Canvas* const cvs_;

    bool isResized_;
    Side activeSide_;

    bool isDragged_;
    dr4::Vec2f dragOffset_{};

    bool isCreating_;
    std::unique_ptr<dr4::Line> caret_;
    double caretBlinkPeriod_;
    mutable double caretPrevBlinkTime_;
    mutable bool caretIsHide_;
    size_t caretPos_;

    bool inSelectMode_;
    size_t selectPos_;
    bool isSelectedSmth_;
    std::unique_ptr<dr4::Rectangle> selectedTextRect_;

private:
    bool OnMe(dr4::Vec2f relCoord) const;
    bool OnOutline(dr4::Vec2f relCoord) const;
    Side ClosestSide(dr4::Vec2f relCoord) const;

    void UpdateSelectRect();
    void UpdateCaret();
    void UpdateSelectedTextRect();

    void ResizeBySide(dr4::Vec2f offset);
    void DrawHandles(dr4::Texture& texture) const;
};
