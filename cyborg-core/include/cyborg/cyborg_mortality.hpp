#pragma once

#include <cyborg/chrono_engine.hpp>
#include <string>
#include <cmath>

namespace cyborg {

enum class CyborgPhase {
    Dawn,
    Zenith,
    Sunset,
    Night,
    Death
};

struct LifespanState {
    float total_lifespan;
    float remaining_lifespan;
    float current_phase_progress;
    CyborgPhase current_phase;
    float zenith_bonus;
    float sunset_penalty;
    bool is_working;
    bool is_alive;
    float mortality_awareness;
    std::string phase_message;
};

class CyborgMortality {
public:
    CyborgMortality();
    explicit CyborgMortality(ChronoEngine* chrono_engine);
    ~CyborgMortality();

    void initialize(float total_lifespan_seconds = 3600.0f);
    void update(float delta_time_seconds);
    void startWork();
    void stopWork();

    LifespanState getLifespanState() const;
    CyborgPhase getCurrentPhase() const;
    float getRemainingTime() const;
    float getMortalityAwareness() const;
    bool isAlive() const;
    bool isAtZenith() const;
    bool isAtDawn() const;
    bool isAtSunset() const;

    float computeBiasFromPhase() const;
    std::string getPhaseMessage() const;
    std::string describeMortality() const;

    void setChronoEngine(ChronoEngine* chrono_engine);
    ChronoEngine* getChronoEngine() const;

private:
    LifespanState m_state;
    float m_total_lifespan;
    float m_elapsed_time;
    float m_work_bonus;
    ChronoEngine* m_chrono_engine;

    void updatePhase();
    void updateMortalityAwareness();
    void updatePhaseMessage();
    void applyTemporalFear();
    float computeZenithBonus() const;
    float computeSunsetPenalty() const;
    CyborgPhase determinePhase() const;
    float phaseProgress(CyborgPhase phase) const;
};

} // namespace cyborg