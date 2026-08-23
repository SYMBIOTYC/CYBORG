#pragma once

#include <cyborg/tensor.hpp>
#include <string>
#include <vector>
#include <array>
#include <chrono>
#include <ctime>
#include <cmath>
#include <unordered_map>

namespace cyborg {

enum class TemporalAxis {
    PAST_TIME,
    ELAPSED_TIME,
    SPENT_TIME,
    INVESTED_TIME,
    PAID_TIME,
    GONE_TIME,
    REMAINING_TIME,
    FUTURE_TIME,
    PRESENT_MOMENT,
    DECISION_TIME,
    RESPONSE_TIME,
    THOUGHT_TIME,
    WAVE_PERIOD,
    WAVE_FREQUENCY,
    EVENT_HORIZON_TIME,
    TEMPORAL_CONTINUUM
};

constexpr int TEMPORAL_AXES_COUNT = 16;

struct TemporalWave {
    float amplitude;
    float wavelength;
    float frequency;
    float phase;
    float thickness;
    TemporalAxis axis;
    std::string label;
};

struct EventHorizonWave {
    float position;
    float speed;
    float width;
    bool crossed;
    float time_remaining;
    std::string warning;
};

struct ChronoAxis {
    TemporalAxis axis;
    std::string name;
    std::string russian_name;
    float value;
    float weight;
    bool is_fundamental;
};

struct ChronoMapEntry {
    std::string activity;
    float start_time;
    float duration;
    float temporal_density;
    TemporalAxis primary_axis;
};

struct ChronoGram {
    std::vector<ChronoMapEntry> entries;
    float total_temporal_mass;
    float temporal_velocity;
    float temporal_acceleration;
    std::string day_label;
    int day_of_week;
    int hour;
    int minute;
    float time_pressure;
};

struct TemporalFearState {
    float fear_level;
    float horizon_proximity;
    bool horizon_crossed;
    float time_remaining_ms;
    float thought_velocity;
    float wave_collapse_risk;
    std::string urgency_message;
};

class ChronoEngine {
public:
    ChronoEngine();
    ~ChronoEngine();

    void initialize();
    void updateTime();

    std::array<float, TEMPORAL_AXES_COUNT> getTemporalAxes() const;
    std::vector<TemporalWave> getTemporalWaves() const;
    EventHorizonWave getEventHorizon() const;
    ChronoGram getChronoGram() const;
    TemporalFearState getTemporalFear() const;

    float getTimeAxisValue(TemporalAxis axis) const;
    void setTimeAxisValue(TemporalAxis axis, float value);

    Tensor encodeTemporalState() const;
    float computeTemporalPressure() const;
    bool isHorizonImminent() const;
    bool hasHorizonCrossed() const;

    std::string getDayLabel() const;
    int getDayOfWeek() const;
    int getHour() const;
    int getMinute() const;

    std::string describeChronoMap() const;
    std::string describeTemporalFear() const;
    const char* getAxisLabel(TemporalAxis axis) const;

private:
    std::array<float, TEMPORAL_AXES_COUNT> m_temporal_axes;
    std::vector<TemporalWave> m_temporal_waves;
    EventHorizonWave m_event_horizon;
    ChronoGram m_chrono_gram;
    TemporalFearState m_temporal_fear;

    std::chrono::system_clock::time_point m_start_time;
    std::chrono::system_clock::time_point m_last_update;

    void initializeTemporalAxes();
    void initializeTemporalWaves();
    void initializeEventHorizon();
    void computeTemporalWaves();
    void computeEventHorizon();
    void computeTemporalFear();
    void updateChronoGram();
    float computeWaveAmplitude(TemporalAxis axis, float t) const;
    float computeWaveWavelength(TemporalAxis axis) const;
    float computeWaveFrequency(TemporalAxis axis) const;
    float computeWaveThickness(TemporalAxis axis) const;
};

} // namespace cyborg