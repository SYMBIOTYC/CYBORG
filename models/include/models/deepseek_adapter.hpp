#pragma once

#include "model_adapter.hpp"
#include "model_info.hpp"
#include <cyborg/model_loader.hpp>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <vector>
#include <cstdint>

namespace cyborg {

class DeepSeekAdapter : public ModelAdapter {
public:
    enum class Mode {
        API,
        GGUF,
        PROXY
    };

    DeepSeekAdapter();
    ~DeepSeekAdapter();

    bool load(const std::string& model_path) override;
    bool loadViaProxy(const std::string& proxy_base_url);
    std::string predict(const std::string& prompt, const BiasConfig& bias) override;
    ModelInfo getModelInfo() const override;

    Mode getMode() const;
    bool isLoaded() const override;
    void unload() override;

private:
    Mode mode_;
    std::string api_endpoint_;
    std::string proxy_base_url_;
    std::string model_path_;
    ModelInfo model_info_;
    bool loaded_;
    ModelLoader model_loader_;

    bool loadGGUF(const std::string& path);
    std::string predictGGUF(const std::string& prompt, const BiasConfig& bias);
    bool connectAPI(const std::string& endpoint);
    bool connectProxy(const std::string& proxy_url);
    std::string predictAPI(const std::string& prompt, const BiasConfig& bias);
    std::string predictProxy(const std::string& prompt, const BiasConfig& bias);
    void applySuperposition(std::string& output);
    static std::string buildAPIRequest(const std::string& prompt, const BiasConfig& bias);
    static std::string buildProxyRequest(const std::string& prompt, const BiasConfig& bias);
};

}
