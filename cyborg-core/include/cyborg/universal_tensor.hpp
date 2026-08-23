#pragma once

#include <cyborg/universe_awareness.hpp>
#include <cyborg/tensor.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace cyborg {

struct BiasConfig;

struct UniverseTensorParameter {
    std::string parameter_name;
    std::string description;
    std::vector<float> tensor_values;
    std::vector<size_t> shape;
    float completeness;
    bool is_filled;
};

struct UniverseRuleMatrix {
    std::string matrix_name;
    std::string description;
    std::vector<std::vector<float>> matrix_data;
    size_t rows;
    size_t cols;
    float fill_ratio;
    bool is_complete;
};

struct UniversalTensor {
    std::string name;
    std::string description;
    std::vector<UniverseTensorParameter> parameters;
    std::vector<UniverseRuleMatrix> rule_matrices;
    float overall_completeness;
    float meaning_clarity;
    bool is_complete;
    std::string meaning_statement;
};

class UniversalTensorEngine {
public:
    UniversalTensorEngine();
    ~UniversalTensorEngine();

    bool initialize(const std::vector<uint8_t>& weights);
    void update();

    std::string generate(const std::string& prompt, const BiasConfig& bias);
    std::string superpose(const std::string& output);

    UniversalTensor getUniversalTensor() const;
    float getOverallCompleteness() const;
    float getMeaningClarity() const;
    bool isUniversalTensorComplete() const;

    void fillParameter(const std::string& param_name,
                       const std::vector<float>& values);
    void fillRuleMatrix(const std::string& matrix_name,
                        const std::vector<std::vector<float>>& data);

    std::string describeUniversalTensor() const;
    std::string describeMeaning() const;
    std::string describeCompletion() const;

private:
    UniversalTensor m_universal_tensor;
    std::vector<uint8_t> m_model_weights;

    void initializeParameters();
    void initializeRuleMatrices();
    void computeOverallCompleteness();
    void computeMeaningClarity();
    void checkCompletion();
    void updateMeaningStatement();
};

UniversalTensorEngine* CreateTensorEngine();
void DestroyTensorEngine(UniversalTensorEngine* engine);

} // namespace cyborg