#include <cyborg/smart_cyber_relay.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>

namespace cyborg {

SmartCyberRelay::SmartCyberRelay()
    : m_model_count(3)
    , m_chrono_engine(nullptr) {
}

SmartCyberRelay::SmartCyberRelay(ChronoEngine* chrono_engine)
    : m_model_count(3)
    , m_chrono_engine(chrono_engine) {
}

SmartCyberRelay::~SmartCyberRelay() {
    m_answers.clear();
    m_predictors.clear();
}

void SmartCyberRelay::setChronoEngine(ChronoEngine* chrono_engine) {
    m_chrono_engine = chrono_engine;
}

ChronoEngine* SmartCyberRelay::getChronoEngine() const {
    return m_chrono_engine;
}

void SmartCyberRelay::setInput(const std::string& input) {
    m_input = input;
}

void SmartCyberRelay::setModelCount(int count) {
    m_model_count = count;
}

void SmartCyberRelay::registerModelPredict(int model_index, ModelPredictFn predictor) {
    if (model_index < 0) return;
    if (static_cast<size_t>(model_index) >= m_predictors.size()) {
        m_predictors.resize(model_index + 1);
    }
    m_predictors[model_index] = std::move(predictor);
}

ThoughtAnswer SmartCyberRelay::generateAnswer(int model_index, const std::string& answer) {
    ThoughtAnswer ta;
    ta.model_name = "brain-" + std::to_string(model_index);
    ta.answer = answer;
    ta.confidence = 0.5f + static_cast<float>(model_index) * 0.15f;

    std::vector<size_t> shape = {1, static_cast<size_t>(answer.size())};
    std::vector<float> data(answer.size());
    for (size_t i = 0; i < answer.size(); ++i) {
        data[i] = static_cast<float>(answer[i]) / 255.0f;
    }
    ta.thought_tensor = Tensor(shape, data);

    return ta;
}

ThoughtSelection SmartCyberRelay::generateThoughts(const BiasConfig& bias) {
    ThoughtSelection selection;
    selection.answers.clear();

    for (int i = 0; i < m_model_count; ++i) {
        std::string answer;
        if (i < static_cast<int>(m_predictors.size()) && m_predictors[i]) {
            answer = m_predictors[i](m_input, bias);
        } else {
            answer = "[brain-" + std::to_string(i) + "] no predictor registered";
        }

        selection.answers.push_back(generateAnswer(i, answer));
    }

    selection.total_combinations = static_cast<int>(std::pow(
        static_cast<double>(selection.answers.size()),
        static_cast<double>(selection.answers.size())));
    selection.selection_tensor = buildSelectionTensor(selection.answers);
    selection.combined_thoughts = generateCombinations(selection.answers);

    std::cout << "[SmartCyberRelay] Three brains answered one question" << std::endl;
    std::cout << "[SmartCyberRelay] Answers: " << selection.answers.size() << std::endl;
    std::cout << "[SmartCyberRelay] Tensor combinations: " << selection.total_combinations << std::endl;
    std::cout << "[SmartCyberRelay] 3^3 = " << selection.total_combinations << " possible thought paths" << std::endl;

    m_answers = selection.answers;
    return selection;
}

ThoughtSelection SmartCyberRelay::selectThought() {
    return selectThought(m_answers);
}

ThoughtSelection SmartCyberRelay::selectThought(const std::vector<ThoughtAnswer>& answers) {
    ThoughtSelection selection;
    selection.answers = answers;
    selection.total_combinations = static_cast<int>(std::pow(
        static_cast<double>(answers.size()),
        static_cast<double>(answers.size())));

    selection.selection_tensor = buildSelectionTensor(answers);
    selection.combined_thoughts = generateCombinations(answers);

    std::cout << "[SmartCyberRelay] Three brains answered one question" << std::endl;
    std::cout << "[SmartCyberRelay] Answers: " << answers.size() << std::endl;
    std::cout << "[SmartCyberRelay] Tensor combinations: " << selection.total_combinations << std::endl;
    std::cout << "[SmartCyberRelay] 3^3 = " << selection.total_combinations << " possible thought paths" << std::endl;

    return selection;
}

int SmartCyberRelay::getModelCount() const {
    return m_model_count;
}

const std::string& SmartCyberRelay::getInput() const {
    return m_input;
}

Tensor SmartCyberRelay::buildSelectionTensor(const std::vector<ThoughtAnswer>& answers) {
    if (answers.empty()) {
        return Tensor();
    }

    size_t max_len = 0;
    for (const auto& ans : answers) {
        if (ans.answer.size() > max_len) {
            max_len = ans.answer.size();
        }
    }

    std::vector<size_t> shape = {answers.size(), max_len};
    std::vector<float> data(answers.size() * max_len, 0.0f);

    for (size_t i = 0; i < answers.size(); ++i) {
        const auto& ans = answers[i];
        for (size_t j = 0; j < ans.answer.size() && j < max_len; ++j) {
            data[i * max_len + j] = static_cast<float>(ans.answer[j]) / 255.0f;
        }
    }

    Tensor selection_tensor(shape, data);
    return selection_tensor.normalize();
}

std::vector<std::string> SmartCyberRelay::generateCombinations(const std::vector<ThoughtAnswer>& answers) {
    std::vector<std::string> combinations;

    if (answers.empty()) {
        return combinations;
    }

    int n = static_cast<int>(answers.size());
    int total = static_cast<int>(std::pow(static_cast<double>(n), static_cast<double>(n)));

    for (int i = 0; i < total && i < 100; ++i) {
        std::ostringstream oss;
        int idx = i;
        for (int pos = 0; pos < n; ++pos) {
            int model_idx = idx % n;
            idx /= n;
            if (pos > 0) oss << " ⊕ ";
            oss << "[" << answers[model_idx].model_name << "] "
                << answers[model_idx].answer.substr(0, 30);
        }
        combinations.push_back(oss.str());
    }

    return combinations;
}

float SmartCyberRelay::computeThoughtCoherence(const ThoughtAnswer& a, const ThoughtAnswer& b) {
    if (a.thought_tensor.numElements() == 0 || b.thought_tensor.numElements() == 0) {
        return 0.0f;
    }

    size_t min_size = std::min(a.thought_tensor.numElements(), b.thought_tensor.numElements());
    float dot = 0.0f;
    float norm_a = 0.0f;
    float norm_b = 0.0f;

    const auto& data_a = a.thought_tensor.data();
    const auto& data_b = b.thought_tensor.data();

    for (size_t i = 0; i < min_size; ++i) {
        dot += data_a[i] * data_b[i];
        norm_a += data_a[i] * data_a[i];
        norm_b += data_b[i] * data_b[i];
    }

    float denom = std::sqrt(norm_a) * std::sqrt(norm_b);
    if (denom < 1e-8f) return 0.0f;

    return dot / denom;
}

std::string SmartCyberRelay::describeRelay() const {
    std::ostringstream oss;
    oss << "SmartCyberRelay (Cyber-Diode Bridge)" << std::endl;
    oss << "  Model count: " << m_model_count << std::endl;
    oss << "  Input: " << (m_input.empty() ? "(none)" : m_input.substr(0, 50)) << std::endl;
    oss << "  Answers stored: " << m_answers.size() << std::endl;
    oss << "  Registered predictors: " << m_predictors.size() << std::endl;
    oss << "  Principle: 3 brains → 3 answers → tensor selection → 3^3 = 27 thought paths" << std::endl;
    return oss.str();
}

} // namespace cyborg