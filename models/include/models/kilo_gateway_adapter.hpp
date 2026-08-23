#pragma once

#include "model_adapter.hpp"
#include "model_info.hpp"
#include <string>
#include <vector>
#include <cstdint>
#include <memory>
#include <unordered_map>

namespace cyborg {

enum class KiloModel {
    StepFunFree,
    Ling3_0,
    Nemotron3SuperFree
};

struct KiloModelInfo {
    KiloModel model;
    std::string id;
    std::string name;
    std::string provider;
    bool free;
};

class KiloGatewayAdapter : public ModelAdapter {
public:
    KiloGatewayAdapter();
    ~KiloGatewayAdapter();

    KiloGatewayAdapter(const KiloGatewayAdapter&) = delete;
    KiloGatewayAdapter& operator=(const KiloGatewayAdapter&) = delete;
    KiloGatewayAdapter(KiloGatewayAdapter&&) = delete;
    KiloGatewayAdapter& operator=(KiloGatewayAdapter&&) = delete;

    bool load(const std::string& model_path) override;
    bool loadModel(KiloModel model);
    std::string predict(const std::string& prompt, const BiasConfig& bias) override;
    ModelInfo getModelInfo() const override;
    bool isLoaded() const override;
    void unload() override;

    void setGatewayURL(const std::string& url);
    std::string getGatewayURL() const;

    static std::vector<KiloModelInfo> listAvailableModels();
    static KiloModel parseModelFromString(const std::string& model_str);

private:
    bool loaded_;
    std::string gateway_url_;
    KiloModel current_model_;
    ModelInfo model_info_;
    std::unordered_map<KiloModel, ModelInfo> model_cache_;

    bool connectGateway(const std::string& url);
    std::string buildChatRequest(const std::string& prompt, const BiasConfig& bias) const;
    std::string sendRequest(const std::string& request_body) const;
    void updateModelInfo(KiloModel model);
};

}