#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>

namespace cyborg {

class ModelManager {
public:
    static ModelManager& instance();

    bool loadModel(const std::string& name);
    bool unloadModel(const std::string& name);
    bool reloadModel(const std::string& name);
    std::vector<std::string> getActiveModels() const;
    void setBias(float bias);
    float getBias() const;
    bool isModelLoaded(const std::string& name) const;

private:
    ModelManager() = default;
    ModelManager(const ModelManager&) = delete;
    ModelManager& operator=(const ModelManager&) = delete;

    std::unordered_map<std::string, bool> m_loaded;
    float m_bias;
    mutable std::mutex m_mutex;
};

} // namespace cyborg