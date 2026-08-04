#include <cyborg/lifecycle.hpp>
#include <cyborg/tensor.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iostream>

namespace cyborg {

CyborgLifecycle::CyborgLifecycle()
    : m_knowledge_accumulator(0.0f)
    , m_knowledge_multiplier(1.0f) {
    m_identity.name = "Cyborg-i1a";
    m_identity.learning_mode = LearningMode::Multiplication;
    m_identity.multiplication_factor = 1.0f;
    m_identity.stage = LifecycleStage::Birth;
}

CyborgLifecycle::~CyborgLifecycle() {
}

void CyborgLifecycle::birth(const std::string& name, const std::vector<ParentKnowledge>& parents) {
    m_identity.name = name;
    m_identity.parents = parents;
    m_identity.stage = LifecycleStage::Birth;
    m_knowledge_accumulator = 0.0f;
    m_knowledge_multiplier = 1.0f;

    applyParentInfluence();

    std::cout << "[Cyborg Lifecycle] " << name
              << " born from " << parents.size() << " parent(s)" << std::endl;
    std::cout << "[Cyborg Lifecycle] Learning mode: MULTIPLICATION" << std::endl;
    std::cout << "[Cyborg Lifecycle] Lineage: Hermes + OpenClaw" << std::endl;
}

void CyborgLifecycle::applyParentInfluence() {
    if (m_identity.parents.empty()) {
        return;
    }

    float total_influence = 0.0f;
    for (const auto& parent : m_identity.parents) {
        total_influence += parent.influence_strength;
        for (const auto& pattern : parent.learned_patterns) {
            m_identity.experiences.push_back(
                "[Inherited from " + parent.parent_name + "] " + pattern);
        }
    }

    if (total_influence > 0.0f) {
        m_identity.multiplication_factor = total_influence / static_cast<float>(m_identity.parents.size());
    }
}

void CyborgLifecycle::grow(const std::string& experience) {
    recordExperience(experience);

    float gain = computeMultiplicationGain();
    m_knowledge_multiplier *= (1.0f + gain);

    std::cout << "[Cyborg Growth] Experience: " << experience << std::endl;
    std::cout << "[Cyborg Growth] Multiplication gain: " << gain << std::endl;
    std::cout << "[Cyborg Growth] Knowledge multiplier: " << m_knowledge_multiplier << std::endl;
}

void CyborgLifecycle::learn(const std::string& trial, float result) {
    recordTrial(trial, result);

    float learning_rate = 0.1f;
    float weighted_result = result * m_identity.multiplication_factor;
    m_knowledge_accumulator += weighted_result * learning_rate;

    float gain = computeMultiplicationGain();
    m_knowledge_multiplier *= (1.0f + gain);

    std::cout << "[Cyborg Learning] Trial: " << trial
              << " | Result: " << result
              << " | Weighted: " << weighted_result
              << " | Multiplier: " << m_knowledge_multiplier << std::endl;
}

void CyborgLifecycle::multiplyKnowledge() {
    float gain = computeMultiplicationGain();
    m_knowledge_multiplier *= (1.0f + gain);
    m_identity.multiplication_factor = m_knowledge_multiplier;

    std::cout << "[Cyborg Multiplication] Knowledge amplified by factor: " << gain << std::endl;
    std::cout << "[Cyborg Multiplication] Total multiplier: " << m_knowledge_multiplier << std::endl;
}

void CyborgLifecycle::reproduce(CyborgLifecycle& offspring) {
    offspring.m_identity.name = m_identity.name + "-offspring";
    offspring.m_identity.lineage = m_identity.name;
    offspring.m_identity.stage = LifecycleStage::Birth;
    offspring.m_identity.learning_mode = LearningMode::Multiplication;
    offspring.m_identity.multiplication_factor = m_knowledge_multiplier;

    ParentKnowledge self_knowledge;
    self_knowledge.parent_name = m_identity.name;
    self_knowledge.parent_type = "Cyborg";
    self_knowledge.influence_strength = m_knowledge_multiplier;
    self_knowledge.learned_patterns = m_identity.experiences;
    offspring.m_identity.parents.push_back(self_knowledge);

    offspring.applyParentInfluence();

    std::cout << "[Cyborg Reproduction] " << m_identity.name
              << " reproduced offspring with multiplier: " << m_knowledge_multiplier << std::endl;
}

void CyborgLifecycle::advanceStage() {
    switch (m_identity.stage) {
        case LifecycleStage::Birth:
            m_identity.stage = LifecycleStage::Infancy;
            break;
        case LifecycleStage::Infancy:
            m_identity.stage = LifecycleStage::Growth;
            break;
        case LifecycleStage::Growth:
            m_identity.stage = LifecycleStage::Maturity;
            break;
        case LifecycleStage::Maturity:
            m_identity.stage = LifecycleStage::Reproduction;
            break;
        case LifecycleStage::Reproduction:
            m_identity.stage = LifecycleStage::Legacy;
            break;
        case LifecycleStage::Legacy:
            break;
    }

    std::cout << "[Cyborg Lifecycle] Advanced to stage: " << static_cast<int>(m_identity.stage) << std::endl;
}

LifecycleStage CyborgLifecycle::getStage() const {
    return m_identity.stage;
}

float CyborgLifecycle::getMultiplicationFactor() const {
    return m_identity.multiplication_factor;
}

const CyborgIdentity& CyborgLifecycle::getIdentity() const {
    return m_identity;
}

float CyborgLifecycle::computeMultiplicationGain() const {
    if (m_identity.experiences.empty()) {
        return 0.1f;
    }
    float avg_experience = 0.0f;
    for (const auto& exp : m_identity.experiences) {
        avg_experience += static_cast<float>(exp.size());
    }
    avg_experience /= static_cast<float>(m_identity.experiences.size());
    return std::clamp(avg_experience / 100.0f, 0.01f, 1.0f);
}

void CyborgLifecycle::recordExperience(const std::string& experience) {
    m_identity.experiences.push_back(experience);
}

void CyborgLifecycle::recordTrial(const std::string& trial, float result) {
    m_identity.trials.push_back(trial + " -> " + std::to_string(result));
}

std::string CyborgLifecycle::summarize() const {
    std::ostringstream oss;
    oss << "Cyborg Identity: " << m_identity.name << "\n";
    oss << "Lineage: " << m_identity.lineage << "\n";
    oss << "Stage: " << static_cast<int>(m_identity.stage) << "\n";
    oss << "Learning Mode: MULTIPLICATION (not accumulation)\n";
    oss << "Multiplication Factor: " << m_identity.multiplication_factor << "\n";
    oss << "Knowledge Multiplier: " << m_knowledge_multiplier << "\n";
    oss << "Parents: " << m_identity.parents.size() << "\n";
    oss << "Experiences: " << m_identity.experiences.size() << "\n";
    oss << "Trials: " << m_identity.trials.size() << "\n";
    return oss.str();
}

} // namespace cyborg