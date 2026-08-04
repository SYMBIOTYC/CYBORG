#include <cyborg/ensemble_router.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <regex>

namespace cyborg {

EnsembleRouter::EnsembleRouter()
    : m_bias(0.5f) {
}

EnsembleRouter::~EnsembleRouter() {
    m_models.clear();
}

void EnsembleRouter::addModel(const std::string& name) {
    m_models[name] = 0.0f;
}

void EnsembleRouter::removeModel(const std::string& name) {
    m_models.erase(name);
}

ModelChoice EnsembleRouter::route(const Request& req) {
    if (m_models.empty()) {
        throw std::runtime_error("No models registered in ensemble router");
    }

    std::vector<ModelChoice> candidates;
    for (const auto& [name, model] : m_models) {
        float score = 0.0f;
        std::string reason;

        float code_score = scoreCode(req);
        float math_score = scoreMath(req);
        float general_score = scoreGeneral(req);
        float philosophy_score = scorePhilosophy(req);

        if (name.find("qwen") != std::string::npos || name.find("Qwen") != std::string::npos) {
            score = math_score * 0.6f + code_score * 0.3f + general_score * 0.1f;
            reason = "Qwen excels at math and code";
        } else if (name.find("llama") != std::string::npos || name.find("Llama") != std::string::npos) {
            score = general_score * 0.5f + philosophy_score * 0.3f + code_score * 0.2f;
            reason = "Llama balanced across domains";
        } else if (name.find("deepseek") != std::string::npos || name.find("DeepSeek") != std::string::npos) {
            score = code_score * 0.7f + math_score * 0.2f + general_score * 0.1f;
            reason = "DeepSeek specialized for code generation";
        } else {
            score = general_score * 0.4f + code_score * 0.3f + math_score * 0.3f;
            reason = "Default routing based on general capability";
        }

        score = score * (1.0f + m_bias * 0.2f);

        candidates.push_back({name, score, reason});
    }

    std::sort(candidates.begin(), candidates.end(),
        [](const ModelChoice& a, const ModelChoice& b) {
            return a.score > b.score;
        });

    return candidates.front();
}

Response EnsembleRouter::execute(const Request& req) {
    auto choice = route(req);
    Response resp;
    resp.model_used = choice.model_name;
    resp.confidence = choice.score;
    resp.text = "Cyborg processed via " + choice.model_name + " with bias " + std::to_string(m_bias);
    resp.tokens = static_cast<int>(resp.text.size() / 4);
    return resp;
}

void EnsembleRouter::setBias(float bias) {
    m_bias = std::clamp(bias, 0.0f, 1.0f);
}

float EnsembleRouter::getBias() const {
    return m_bias;
}

std::vector<std::string> EnsembleRouter::listModels() const {
    std::vector<std::string> names;
    for (const auto& [name, _] : m_models) {
        names.push_back(name);
    }
    return names;
}

bool EnsembleRouter::hasModel(const std::string& name) const {
    return m_models.find(name) != m_models.end();
}

float EnsembleRouter::scoreCode(const Request& req) {
    std::regex code_pattern(R"(\b(function|def|class|import|from|return|if|else|for|while|try|catch|const|let|var|async|await|yield|lambda|map|filter|reduce)\b)");
    std::smatch match;
    if (std::regex_search(req.prompt, match, code_pattern)) {
        return 0.9f;
    }
    std::regex code_kw(R"(\b(code|function|algorithm|implement|compile|debug|syntax|variable|loop|recursion)\b)");
    if (std::regex_search(req.prompt, match, code_kw)) {
        return 0.7f;
    }
    return 0.1f;
}

float EnsembleRouter::scoreMath(const Request& req) {
    std::regex math_pattern(R"(\b(calculate|compute|integrate|derivative|matrix|tensor|equation|formula|prove|theorem|limit|sum|product|factorial|derivative|differential|integral|linear algebra|eigenvalue|determinant)\b)");
    std::smatch match;
    if (std::regex_search(req.prompt, match, math_pattern)) {
        return 0.95f;
    }
    std::regex math_sym(R"([+\-*/=<>≤≥≠≈∞∑∏∫∂∇√πe^])");
    if (std::regex_search(req.prompt, match, math_sym)) {
        return 0.85f;
    }
    return 0.05f;
}

float EnsembleRouter::scoreGeneral(const Request& req) {
    return 0.5f;
}

float EnsembleRouter::scorePhilosophy(const Request& req) {
    std::regex phil_pattern(R"(\b(why|meaning|purpose|consciousness|existence|nature|reality|truth|ethics|philosophy|being|become|understand|aware|intentional|bias|Cyborg)\b)");
    std::smatch match;
    if (std::regex_search(req.prompt, match, phil_pattern)) {
        return 0.8f;
    }
    return 0.1f;
}

std::string EnsembleRouter::applyBias(const std::string& text, float bias) {
    if (bias < 0.3f) return text;
    return "[Cyborg-Bias:" + std::to_string(bias) + "] " + text;
}

} // namespace cyborg