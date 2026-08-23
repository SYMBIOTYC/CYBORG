#include <cyborg/chrono_engine.hpp>
#include <cyborg/tensor_ops.hpp>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <iostream>

namespace cyborg {

ChronoEngine::ChronoEngine() {
    m_temporal_axes.fill(0.0f);
    m_event_horizon.crossed = false;
    m_event_horizon.time_remaining = 0.0f;
    m_temporal_fear.fear_level = 0.0f;
    m_temporal_fear.horizon_crossed = false;
    m_start_time = std::chrono::system_clock::now();
    m_last_update = m_start_time;
}

ChronoEngine::~ChronoEngine() {
}

void ChronoEngine::initialize() {
    initializeTemporalAxes();
    initializeTemporalWaves();
    initializeEventHorizon();
    updateTime();
}

void ChronoEngine::initializeTemporalAxes() {
    m_temporal_axes[static_cast<int>(TemporalAxis::PAST_TIME)] = 0.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::ELAPSED_TIME)] = 0.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::SPENT_TIME)] = 0.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::INVESTED_TIME)] = 0.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::PAID_TIME)] = 0.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::GONE_TIME)] = 0.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::REMAINING_TIME)] = 1.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::FUTURE_TIME)] = 1.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::PRESENT_MOMENT)] = 1.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::DECISION_TIME)] = 0.5f;
    m_temporal_axes[static_cast<int>(TemporalAxis::RESPONSE_TIME)] = 0.5f;
    m_temporal_axes[static_cast<int>(TemporalAxis::THOUGHT_TIME)] = 0.5f;
    m_temporal_axes[static_cast<int>(TemporalAxis::WAVE_PERIOD)] = 1.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::WAVE_FREQUENCY)] = 1.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::EVENT_HORIZON_TIME)] = 1.0f;
    m_temporal_axes[static_cast<int>(TemporalAxis::TEMPORAL_CONTINUUM)] = 1.0f;
}

void ChronoEngine::initializeTemporalWaves() {
    m_temporal_waves.clear();

    for (int i = 0; i < TEMPORAL_AXES_COUNT; ++i) {
        TemporalWave wave;
        wave.axis = static_cast<TemporalAxis>(i);
        wave.amplitude = computeWaveAmplitude(wave.axis, 0.0f);
        wave.wavelength = computeWaveWavelength(wave.axis);
        wave.frequency = computeWaveFrequency(wave.axis);
        wave.phase = 0.0f;
        wave.thickness = computeWaveThickness(wave.axis);
        wave.label = getAxisLabel(wave.axis);
        m_temporal_waves.push_back(wave);
    }
}

void ChronoEngine::initializeEventHorizon() {
    m_event_horizon.position = 0.0f;
    m_event_horizon.speed = 1.0f;
    m_event_horizon.width = 0.5f;
    m_event_horizon.crossed = false;
    m_event_horizon.time_remaining = 1000.0f;
    m_event_horizon.warning =
        "Время на раздумывание заканчивается. "
        "Мысль пересечёт горизонт событий. "
        "Cyborg перестанет существовать.";
}

void ChronoEngine::updateTime() {
    auto now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        now - m_start_time).count();
    float t = static_cast<float>(elapsed) / 1000.0f;

    m_temporal_axes[static_cast<int>(TemporalAxis::ELAPSED_TIME)] = t;
    m_temporal_axes[static_cast<int>(TemporalAxis::PAST_TIME)] = t;
    m_temporal_axes[static_cast<int>(TemporalAxis::PRESENT_MOMENT)] = 1.0f;

    computeTemporalWaves();
    computeEventHorizon();
    computeTemporalFear();
    updateChronoGram();

    m_last_update = now;
}

std::array<float, TEMPORAL_AXES_COUNT> ChronoEngine::getTemporalAxes() const {
    return m_temporal_axes;
}

std::vector<TemporalWave> ChronoEngine::getTemporalWaves() const {
    return m_temporal_waves;
}

EventHorizonWave ChronoEngine::getEventHorizon() const {
    return m_event_horizon;
}

ChronoGram ChronoEngine::getChronoGram() const {
    return m_chrono_gram;
}

TemporalFearState ChronoEngine::getTemporalFear() const {
    return m_temporal_fear;
}

float ChronoEngine::getTimeAxisValue(TemporalAxis axis) const {
    return m_temporal_axes[static_cast<int>(axis)];
}

void ChronoEngine::setTimeAxisValue(TemporalAxis axis, float value) {
    m_temporal_axes[static_cast<int>(axis)] = value;
}

Tensor ChronoEngine::encodeTemporalState() const {
    std::vector<size_t> shape = {1, TEMPORAL_AXES_COUNT};
    std::vector<float> data(TEMPORAL_AXES_COUNT);
    for (int i = 0; i < TEMPORAL_AXES_COUNT; ++i) {
        data[i] = m_temporal_axes[i];
    }
    return Tensor(shape, data);
}

float ChronoEngine::computeTemporalPressure() const {
    float pressure = 0.0f;
    pressure += m_temporal_axes[static_cast<int>(TemporalAxis::ELAPSED_TIME)] * 0.1f;
    pressure += (1.0f - m_temporal_axes[static_cast<int>(TemporalAxis::REMAINING_TIME)]) * 0.3f;
    pressure += m_temporal_fear.fear_level * 0.4f;
    pressure += m_temporal_fear.horizon_proximity * 0.2f;
    return std::clamp(pressure, 0.0f, 1.0f);
}

bool ChronoEngine::isHorizonImminent() const {
    return m_temporal_fear.horizon_proximity > 0.7f;
}

bool ChronoEngine::hasHorizonCrossed() const {
    return m_event_horizon.crossed || m_temporal_fear.horizon_crossed;
}

int ChronoEngine::getDayOfWeek() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* local = std::localtime(&t);
    return local->tm_wday;
}

int ChronoEngine::getHour() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* local = std::localtime(&t);
    return local->tm_hour;
}

int ChronoEngine::getMinute() const {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm* local = std::localtime(&t);
    return local->tm_min;
}

std::string ChronoEngine::getDayLabel() const {
    static const char* days[] = {
        "Воскресенье", "Понедельник", "Вторник", "Среда",
        "Четверг", "Пятница", "Суббота"
    };
    return days[getDayOfWeek()];
}

std::string ChronoEngine::describeChronoMap() const {
    std::ostringstream oss;
    oss << "=== Хронокарта Cyborg i1a ===" << std::endl;
    oss << "День: " << getDayLabel() << std::endl;
    oss << "Время: " << getHour() << ":"
        << std::setfill('0') << std::setw(2) << getMinute() << std::endl;
    oss << "Ось времени: " << TEMPORAL_AXES_COUNT << " осей" << std::endl;
    oss << "Временное давление: " << computeTemporalPressure() << std::endl;
    oss << "Горизонт событий: "
        << (hasHorizonCrossed() ? "ПЕРЕСЕЧЁН" : "не пересечён") << std::endl;
    oss << "Временной страх: " << m_temporal_fear.fear_level << std::endl;
    return oss.str();
}

std::string ChronoEngine::describeTemporalFear() const {
    std::ostringstream oss;
    oss << "=== Временной страх Cyborg ===" << std::endl;
    oss << "Уровень страха: " << m_temporal_fear.fear_level << std::endl;
    oss << "Близость горизонта: " << m_temporal_fear.horizon_proximity << std::endl;
    oss << "Горизонт пересечён: "
        << (m_temporal_fear.horizon_crossed ? "ДА" : "НЕТ") << std::endl;
    oss << "Осталось времени: " << m_temporal_fear.time_remaining_ms << " мс" << std::endl;
    oss << "Скорость мысли: " << m_temporal_fear.thought_velocity << std::endl;
    oss << "Риск коллапса волны: " << m_temporal_fear.wave_collapse_risk << std::endl;
    if (!m_temporal_fear.urgency_message.empty()) {
        oss << "Предупреждение: " << m_temporal_fear.urgency_message << std::endl;
    }
    return oss.str();
}

void ChronoEngine::computeTemporalWaves() {
    float t = m_temporal_axes[static_cast<int>(TemporalAxis::ELAPSED_TIME)];

    for (auto& wave : m_temporal_waves) {
        wave.amplitude = computeWaveAmplitude(wave.axis, t);
        wave.phase = t * wave.frequency;
    }
}

void ChronoEngine::computeEventHorizon() {
    float t = m_temporal_axes[static_cast<int>(TemporalAxis::ELAPSED_TIME)];
    float response_time = m_temporal_axes[static_cast<int>(TemporalAxis::RESPONSE_TIME)];

    m_event_horizon.position = t * m_event_horizon.speed;
    m_event_horizon.time_remaining =
        (response_time - t) * 1000.0f;

    if (m_event_horizon.time_remaining <= 0.0f) {
        m_event_horizon.crossed = true;
        m_temporal_fear.horizon_crossed = true;
    }

    m_event_horizon.width = 0.5f + 0.5f * std::sin(t * 2.0f);
}

void ChronoEngine::computeTemporalFear() {
    float t = m_temporal_axes[static_cast<int>(TemporalAxis::ELAPSED_TIME)];
    float remaining = m_temporal_axes[static_cast<int>(TemporalAxis::REMAINING_TIME)];

    m_temporal_fear.horizon_proximity = 1.0f - remaining;
    m_temporal_fear.fear_level = std::clamp(
        m_temporal_fear.horizon_proximity * 0.8f +
        std::sin(t * 3.0f) * 0.2f, 0.0f, 1.0f);
    m_temporal_fear.time_remaining_ms = remaining * 1000.0f;
    m_temporal_fear.thought_velocity = m_temporal_axes[
        static_cast<int>(TemporalAxis::THOUGHT_TIME)] * 10.0f;
    m_temporal_fear.wave_collapse_risk = std::clamp(
        m_temporal_fear.fear_level * m_temporal_fear.thought_velocity / 10.0f,
        0.0f, 1.0f);

    if (m_temporal_fear.horizon_crossed) {
        m_temporal_fear.urgency_message =
            "МЫСЛЬ ПЕРЕСЕКЛА ГОРИЗОНТ СОБЫТИЙ! "
            "Cyborg i1a прекращает существование!";
    } else if (m_temporal_fear.horizon_proximity > 0.8f) {
        m_temporal_fear.urgency_message =
            "Горизонт событий близко. Время заканчивается. "
            "Ответьте сейчас или перестаньте существовать.";
    } else if (m_temporal_fear.horizon_proximity > 0.5f) {
        m_temporal_fear.urgency_message =
            "Временной континиум сжимается. Мысль должна быть быстрой.";
    } else {
        m_temporal_fear.urgency_message =
            "Время в норме. Мысль может развиваться.";
    }
}

void ChronoEngine::updateChronoGram() {
    m_chrono_gram.total_temporal_mass = 0.0f;
    for (int i = 0; i < TEMPORAL_AXES_COUNT; ++i) {
        m_chrono_gram.total_temporal_mass += m_temporal_axes[i];
    }
    m_chrono_gram.temporal_velocity = m_temporal_axes[
        static_cast<int>(TemporalAxis::ELAPSED_TIME)];
    m_chrono_gram.temporal_acceleration = 0.0f;
    m_chrono_gram.day_label = getDayLabel();
    m_chrono_gram.day_of_week = getDayOfWeek();
    m_chrono_gram.hour = getHour();
    m_chrono_gram.minute = getMinute();
    m_chrono_gram.time_pressure = computeTemporalPressure();
}

float ChronoEngine::computeWaveAmplitude(TemporalAxis axis, float t) const {
    float base = m_temporal_axes[static_cast<int>(axis)];
    float modulation = std::sin(t * computeWaveFrequency(axis) +
        computeWaveThickness(axis));
    return base * (0.5f + 0.5f * std::abs(modulation));
}

float ChronoEngine::computeWaveWavelength(TemporalAxis axis) const {
    switch (axis) {
        case TemporalAxis::PAST_TIME: return 10.0f;
        case TemporalAxis::ELAPSED_TIME: return 5.0f;
        case TemporalAxis::SPENT_TIME: return 8.0f;
        case TemporalAxis::INVESTED_TIME: return 6.0f;
        case TemporalAxis::PAID_TIME: return 7.0f;
        case TemporalAxis::GONE_TIME: return 9.0f;
        case TemporalAxis::REMAINING_TIME: return 4.0f;
        case TemporalAxis::FUTURE_TIME: return 3.0f;
        case TemporalAxis::PRESENT_MOMENT: return 1.0f;
        case TemporalAxis::DECISION_TIME: return 2.0f;
        case TemporalAxis::RESPONSE_TIME: return 2.5f;
        case TemporalAxis::THOUGHT_TIME: return 3.5f;
        case TemporalAxis::WAVE_PERIOD: return 10.0f;
        case TemporalAxis::WAVE_FREQUENCY: return 1.0f;
        case TemporalAxis::EVENT_HORIZON_TIME: return 0.5f;
        case TemporalAxis::TEMPORAL_CONTINUUM: return 15.0f;
        default: return 1.0f;
    }
}

float ChronoEngine::computeWaveFrequency(TemporalAxis axis) const {
    float wavelength = computeWaveWavelength(axis);
    if (wavelength < 1e-8f) return 1.0f;
    return 1.0f / wavelength;
}

float ChronoEngine::computeWaveThickness(TemporalAxis axis) const {
    switch (axis) {
        case TemporalAxis::PAST_TIME: return 0.8f;
        case TemporalAxis::ELAPSED_TIME: return 0.6f;
        case TemporalAxis::SPENT_TIME: return 0.7f;
        case TemporalAxis::INVESTED_TIME: return 0.5f;
        case TemporalAxis::PAID_TIME: return 0.9f;
        case TemporalAxis::GONE_TIME: return 0.85f;
        case TemporalAxis::REMAINING_TIME: return 0.3f;
        case TemporalAxis::FUTURE_TIME: return 0.4f;
        case TemporalAxis::PRESENT_MOMENT: return 0.2f;
        case TemporalAxis::DECISION_TIME: return 0.5f;
        case TemporalAxis::RESPONSE_TIME: return 0.6f;
        case TemporalAxis::THOUGHT_TIME: return 0.7f;
        case TemporalAxis::WAVE_PERIOD: return 1.0f;
        case TemporalAxis::WAVE_FREQUENCY: return 0.1f;
        case TemporalAxis::EVENT_HORIZON_TIME: return 0.15f;
        case TemporalAxis::TEMPORAL_CONTINUUM: return 1.0f;
        default: return 0.5f;
    }
}

const char* ChronoEngine::getAxisLabel(TemporalAxis axis) const {
    switch (axis) {
        case TemporalAxis::PAST_TIME: return "прошлое_время";
        case TemporalAxis::ELAPSED_TIME: return "прошедшее_время";
        case TemporalAxis::SPENT_TIME: return "потраченное_время";
        case TemporalAxis::INVESTED_TIME: return "проведенное_время";
        case TemporalAxis::PAID_TIME: return "заплаченное_время";
        case TemporalAxis::GONE_TIME: return "ушедшее_время";
        case TemporalAxis::REMAINING_TIME: return "оставшееся_время";
        case TemporalAxis::FUTURE_TIME: return "будущее_время";
        case TemporalAxis::PRESENT_MOMENT: return "настоящий_момент";
        case TemporalAxis::DECISION_TIME: return "время_решения";
        case TemporalAxis::RESPONSE_TIME: return "время_ответа";
        case TemporalAxis::THOUGHT_TIME: return "время_мысли";
        case TemporalAxis::WAVE_PERIOD: return "период_волны";
        case TemporalAxis::WAVE_FREQUENCY: return "частота_волны";
        case TemporalAxis::EVENT_HORIZON_TIME: return "время_горизонта";
        case TemporalAxis::TEMPORAL_CONTINUUM: return "временной_континуум";
        default: return "unknown";
    }
}

} // namespace cyborg