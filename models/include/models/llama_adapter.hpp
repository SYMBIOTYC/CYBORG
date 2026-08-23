#pragma once

#include "model_adapter.hpp"
#include "model_info.hpp"
#include <string>
#include <memory>
#include <optional>

namespace cyborg {

class UniversalTensorEngine;

enum class LlamaBackend {
    GGUF,
    LlamaCppServer,
    MPS
};

struct ServerConfig {
    std::string host;
    int port;
    std::string api_path;
};

class LlamaAdapter : public ModelAdapter {
public:
    LlamaAdapter();
    ~LlamaAdapter();

    LlamaAdapter(const LlamaAdapter&) = delete;
    LlamaAdapter& operator=(const LlamaAdapter&) = delete;
    LlamaAdapter(LlamaAdapter&&) = delete;
    LlamaAdapter& operator=(LlamaAdapter&&) = delete;

    bool load(const std::string& model_path) override;
    bool loadServer(const ServerConfig& config);
    std::string predict(const std::string& prompt, const BiasConfig& bias) override;
    ModelInfo getModelInfo() const override;

    void setBackend(LlamaBackend backend);
    LlamaBackend getBackend() const;
    bool isLoaded() const override;
    void unload() override;

private:
    bool loadGGUF(const std::string& model_path);
    bool loadMPS(const std::string& model_path);
    bool connectToServer(const ServerConfig& config);
    std::string predictServer(const std::string& prompt, const BiasConfig& bias);
    std::string predictLocal(const std::string& prompt, const BiasConfig& bias);
    std::string applyBiasSuperposition(const std::string& response, const BiasConfig& bias);
    bool initializeTensorSuperposition();

    bool loaded_;
    LlamaBackend backend_;
    ModelInfo model_info_;
    ServerConfig server_config_;
    UniversalTensorEngine* tensor_engine_;
    std::optional<std::string> model_path_;
    void* model_handle_;
    void* context_handle_;
};

}
