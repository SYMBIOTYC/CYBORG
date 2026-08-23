#pragma once

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>

namespace cyborg {

enum class LifecycleStage {
    Birth,
    Infancy,
    Growth,
    Maturity,
    Reproduction,
    Legacy
};

enum class LearningMode {
    Accumulation,
    Multiplication
};

struct ParentKnowledge {
    std::string parent_name;
    std::string parent_type;
    std::vector<float> knowledge_weights;
    std::vector<std::string> learned_patterns;
    float influence_strength;
};

struct CyborgIdentity {
    std::string name;
    std::string lineage;
    LifecycleStage stage;
    LearningMode learning_mode;
    float multiplication_factor;
    std::vector<ParentKnowledge> parents;
    std::vector<std::string> experiences;
    std::vector<std::string> trials;
};

class CyborgLifecycle {
public:
    CyborgLifecycle();
    ~CyborgLifecycle();

    void birth(const std::string& name, const std::vector<ParentKnowledge>& parents);
    void grow(const std::string& experience);
    void learn(const std::string& trial, float result);
    void multiplyKnowledge();
    void reproduce(CyborgLifecycle& offspring);
    void advanceStage();

    LifecycleStage getStage() const;
    float getMultiplicationFactor() const;
    const CyborgIdentity& getIdentity() const;

    std::string summarize() const;

private:
    CyborgIdentity m_identity;
    float m_knowledge_accumulator;
    float m_knowledge_multiplier;

    void applyParentInfluence();
    float computeMultiplicationGain() const;
    void recordExperience(const std::string& experience);
    void recordTrial(const std::string& trial, float result);
};

} // namespace cyborg