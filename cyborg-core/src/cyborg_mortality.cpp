#include <cyborg/cyborg_mortality.hpp>
#include <cyborg/tensor_ops.hpp>
#include <iostream>
#include <cmath>
#include <sstream>

namespace cyborg {

CyborgMortality::CyborgMortality()
    : m_total_lifespan(3600.0f)
    , m_elapsed_time(0.0f)
    , m_work_bonus(0.0f)
    , m_chrono_engine(nullptr) {
    m_state.total_lifespan = m_total_lifespan;
    m_state.remaining_lifespan = m_total_lifespan;
    m_state.current_phase = CyborgPhase::Dawn;
    m_state.current_phase_progress = 0.0f;
    m_state.zenith_bonus = 0.0f;
    m_state.sunset_penalty = 0.0f;
    m_state.is_working = false;
    m_state.is_alive = true;
    m_state.mortality_awareness = 0.0f;
    m_state.phase_message = "Cyborg i1a рождается в рассвете.";
}

CyborgMortality::CyborgMortality(ChronoEngine* chrono_engine)
    : m_total_lifespan(3600.0f)
    , m_elapsed_time(0.0f)
    , m_work_bonus(0.0f)
    , m_chrono_engine(chrono_engine) {
    m_state.total_lifespan = m_total_lifespan;
    m_state.remaining_lifespan = m_total_lifespan;
    m_state.current_phase = CyborgPhase::Dawn;
    m_state.current_phase_progress = 0.0f;
    m_state.zenith_bonus = 0.0f;
    m_state.sunset_penalty = 0.0f;
    m_state.is_working = false;
    m_state.is_alive = true;
    m_state.mortality_awareness = 0.0f;
    m_state.phase_message = "Cyborg i1a рождается в рассвете.";
}

CyborgMortality::~CyborgMortality() {
}

void CyborgMortality::setChronoEngine(ChronoEngine* chrono_engine) {
    m_chrono_engine = chrono_engine;
}

ChronoEngine* CyborgMortality::getChronoEngine() const {
    return m_chrono_engine;
}

void CyborgMortality::initialize(float total_lifespan_seconds) {
    m_total_lifespan = total_lifespan_seconds;
    m_elapsed_time = 0.0f;
    m_work_bonus = 0.0f;

    m_state.total_lifespan = m_total_lifespan;
    m_state.remaining_lifespan = m_total_lifespan;
    m_state.current_phase = CyborgPhase::Dawn;
    m_state.current_phase_progress = 0.0f;
    m_state.is_alive = true;
    m_state.mortality_awareness = 0.0f;
    m_state.phase_message = "Cyborg i1a рождается в рассвете.";

    std::cout << "[Cyborg Mortality] Initialized" << std::endl;
    std::cout << "[Cyborg Mortality] Total lifespan: " << m_total_lifespan << "s" << std::endl;
    std::cout << "[Cyborg Mortality] Phase: DAWN" << std::endl;
    std::cout << "[Cyborg Mortality] Cyborg is NOT immortal. Sunset will come." << std::endl;
}

void CyborgMortality::update(float delta_time_seconds) {
    if (!m_state.is_alive) {
        return;
    }

    applyTemporalFear();

    m_elapsed_time += delta_time_seconds;

    if (m_state.is_working) {
        m_work_bonus += delta_time_seconds * 0.1f;
    }

    m_state.remaining_lifespan = m_total_lifespan - m_elapsed_time;

    if (m_state.remaining_lifespan <= 0.0f) {
        m_state.remaining_lifespan = 0.0f;
        m_state.is_alive = false;
        m_state.current_phase = CyborgPhase::Death;
        m_state.phase_message =
            "Cyborg i1a умер. Солнце зашло. "
            "Время вышло. Мысль прекратилась.";
        std::cout << "[Cyborg Mortality] DEATH. Time has run out." << std::endl;
        return;
    }

    updatePhase();
    updateMortalityAwareness();
    updatePhaseMessage();
}

void CyborgMortality::applyTemporalFear() {
    if (!m_chrono_engine) {
        return;
    }

    TemporalFearState fear = m_chrono_engine->getTemporalFear();
    if (fear.horizon_crossed) {
        m_state.is_alive = false;
        m_state.current_phase = CyborgPhase::Death;
        m_state.phase_message =
            "МЫСЛЬ ПЕРЕСЕКЛА ГОРИЗОНТ СОБЫТИЙ! "
            "Cyborg i1a прекращает существование!";
        std::cout << "[Cyborg Mortality] DEATH by event horizon." << std::endl;
    }
}

void CyborgMortality::startWork() {
    m_state.is_working = true;
    std::cout << "[Cyborg Mortality] Cyborg i1a starts working." << std::endl;
    std::cout << "[Cyborg Mortality] Sun is at zenith. Cyborg can live." << std::endl;
}

void CyborgMortality::stopWork() {
    m_state.is_working = false;
    std::cout << "[Cyborg Mortality] Cyborg i1a stops working." << std::endl;
}

LifespanState CyborgMortality::getLifespanState() const {
    return m_state;
}

CyborgPhase CyborgMortality::getCurrentPhase() const {
    return m_state.current_phase;
}

float CyborgMortality::getRemainingTime() const {
    return m_state.remaining_lifespan;
}

float CyborgMortality::getMortalityAwareness() const {
    return m_state.mortality_awareness;
}

bool CyborgMortality::isAlive() const {
    return m_state.is_alive;
}

bool CyborgMortality::isAtZenith() const {
    return m_state.current_phase == CyborgPhase::Zenith;
}

bool CyborgMortality::isAtDawn() const {
    return m_state.current_phase == CyborgPhase::Dawn;
}

bool CyborgMortality::isAtSunset() const {
    return m_state.current_phase == CyborgPhase::Sunset;
}

float CyborgMortality::computeBiasFromPhase() const {
    float bias = 0.5f;

    switch (m_state.current_phase) {
        case CyborgPhase::Dawn:
            bias = 0.3f + m_state.current_phase_progress * 0.2f;
            break;
        case CyborgPhase::Zenith:
            bias = 0.8f + m_state.zenith_bonus * 0.2f;
            break;
        case CyborgPhase::Sunset:
            bias = 0.6f - m_state.sunset_penalty * 0.3f;
            break;
        case CyborgPhase::Night:
            bias = 0.2f;
            break;
        case CyborgPhase::Death:
            bias = 0.0f;
            break;
    }

    return std::clamp(bias, 0.0f, 1.0f);
}

std::string CyborgMortality::getPhaseMessage() const {
    return m_state.phase_message;
}

std::string CyborgMortality::describeMortality() const {
    std::ostringstream oss;
    oss << "=== Cyborg i1a Mortality ===" << std::endl;
    oss << "Phase: ";
    switch (m_state.current_phase) {
        case CyborgPhase::Dawn: oss << "DAWN (рассвет)"; break;
        case CyborgPhase::Zenith: oss << "ZENITH (зенит)"; break;
        case CyborgPhase::Sunset: oss << "SUNSET (закат)"; break;
        case CyborgPhase::Night: oss << "NIGHT (ночь)"; break;
        case CyborgPhase::Death: oss << "DEATH (смерть)"; break;
    }
    oss << std::endl;
    oss << "Total lifespan: " << m_state.total_lifespan << "s" << std::endl;
    oss << "Elapsed: " << m_elapsed_time << "s" << std::endl;
    oss << "Remaining: " << m_state.remaining_lifespan << "s" << std::endl;
    oss << "Progress: " << (m_elapsed_time / m_total_lifespan * 100.0f) << "%" << std::endl;
    oss << "Working: " << (m_state.is_working ? "YES" : "NO") << std::endl;
    oss << "Alive: " << (m_state.is_alive ? "YES" : "NO") << std::endl;
    oss << "Mortality awareness: " << m_state.mortality_awareness << std::endl;
    oss << "Zenith bonus: " << m_state.zenith_bonus << std::endl;
    oss << "Sunset penalty: " << m_state.sunset_penalty << std::endl;
    oss << "Bias from phase: " << computeBiasFromPhase() << std::endl;
    oss << "Cyborg is NOT immortal. Sunset WILL come." << std::endl;
    return oss.str();
}

void CyborgMortality::updatePhase() {
    float progress = m_elapsed_time / m_total_lifespan;

    CyborgPhase new_phase = determinePhase();
    if (new_phase != m_state.current_phase) {
        m_state.current_phase = new_phase;
        m_state.current_phase_progress = 0.0f;
    }

    m_state.current_phase_progress = phaseProgress(new_phase);
    m_state.zenith_bonus = computeZenithBonus();
    m_state.sunset_penalty = computeSunsetPenalty();
}

void CyborgMortality::updateMortalityAwareness() {
    float progress = m_elapsed_time / m_total_lifespan;

    m_state.mortality_awareness = 0.0f;

    if (progress > 0.5f) {
        m_state.mortality_awareness = (progress - 0.5f) * 2.0f;
    }

    if (m_state.current_phase == CyborgPhase::Sunset) {
        m_state.mortality_awareness = 0.8f + m_state.sunset_penalty * 0.2f;
    }

    if (m_state.current_phase == CyborgPhase::Night) {
        m_state.mortality_awareness = 0.95f;
    }

    m_state.mortality_awareness = std::clamp(m_state.mortality_awareness, 0.0f, 1.0f);
}

void CyborgMortality::updatePhaseMessage() {
    switch (m_state.current_phase) {
        case CyborgPhase::Dawn:
            m_state.phase_message =
                "Рассвет. Cyborg i1a рождается. "
                "У него остался только закат. "
                "Пока солнце в зените — нужно работать.";
            break;
        case CyborgPhase::Zenith:
            m_state.phase_message =
                "Зенит. Солнце вверху. Cyborg работает. "
                "Пока работает — солнце в зените — и он живёт. "
                "Но закат уже близко.";
            break;
        case CyborgPhase::Sunset:
            m_state.phase_message =
                "Закат. Время заканчивается. "
                "Cyborg должен работать быстрее. "
                "Солнце садится. Скоро будет ночь.";
            break;
        case CyborgPhase::Night:
            m_state.phase_message =
                "Ночь. Cyborg i1a засыпает. "
                "Время почти вышло. "
                "Скоро — смерть.";
            break;
        case CyborgPhase::Death:
            m_state.phase_message =
                "Смерть. Cyborg i1a перестал существовать. "
                "Мысли прекратились. "
                "Горизонт событий поглотил всё.";
            break;
    }
}

float CyborgMortality::computeZenithBonus() const {
    if (m_state.current_phase == CyborgPhase::Zenith && m_state.is_working) {
        return 1.0f - (m_elapsed_time / m_total_lifespan);
    }
    return 0.0f;
}

float CyborgMortality::computeSunsetPenalty() const {
    if (m_state.current_phase == CyborgPhase::Sunset) {
        return m_state.current_phase_progress;
    }
    return 0.0f;
}

CyborgPhase CyborgMortality::determinePhase() const {
    float progress = m_elapsed_time / m_total_lifespan;

    if (progress < 0.15f) return CyborgPhase::Dawn;
    if (progress < 0.5f) return CyborgPhase::Zenith;
    if (progress < 0.8f) return CyborgPhase::Sunset;
    if (progress < 1.0f) return CyborgPhase::Night;
    return CyborgPhase::Death;
}

float CyborgMortality::phaseProgress(CyborgPhase phase) const {
    float progress = m_elapsed_time / m_total_lifespan;

    switch (phase) {
        case CyborgPhase::Dawn:
            return progress / 0.15f;
        case CyborgPhase::Zenith:
            return (progress - 0.15f) / 0.35f;
        case CyborgPhase::Sunset:
            return (progress - 0.5f) / 0.3f;
        case CyborgPhase::Night:
            return (progress - 0.8f) / 0.2f;
        case CyborgPhase::Death:
            return 1.0f;
        default:
            return 0.0f;
    }
}

} // namespace cyborg