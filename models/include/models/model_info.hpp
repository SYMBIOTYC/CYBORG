#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace cyborg {

enum class ModelCapabilities {
    CodeGen,
    MathReasoning,
    GeneralChat,
    TensorOps
};

struct ModelInfo {
    std::string name;
    std::string version;
    int64_t parameters;
    int64_t context_length;
    std::vector<ModelCapabilities> supported_features;
};

struct BiasConfig {
    double temperature;
    double top_p;
    double top_k;
    double repetition_penalty;
    std::string bias_profile;
    bool enable_tensor_superposition;
};

}
