#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <mutex>
#include <vector>

#include "model_adapter.hpp"

namespace cyborg {

struct ModelInfo;
struct BiasConfig;

class ModelRegistry {
public:
    static ModelRegistry& instance();

    void registerModel(const std::string& name, ModelAdapter* adapter);
    ModelAdapter* getModel(const std::string& name);
    const ModelAdapter* getModel(const std::string& name) const;
    std::vector<std::string> listModels() const;
    bool hasModel(const std::string& name) const;
    void removeModel(const std::string& name);

    size_t count() const;

private:
    ModelRegistry() = default;
    ModelRegistry(const ModelRegistry&) = delete;
    ModelRegistry& operator=(const ModelRegistry&) = delete;

    std::unordered_map<std::string, ModelAdapter*> m_models;
    mutable std::mutex m_mutex;
};

}
 // namespace cyborg