#pragma once

#include "model_adapter.hpp"
#include "model_info.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace cyborg {

class TensorEngine;

class QwenAdapter : public ModelAdapter {
public:
    QwenAdapter();
    ~QwenAdapter();

    bool load(const std::string& model_path) override;
    bool loadFromOllamaManifest(const std::string& manifest_path);

    std::string predict(const std::string& prompt, const BiasConfig& bias) override;

    ModelInfo getModelInfo() const override;

    bool isLoaded() const override;
    void unload() override;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    bool loadGGUF(const std::string& model_path);
    bool initializeTensorSuperposition();
    std::string runInference(const std::string& prompt, const BiasConfig& bias);
    std::string applySuperposition(const std::string& output);
};

}
