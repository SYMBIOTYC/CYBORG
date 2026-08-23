#pragma once

#include <cyborg/tensor.hpp>
#include <cyborg/smart_cyber_relay.hpp>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>

namespace cyborg {

struct ScalarSinus {
    float frequency;
    float amplitude;
    float phase;
    float quality;
};

struct QualityScalar {
    float precision;
    float coherence;
    float depth;
    float relevance;
    float novelty;
    float bias_alignment;
    float tensor_fidelity;
    float superposition_quality;
};

struct InferenceResult {
    std::string answer;
    float probability;
    float sinus_value;
    QualityScalar quality;
    int source_brain;
};

class CyborgInference {
public:
    CyborgInference();
    ~CyborgInference();

    void setInputAnswers(const std::vector<ThoughtAnswer>& answers);
    void setPhaseBinary(bool binary);

    std::vector<InferenceResult> infer();
    Tensor buildProbabilityTensor(const std::vector<InferenceResult>& results);
    std::vector<ScalarSinus> computeSinusScalars(const Tensor& prob_tensor);
    std::vector<QualityScalar> computeQualityScalars(const std::vector<InferenceResult>& results);

    int getAnswerCount() const;
    int getTopCount() const;
    float getInferenceThreshold() const;

    std::string describePhase() const;

private:
    std::vector<ThoughtAnswer> m_answers;
    bool m_binary_phase;
    int m_top_count;
    float m_threshold;

    std::vector<ScalarSinus> m_sinus_scalars;
    std::vector<QualityScalar> m_quality_scalars;

    static constexpr int SINUS_COUNT = 4;
    static constexpr int QUALITY_PARAMS_PER_ANSWER = 18;
    static constexpr int TOTAL_QUALITY_PARAMS = 72;

    float computeSinusValue(float frequency, float amplitude, float phase, float t);
    float computeBinaryScore(const InferenceResult& result);
    std::vector<InferenceResult> selectTopAnswers(const std::vector<InferenceResult>& results);
    Tensor buildSquareProbabilityTensor(const std::vector<InferenceResult>& top_results);
};

} // namespace cyborg