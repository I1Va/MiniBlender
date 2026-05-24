#pragma once

#include "cum/ifc/pp.hpp"
#include <memory>

class PPToolPlugin final : public ::cum::PPToolPlugin {
    dr4::Font *textToolFont = nullptr;
public:
    PPToolPlugin();
    ~PPToolPlugin();

    virtual std::string_view GetIdentifier() const override;

    virtual std::string_view GetName() const override;

    virtual std::string_view GetDescription() const override;

    virtual std::vector<std::string_view> GetDependencies() const override;

    virtual std::vector<std::string_view> GetConflicts() const override;

    virtual void AfterLoad() override;

    virtual std::vector<std::unique_ptr<::pp::Tool>> CreateTools(::pp::Canvas* cvs) override;
};
