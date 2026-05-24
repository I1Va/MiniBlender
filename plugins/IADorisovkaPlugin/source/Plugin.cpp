#include <memory>

#include "Plugin.hpp"
#include "ArrowTool.hpp"
#include "CircleTool.hpp"
#include "RectangleTool.hpp"
#include "TextTool.hpp"

PPToolPlugin::PPToolPlugin() = default;

PPToolPlugin::~PPToolPlugin() {
    if (textToolFont) delete textToolFont;
}

std::string_view PPToolPlugin::GetIdentifier() const {
    return "PPToolPlugin";
}

std::string_view PPToolPlugin::GetName() const {
    return "VovchikPPToolPluginName";
}

std::string_view PPToolPlugin::GetDescription() const {
    return "VovchikPPToolPluginDesc";
}

std::vector<std::string_view> PPToolPlugin::GetDependencies() const {
    return {"DR4BackendPlugin"};
}

std::vector<std::string_view> PPToolPlugin::GetConflicts() const {
    return {};
}

void PPToolPlugin::AfterLoad() {

}

std::vector<std::unique_ptr<::pp::Tool>> PPToolPlugin::CreateTools(::pp::Canvas *cvs) {
    std::vector<std::unique_ptr<::pp::Tool>> vec;
    vec.emplace_back(std::make_unique<RectangleTool>(cvs));
    vec.emplace_back(std::make_unique<CircleTool>(cvs));
    vec.emplace_back(std::make_unique<ArrowTool>(cvs));
    vec.emplace_back(std::make_unique<TextTool>(cvs->GetWindow()->GetDefaultFont(), cvs));
    return vec;
}

extern "C" cum::Plugin* CreatePlugin() {
    return new PPToolPlugin();
}