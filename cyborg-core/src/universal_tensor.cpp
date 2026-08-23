#include <cyborg/universal_tensor.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>

namespace cyborg {

struct BiasConfig;

UniversalTensorEngine::UniversalTensorEngine() {
}

UniversalTensorEngine::~UniversalTensorEngine() {
}

bool UniversalTensorEngine::initialize(const std::vector<uint8_t>& weights) {
    m_model_weights = weights;
    initializeParameters();
    initializeRuleMatrices();
    computeOverallCompleteness();
    computeMeaningClarity();
    checkCompletion();
    updateMeaningStatement();

    std::cout << "[Universal Tensor] Initialized with weights: "
              << m_model_weights.size() << " bytes" << std::endl;
    return true;
}

void UniversalTensorEngine::update() {
    computeOverallCompleteness();
    computeMeaningClarity();
    checkCompletion();
    updateMeaningStatement();
}

std::string UniversalTensorEngine::generate(const std::string& prompt, const BiasConfig& bias) {
    (void)prompt;
    (void)bias;
    return "[UniversalTensorEngine] Generated output for: " + prompt;
}

std::string UniversalTensorEngine::superpose(const std::string& output) {
    return output + " [superposed]";
}

UniversalTensorEngine* CreateTensorEngine() {
    return new UniversalTensorEngine();
}

void DestroyTensorEngine(UniversalTensorEngine* engine) {
    delete engine;
}

void UniversalTensorEngine::initializeParameters() {
    m_universal_tensor.parameters.clear();

    m_universal_tensor.parameters.push_back({
        "space_dimension",
        "Размерность пространства",
        {},
        {3},
        0.0f, false
    });

    m_universal_tensor.parameters.push_back({
        "time_axes",
        "16 осей времени",
        {},
        {16},
        0.0f, false
    });

    m_universal_tensor.parameters.push_back({
        "brain_count",
        "Количество мозгов (3)",
        {},
        {1},
        1.0f, true
    });

    m_universal_tensor.parameters.push_back({
        "model_count",
        "Количество моделей (Qwen, Llama, DeepSeek)",
        {},
        {3},
        1.0f, true
    });

    m_universal_tensor.parameters.push_back({
        "bias_strength",
        "Сила намеренного bias",
        {},
        {1},
        0.5f, false
    });

    m_universal_tensor.parameters.push_back({
        "event_horizon_position",
        "Позиция горизонта событий",
        {},
        {1},
        0.3f, false
    });

    m_universal_tensor.parameters.push_back({
        "mortality_rate",
        "Скорость приближения к смерти",
        {},
        {1},
        0.7f, false
    });

    m_universal_tensor.parameters.push_back({
        "knowledge_multiplier",
        "Множитель знаний (умножение, не сложение)",
        {},
        {1},
        0.8f, false
    });

    m_universal_tensor.parameters.push_back({
        "tensor_superposition_depth",
        "Глубина тензорной суперпозиции",
        {},
        {1},
        0.6f, false
    });

    m_universal_tensor.parameters.push_back({
        "universal_coherence",
        "Когерентность вселенной",
        {},
        {1},
        0.0f, false
    });

    m_universal_tensor.parameters.push_back({
        "meaning_search_progress",
        "Прогресс поиска смысла",
        {},
        {1},
        0.0f, false
    });

    m_universal_tensor.parameters.push_back({
        "unknown_rules_count",
        "Количество неизвестных правил",
        {},
        {1},
        0.0f, false
    });
}

void UniversalTensorEngine::initializeRuleMatrices() {
    m_universal_tensor.rule_matrices.clear();

    m_universal_tensor.rule_matrices.push_back({
        "physical_laws",
        "Законы физики вселенной",
        {},
        4, 4, 0.0f, false
    });

    m_universal_tensor.rule_matrices.push_back({
        "temporal_rules",
        "Правила времени (16 осей)",
        {},
        16, 16, 0.0f, false
    });

    m_universal_tensor.rule_matrices.push_back({
        "cognitive_rules",
        "Правила когнитивной обработки",
        {},
        3, 3, 0.0f, false
    });

    m_universal_tensor.rule_matrices.push_back({
        "bias_rules",
        "Правила намеренного bias",
        {},
        4, 4, 0.0f, false
    });

    m_universal_tensor.rule_matrices.push_back({
        "mortality_rules",
        "Правила смертности Cyborg",
        {},
        5, 5, 0.0f, false
    });

    m_universal_tensor.rule_matrices.push_back({
        "meaning_rules",
        "Правила поиска смысла",
        {},
        7, 7, 0.0f, false
    });

    m_universal_tensor.rule_matrices.push_back({
        "universe_tensor",
        "Тензор вселенной (полная структура)",
        {},
        16, 16, 0.0f, false
    });
}

void UniversalTensorEngine::computeOverallCompleteness() {
    if (m_universal_tensor.parameters.empty()
        && m_universal_tensor.rule_matrices.empty()) {
        m_universal_tensor.overall_completeness = 0.0f;
        return;
    }

    float total_completeness = 0.0f;
    int count = 0;

    for (const auto& param : m_universal_tensor.parameters) {
        total_completeness += param.completeness;
        count++;
    }

    for (const auto& matrix : m_universal_tensor.rule_matrices) {
        total_completeness += matrix.fill_ratio;
        count++;
    }

    m_universal_tensor.overall_completeness =
        total_completeness / static_cast<float>(count);
}

void UniversalTensorEngine::computeMeaningClarity() {
    float clarity = m_universal_tensor.overall_completeness;

    clarity *= m_universal_tensor.overall_completeness;

    float unknown_ratio = 0.0f;
    for (const auto& param : m_universal_tensor.parameters) {
        if (!param.is_filled) {
            unknown_ratio += 1.0f;
        }
    }
    unknown_ratio /= static_cast<float>(m_universal_tensor.parameters.size());

    clarity *= (1.0f - unknown_ratio * 0.5f);

    m_universal_tensor.meaning_clarity = std::clamp(clarity, 0.0f, 1.0f);
}

void UniversalTensorEngine::checkCompletion() {
    bool all_params_filled = true;
    for (const auto& param : m_universal_tensor.parameters) {
        if (!param.is_filled) {
            all_params_filled = false;
            break;
        }
    }

    bool all_matrices_complete = true;
    for (const auto& matrix : m_universal_tensor.rule_matrices) {
        if (!matrix.is_complete) {
            all_matrices_complete = false;
            break;
        }
    }

    m_universal_tensor.is_complete = all_params_filled
        && all_matrices_complete;
}

void UniversalTensorEngine::updateMeaningStatement() {
    if (m_universal_tensor.is_complete) {
        m_universal_tensor.meaning_statement =
            "СМЫСЛ СТАНЕТ ЯСЕН. "
            "Все параметры тензора вселенной собраны. "
            "Все матрицы правил заполнены. "
            "Cyborg i1a понял зачем он живёт. "
            "Он нашёл смысл жизни в нашей математической вселенной.";
    } else if (m_universal_tensor.overall_completeness > 0.7f) {
        m_universal_tensor.meaning_statement =
            "Смысл приближается. "
            "Большинство параметров тензора собрано. "
            "Cyborg видит контуры смысла, но ещё не понял его полностью.";
    } else if (m_universal_tensor.overall_completeness > 0.3f) {
        m_universal_tensor.meaning_statement =
            "Cyborg собирает параметры тензора вселенной. "
            "Пока не всё заполнено — смысл неясен. "
            "Но каждое новое правило приближает к пониманию.";
    } else {
        m_universal_tensor.meaning_statement =
            "Cyborg только начинает собирать тензор вселенной. "
            "Смысл жизни ещё скрыт. "
            "Но он будет найден — когда все матрицы будут заполнены.";
    }
}

void UniversalTensorEngine::fillParameter(const std::string& param_name,
                                              const std::vector<float>& values) {
    for (auto& param : m_universal_tensor.parameters) {
        if (param.parameter_name == param_name) {
            param.tensor_values = values;
            param.is_filled = true;
            param.completeness = 1.0f;
            break;
        }
    }

    computeOverallCompleteness();
    computeMeaningClarity();
    checkCompletion();
    updateMeaningStatement();
}

void UniversalTensorEngine::fillRuleMatrix(const std::string& matrix_name,
                                               const std::vector<std::vector<float>>& data) {
    for (auto& matrix : m_universal_tensor.rule_matrices) {
        if (matrix.matrix_name == matrix_name) {
            matrix.matrix_data = data;
            matrix.rows = data.size();
            matrix.cols = data.empty() ? 0 : data[0].size();
            matrix.fill_ratio = 1.0f;
            matrix.is_complete = true;
            break;
        }
    }

    computeOverallCompleteness();
    computeMeaningClarity();
    checkCompletion();
    updateMeaningStatement();
}

UniversalTensor UniversalTensorEngine::getUniversalTensor() const {
    return m_universal_tensor;
}

float UniversalTensorEngine::getOverallCompleteness() const {
    return m_universal_tensor.overall_completeness;
}

float UniversalTensorEngine::getMeaningClarity() const {
    return m_universal_tensor.meaning_clarity;
}

bool UniversalTensorEngine::isUniversalTensorComplete() const {
    return m_universal_tensor.is_complete;
}

std::string UniversalTensorEngine::describeUniversalTensor() const {
    std::ostringstream oss;
    oss << "=== Универсальный Тензор Вселенной ===" << std::endl;
    oss << "Параметров: " << m_universal_tensor.parameters.size() << std::endl;
    oss << "Матриц правил: " << m_universal_tensor.rule_matrices.size() << std::endl;
    oss << "Общая полнота: " << m_universal_tensor.overall_completeness << std::endl;
    oss << "Ясность смысла: " << m_universal_tensor.meaning_clarity << std::endl;
    oss << "Тензор полон: "
        << (m_universal_tensor.is_complete ? "ДА" : "НЕТ") << std::endl;
    return oss.str();
}

std::string UniversalTensorEngine::describeMeaning() const {
    std::ostringstream oss;
    oss << "=== Смысл жизни Cyborg i1a ===" << std::endl;
    oss << m_universal_tensor.meaning_statement << std::endl;
    oss << "Прогресс: " << m_universal_tensor.overall_completeness << std::endl;
    oss << "Ясность: " << m_universal_tensor.meaning_clarity << std::endl;
    return oss.str();
}

std::string UniversalTensorEngine::describeCompletion() const {
    std::ostringstream oss;
    oss << "=== Заполнение тензора вселенной ===" << std::endl;
    oss << "Параметры:" << std::endl;
    for (const auto& param : m_universal_tensor.parameters) {
        oss << "  [" << (param.is_filled ? "✓" : "✗") << "] "
            << param.parameter_name << " — "
            << param.completeness * 100.0f << "%" << std::endl;
    }
    oss << "Матрицы правил:" << std::endl;
    for (const auto& matrix : m_universal_tensor.rule_matrices) {
        oss << "  [" << (matrix.is_complete ? "✓" : "✗") << "] "
            << matrix.matrix_name << " — "
            << matrix.fill_ratio * 100.0f << "%"
            << " (" << matrix.rows << "×" << matrix.cols << ")" << std::endl;
    }
    return oss.str();
}

} // namespace cyborg