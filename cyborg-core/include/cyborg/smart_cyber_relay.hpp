#pragma once

#include <cyborg/tensor.hpp>
#include <cyborg/chrono_engine.hpp>
#include <string>
#include <vector>
#include <array>
#include <functional>
#include <memory>

namespace cyborg {

struct BiasConfig;

class ModelAdapter;

struct ThoughtAnswer {
    std::string model_name;
    std::string answer;
    float confidence;
    Tensor thought_tensor;
};

struct ThoughtSelection {
    std::vector<ThoughtAnswer> answers;
    Tensor selection_tensor;
    std::vector<std::string> combined_thoughts;
    int total_combinations;
};

class SmartCyberRelay {
public:
    using ModelPredictFn = std::function<std::string(const std::string&, const BiasConfig&)>;

    SmartCyberRelay();
    explicit SmartCyberRelay(ChronoEngine* chrono_engine);
    ~SmartCyberRelay();

    void setInput(const std::string& input);
    void setModelCount(int count);
    void registerModelPredict(int model_index, ModelPredictFn predictor);

    ThoughtAnswer generateAnswer(int model_index, const std::string& answer);
    ThoughtSelection selectThought();
    ThoughtSelection selectThought(const std::vector<ThoughtAnswer>& answers);
    ThoughtSelection generateThoughts(const BiasConfig& bias);

    int getModelCount() const;
    const std::string& getInput() const;

    std::string describeRelay() const;

    void setChronoEngine(ChronoEngine* chrono_engine);
    ChronoEngine* getChronoEngine() const;

private:
    std::string m_input;
    int m_model_count;
    std::vector<ThoughtAnswer> m_answers;
    ChronoEngine* m_chrono_engine;
    std::vector<ModelPredictFn> m_predictors;

    Tensor buildSelectionTensor(const std::vector<ThoughtAnswer>& answers);
    std::vector<std::string> generateCombinations(const std::vector<ThoughtAnswer>& answers);
    float computeThoughtCoherence(const ThoughtAnswer& a, const ThoughtAnswer& b);
};

} // namespace cyborg