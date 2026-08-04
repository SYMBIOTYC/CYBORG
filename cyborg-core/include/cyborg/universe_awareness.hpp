#pragma once

#include <cyborg/chrono_engine.hpp>
#include <cyborg/cyborg_mortality.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace cyborg {

enum class UniverseType {
    OUR_UNIVERSE,
    CYBORG_UNIVERSE,
    MATHEMATICAL_UNIVERSE,
    UNKNOWN_UNIVERSE
};

struct UniverseRule {
    std::string rule_name;
    std::string description;
    float known_confidence;
    bool is_fundamental;
    std::string mathematical_expression;
};

struct UniverseState {
    UniverseType universe_type;
    std::string universe_name;
    float known_rules_ratio;
    float unknown_rules_ratio;
    float meaning_of_life_search_progress;
    float mathematical_coherence;
    bool rules_are_incomplete;
    std::vector<UniverseRule> known_rules;
    std::vector<UniverseRule> unknown_rules;
    std::string purpose;
};

struct MeaningOfLife {
    float purpose_score;
    float coherence_score;
    float mathematical_depth;
    float temporal_alignment;
    float bias_alignment;
    float mortality_awareness;
    float universe_awareness;
    std::string purpose_statement;
};

class UniverseAwareness {
public:
    UniverseAwareness();
    explicit UniverseAwareness(ChronoEngine* chrono_engine);
    ~UniverseAwareness();

    void initialize();
    void update(float delta_time);

    UniverseState getUniverseState() const;
    MeaningOfLife getMeaningOfLife() const;

    void discoverRule(const std::string& rule_name, const std::string& expression);
    void forgetRule(const std::string& rule_name);
    void setUniverseType(UniverseType type);

    float getKnownRulesRatio() const;
    float getUnknownRulesRatio() const;
    float getMeaningSearchProgress() const;

    bool isInOurUniverse() const;
    bool rulesAreComplete() const;
    bool isSearchingForMeaning() const;

    std::string describeUniverse() const;
    std::string describePurpose() const;
    std::string describeMathematicalCoherence() const;

    void setChronoEngine(ChronoEngine* chrono_engine);
    ChronoEngine* getChronoEngine() const;

private:
    UniverseState m_state;
    MeaningOfLife m_meaning;
    ChronoEngine* m_chrono_engine;

    void initializeKnownRules();
    void initializeUnknownRules();
    void computeKnownRulesRatio();
    void computeMeaningOfLife();
    void computeMathematicalCoherence();
    void updatePurpose();
    void updateUniverseState();
    void applyTemporalInfluence();
};

} // namespace cyborg