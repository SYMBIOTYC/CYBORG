#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <memory>

namespace cyborg {

struct ModelInfo;
struct BiasConfig;

class ModelAdapter {
public:
    ModelAdapter() = default;
    virtual ~ModelAdapter() = default;

    ModelAdapter(const ModelAdapter&) = delete;
    ModelAdapter& operator=(const ModelAdapter&) = delete;
    ModelAdapter(ModelAdapter&&) = delete;
    ModelAdapter& operator=(ModelAdapter&&) = delete;

    virtual bool load(const std::string& model_path) = 0;
    virtual std::string predict(const std::string& prompt, const BiasConfig& bias) = 0;
    virtual ModelInfo getModelInfo() const = 0;
    virtual bool isLoaded() const = 0;
    virtual void unload() = 0;
};

}
