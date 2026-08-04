#include <cyborg/cyborg_inference.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

namespace cyborg {

CyborgInference::CyborgInference()
    : m_binary_phase(true)
    , m_top_count(4)
    , m_threshold(0.5f) {
    m_sinus_scalars.reserve(SINUS_COUNT);
    m_quality_scalars.reserve(m_top_count);
}

CyborgInference::~CyborgInference() {
}

void CyborgInference::setInputAnswers(const std::vector<ThoughtAnswer>& answers) {
    m_answers = answers;
}

void CyborgInference::setPhaseBinary(bool binary) {
    m_binary_phase = binary;
}

std::vector<InferenceResult> CyborgInference::infer() {
    std::vector<InferenceResult> all_results;

    for (const auto& answer : m_answers) {
        InferenceResult result;
        result.answer = answer.answer;
        result.probability = answer.confidence;
        result.source_brain = 0;

        for (int s = 0; s < SINUS_COUNT; ++s) {
            ScalarSinus sinus;
            sinus.frequency = 0.5f + static_cast<float>(s) * 0.5f;
            sinus.amplitude = answer.confidence;
            sinus.phase = static_cast<float>(s) * M_PI / 2.0f;
            sinus.quality = answer.confidence;

            float sinus_val = computeSinusValue(
                sinus.frequency, sinus.amplitude, sinus.phase,
                static_cast<float>(s));
            result.sinus_value += sinus_val;
        }

        result.sinus_value /= static_cast<float>(SINUS_COUNT);

        QualityScalar qs;
        qs.precision = answer.confidence * 0.9f;
        qs.coherence = answer.confidence * 0.85f;
        qs.depth = answer.confidence * 0.8f;
        qs.relevance = answer.confidence * 0.95f;
        qs.novelty = answer.confidence * 0.7f;
        qs.bias_alignment = answer.confidence * 0.88f;
        qs.tensor_fidelity = answer.confidence * 0.92f;
        qs.superposition_quality = answer.confidence * 0.87f;
        result.quality = qs;

        all_results.push_back(result);
    }

    if (m_binary_phase) {
        for (auto& result : all_results) {
            float binary_score = computeBinaryScore(result);
            result.probability = binary_score;
        }
    }

    std::sort(all_results.begin(), all_results.end(),
        [](const InferenceResult& a, const InferenceResult& b) {
            return a.probability > b.probability;
        });

    auto top_results = selectTopAnswers(all_results);

    std::cout << "[Cyborg Inference] Phase: "
              << (m_binary_phase ? "BINARY" : "ANALOG") << std::endl;
    std::cout << "[Cyborg Inference] Input answers: " << m_answers.size() << std::endl;
    std::cout << "[Cyborg Inference] Top " << m_top_count
              << " answers selected for sinus processing" << std::endl;
    std::cout << "[Cyborg Inference] Quality scalars: "
              << TOTAL_QUALITY_PARAMS << " parameters" << std::endl;

    return top_results;
}

Tensor CyborgInference::buildProbabilityTensor(
    const std::vector<InferenceResult>& results) {
    if (results.empty()) {
        return Tensor();
    }

    int n = static_cast<int>(results.size());
    std::vector<size_t> shape = {static_cast<size_t>(n), static_cast<size_t>(n)};
    std::vector<float> data(n * n, 0.0f);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                data[i * n + j] = results[i].probability;
            } else {
                float cross = results[i].probability * results[j].probability;
                float sinus_interference = std::sin(
                    results[i].sinus_value * results[j].sinus_value);
                data[i * n + j] = cross * sinus_interference;
            }
        }
    }

    Tensor prob_tensor(shape, data);
    return prob_tensor.normalize();
}

std::vector<ScalarSinus> CyborgInference::computeSinusScalars(
    const Tensor& prob_tensor) {
    std::vector<ScalarSinus> sinus_scalars;

    if (prob_tensor.numElements() == 0) {
        return sinus_scalars;
    }

    const auto& data = prob_tensor.data();
    float max_val = tensor_max(prob_tensor);
    float min_val = tensor_min(prob_tensor);
    float range = max_val - min_val;
    if (range < 1e-8f) range = 1.0f;

    for (int s = 0; s < SINUS_COUNT; ++s) {
        ScalarSinus sinus;
        sinus.frequency = 0.5f + static_cast<float>(s) * 0.5f;
        sinus.amplitude = range;
        sinus.phase = static_cast<float>(s) * M_PI / 2.0f;
        sinus.quality = (data[s % data.size()] - min_val) / range;
        sinus_scalars.push_back(sinus);
    }

    return sinus_scalars;
}

std::vector<QualityScalar> CyborgInference::computeQualityScalars(
    const std::vector<InferenceResult>& results) {
    std::vector<QualityScalar> quality_scalars;

    for (const auto& result : results) {
        quality_scalars.push_back(result.quality);
    }

    return quality_scalars;
}

int CyborgInference::getAnswerCount() const {
    return static_cast<int>(m_answers.size());
}

int CyborgInference::getTopCount() const {
    return m_top_count;
}

float CyborgInference::getInferenceThreshold() const {
    return m_threshold;
}

float CyborgInference::computeSinusValue(
    float frequency, float amplitude, float phase, float t) {
    return amplitude * std::sin(frequency * t + phase);
}

float CyborgInference::computeBinaryScore(const InferenceResult& result) {
    float score = result.probability;
    float sinus_contribution = std::sin(result.sinus_value * M_PI);
    score = (score + sinus_contribution) / 2.0f;
    return std::clamp(score, 0.0f, 1.0f);
}

std::vector<InferenceResult> CyborgInference::selectTopAnswers(
    const std::vector<InferenceResult>& results) {
    std::vector<InferenceResult> top;
    int count = std::min(m_top_count, static_cast<int>(results.size()));
    for (int i = 0; i < count; ++i) {
        top.push_back(results[i]);
    }
    return top;
}

Tensor CyborgInference::buildSquareProbabilityTensor(
    const std::vector<InferenceResult>& top_results) {
    return buildProbabilityTensor(top_results);
}

std::string CyborgInference::describePhase() const {
    std::ostringstream oss;
    oss << "Cyborg Inference Phase" << std::endl;
    oss << "  Phase type: " << (m_binary_phase ? "BINARY" : "ANALOG") << std::endl;
    oss << "  Input answers: " << m_answers.size() << std::endl;
    oss << "  Top answers selected: " << m_top_count << std::endl;
    oss << "  Sinus scalars: " << SINUS_COUNT << std::endl;
    oss << "  Quality params per answer: " << QUALITY_PARAMS_PER_ANSWER << std::endl;
    oss << "  Total quality params: " << TOTAL_QUALITY_PARAMS << std::endl;
    oss << "  Tensor shape: [" << m_top_count << " x " << m_top_count << "]" << std::endl;
    oss << "  Principle: 27 answers → 4 sinus scalars → 72 quality parameters" << std::endl;
    return oss.str();
}

} // namespace cyborg