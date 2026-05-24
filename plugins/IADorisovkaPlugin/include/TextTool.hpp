#pragma once

#include "dr4/texture.hpp"
#include "Text.hpp"
#include "pp/canvas.hpp"
#include "pp/tool.hpp"
#include <memory>

class TextTool final : public ::pp::Tool {
public:
    TextTool(const dr4::Font* font, ::pp::Canvas* cvs);

    virtual std::string_view Icon() const override;
    virtual std::string_view Name() const override;
    virtual bool IsCurrentlyDrawing() const override;

    virtual void OnStart() override;
    virtual void OnBreak() override;
    virtual void OnEnd() override;

    virtual bool OnMouseDown(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseUp(const dr4::Event::MouseButton &evt) override;
    virtual bool OnMouseMove(const dr4::Event::MouseMove &evt) override;
    virtual bool OnKeyDown(const dr4::Event::KeyEvent &evt) override;
    virtual bool OnText(const dr4::Event::TextEvent &evt) override;

private:
    ::pp::Canvas* cvs_;
    bool isDrawing_;
    Text* text_;
    bool keyHandled_;
    const dr4::Font* font_;
    mutable std::unique_ptr<dr4::Text> tempText_;

private:
    size_t FindLetterPos(float mousePosX) const;
};
