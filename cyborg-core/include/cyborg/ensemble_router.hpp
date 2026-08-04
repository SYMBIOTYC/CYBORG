#pragma once

#include <cyborg/tensor.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace cyborg {

struct Request {
    std::string prompt;
    std::string context;
    std::string model_preference;
    float bias_level;
};

struct Response {
    std::string text;
    std::string model_used;
    float confidence;
    int tokens;
};

struct ModelChoice {
    std::string model_name;
    float score;
    std::string reason;
};

class EnsembleRouter {
public:
    EnsembleRouter();
    ~EnsembleRouter();

    void addModel(const std::string& name);
    void removeModel(const std::string& name);
    ModelChoice route(const Request& req);
    Response execute(const Request& req);

    void setBias(float bias);
    float getBias() const;

    std::vector<std::string> listModels() const;
    bool hasModel(const std::string& name) const;

private:
    std::unordered_map<std::string, float> m_models;
    float m_bias;

    float scoreCode(const Request& req);
    float scoreMath(const Request& req);
    float scoreGeneral(const Request& req);
    float scorePhilosophy(const Request& req);
    ModelChoice selectBest(const Request& req);
    std::string applyBias(const std::string& text, float bias);
};

} // namespace cyborg