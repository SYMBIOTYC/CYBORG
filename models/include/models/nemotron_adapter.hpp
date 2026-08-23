#pragma once

#include "model_adapter.hpp"
#include "model_info.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace cyborg {

class UniversalTensorEngine;

class NemotronAdapter : public ModelAdapter {
public:
    NemotronAdapter();
    ~NemotronAdapter();

    NemotronAdapter(const NemotronAdapter&) = delete;
    NemotronAdapter& operator=(const NemotronAdapter&) = delete;
    NemotronAdapter(NemotronAdapter&&) = delete;
    NemotronAdapter& operator=(NemotronAdapter&&) = delete;

    bool load(const std::string& model_path) override;
    bool loadViaProxy(const std::string& proxy_base_url);
    std::string predict(const std::string& prompt, const BiasConfig& bias) override;
    ModelInfo getModelInfo() const override;
    bool isLoaded() const override;
    void unload() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    bool loadLocal(const std::string& model_path);
    bool requestProxyCompletion(const std::string& prompt, const BiasConfig& bias, std::string& out);
    bool initializeTensorSuperposition();
    std::string applySuperposition(const std::string& output);
};

}
