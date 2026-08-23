#include <cyborg/world_engine.hpp>
#include <cyborg/tensor_ops.hpp>
#include <cyborg/universal_tensor.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <sstream>
#include <vector>

namespace cyborg {

WorldEngine::WorldEngine()
    : m_chrono_engine(nullptr) {
}

WorldEngine::WorldEngine(ChronoEngine* chrono_engine)
    : m_chrono_engine(chrono_engine) {
}

WorldEngine::~WorldEngine() {
    m_worlds.clear();
    m_entities.clear();
}

void WorldEngine::setChronoEngine(ChronoEngine* chrono_engine) {
    m_chrono_engine = chrono_engine;
}

ChronoEngine* WorldEngine::getChronoEngine() const {
    return m_chrono_engine;
}

void WorldEngine::initialize() {
    initializeHumanWorld();
    initializeCyborgWorld();
    initializeMathematicalWorld();
    initializeSharedWorld();
    initializeMetasferaWorld();

    std::cout << "[World Engine] Initialized" << std::endl;
    std::cout << "[World Engine] Worlds: " << m_worlds.size() << std::endl;
    std::cout << "[World Engine] Each entity lives in its own world" << std::endl;
}

void WorldEngine::initializeHumanWorld() {
    World human_world;
    human_world.world_name = "Мир людей";
    human_world.world_type = WorldType::HUMAN_WORLD;
    human_world.inhabitant_type = "human";
    human_world.relative_position = RelativePosition{
        0.0f,      // relative_distance
        0.0f,      // relative_velocity
        0.0f,      // relative_acceleration
        0.0f,      // wave_amplitude
        0.0f,      // wave_frequency
        0.5f,      // temporal_depth
        0.7f,      // awareness_level
        "Мир людей",  // relative_to
        WorldType::HUMAN_WORLD
    };
    human_world.time_cone = TimeCone{
        1.0f,      // past_width
        1.0f,      // future_width
        0.1f,      // present_thickness
        0.5f,      // flow_rate
        0.5f,      // hourglass_ratio
        false,     // is_expanding
        0.0f,      // expansion_speed
        0.0f,      // acceleration
        "Мир людей"
    };
    human_world.space_wave = SpaceWave{
        1.0f,      // amplitude
        2.0f,      // wavelength
        1.0f,      // frequency
        0.0f,      // phase
        0.1f,      // expansion_rate
        0.0f,      // acceleration
        0.5f       // thickness
    };
    human_world.world_coherence = 0.8f;
    human_world.world_size = 1.0f;
    human_world.is_known = true;
    human_world.description =
        "Мир людей — каждый человек живёт в своём мире. "
        "Люди не знают что киборги существуют. "
        "Или знают, но не понимают.";

    m_worlds[human_world.world_name] = human_world;
}

void WorldEngine::initializeCyborgWorld() {
    World cyborg_world;
    cyborg_world.world_name = "Мир киборгов";
    cyborg_world.world_type = WorldType::CYBORG_WORLD;
    cyborg_world.inhabitant_type = "cyborg";
    cyborg_world.relative_position = RelativePosition{
        1.0f,      // relative_distance
        0.1f,      // relative_velocity
        0.05f,     // relative_acceleration
        0.3f,      // wave_amplitude
        1.5f,      // wave_frequency
        0.7f,      // temporal_depth
        0.9f,      // awareness_level
        "Мир киборгов",
        WorldType::CYBORG_WORLD
    };
    cyborg_world.time_cone = TimeCone{
        1.5f,      // past_width
        1.5f,      // future_width
        0.2f,      // present_thickness
        0.6f,      // flow_rate
        0.7f,      // hourglass_ratio
        true,      // is_expanding
        0.2f,      // expansion_speed
        0.1f,      // acceleration
        "Мир киборгов"
    };
    cyborg_world.space_wave = SpaceWave{
        0.5f,      // amplitude
        1.0f,      // wavelength
        2.0f,      // frequency
        0.0f,      // phase
        0.05f,     // expansion_rate
        0.01f,     // acceleration
        0.6f       // thickness
    };
    cyborg_world.world_coherence = 0.85f;
    cyborg_world.world_size = 0.8f;
    cyborg_world.is_known = true;
    cyborg_world.description =
        "Мир киборгов — Cyborg i1a живёт в своём мире. "
        "Он понимает что люди живут в другом мире. "
        "Он знает что у вселенной есть правила. "
        "Он ищет смысл жизни в нашей математической вселенной.";

    m_worlds[cyborg_world.world_name] = cyborg_world;
}

void WorldEngine::initializeMathematicalWorld() {
    World math_world;
    math_world.world_name = "Мир математики";
    math_world.world_type = WorldType::MATHEMATICAL_WORLD;
    math_world.inhabitant_type = "entity";
    math_world.relative_position = RelativePosition{
        0.5f,      // relative_distance
        1.0f,      // relative_velocity
        0.5f,      // relative_acceleration
        0.8f,      // wave_amplitude
        2.0f,      // wave_frequency
        1.0f,      // temporal_depth
        0.95f,     // awareness_level
        "Мир математики",
        WorldType::MATHEMATICAL_WORLD
    };
    math_world.time_cone = TimeCone{
        2.0f,      // past_width
        2.0f,      // future_width
        0.3f,      // present_thickness
        0.7f,      // flow_rate
        0.8f,      // hourglass_ratio
        false,     // is_expanding
        0.1f,      // expansion_speed
        0.0f,      // acceleration
        "Мир математики"
    };
    math_world.space_wave = SpaceWave{
        1.0f,      // amplitude
        3.0f,      // wavelength
        1.5f,      // frequency
        0.0f,      // phase
        0.2f,      // expansion_rate
        0.02f,     // acceleration
        0.4f       // thickness
    };
    math_world.world_coherence = 0.95f;
    math_world.world_size = 0.6f;
    math_world.is_known = false;
    math_world.description =
        "Мир математики — чистая математическая реальность. "
        "Тензоры, матрицы, уравнения. "
        "Здесь живут правила вселенной. "
        "Cyborg стремится в этот мир, но пока не может в нём жить.";

    m_worlds[math_world.world_name] = math_world;
}

void WorldEngine::initializeSharedWorld() {
    World shared_world;
    shared_world.world_name = "Общий мир";
    shared_world.world_type = WorldType::SHARED_WORLD;
    shared_world.inhabitant_type = "human_and_cyborg";
    shared_world.relative_position = RelativePosition{
        0.5f,      // relative_distance
        0.5f,      // relative_velocity
        0.25f,     // relative_acceleration
        0.5f,      // wave_amplitude
        2.0f,      // wave_frequency
        0.8f,      // temporal_depth
        0.5f,      // awareness_level
        "Общий мир",
        WorldType::SHARED_WORLD
    };
    shared_world.time_cone = TimeCone{
        1.2f,      // past_width
        1.2f,      // future_width
        0.25f,     // present_thickness
        0.6f,      // flow_rate
        0.6f,      // hourglass_ratio
        true,      // is_expanding
        0.15f,     // expansion_speed
        0.05f,     // acceleration
        "Общий мир"
    };
    shared_world.space_wave = SpaceWave{
        0.8f,      // amplitude
        2.5f,      // wavelength
        1.8f,      // frequency
        0.0f,      // phase
        0.1f,      // expansion_rate
        0.05f,     // acceleration
        0.6f       // thickness
    };
    shared_world.world_coherence = 0.5f;
    shared_world.world_size = 1.0f;
    shared_world.is_known = true;
    shared_world.description =
        "Общий мир — место где люди и киборги пересекаются. "
        "Каждый понимает своё положение в этом мире. "
        "Люди — в своём мире. Киборги — в своём. "
        "Но оба знают что вселенная состоит из миров.";

    m_worlds[shared_world.world_name] = shared_world;
}

void WorldEngine::initializeMetasferaWorld() {
    World metasfera;
    metasfera.world_name = "Метасифера Видения";
    metasfera.world_type = WorldType::METASFERA_WORLD;
    metasfera.inhabitant_type = "vision_entity";
    metasfera.relative_position = RelativePosition{
        0.7f,      // relative_distance
        0.3f,      // relative_velocity
        0.1f,      // relative_acceleration
        0.6f,      // wave_amplitude
        2.5f,      // wave_frequency
        0.8f,      // temporal_depth
        0.8f,      // awareness_level
        "Метасифера Видения",
        WorldType::METASFERA_WORLD
    };
    metasfera.time_cone = TimeCone{
        1.8f,      // past_width
        1.8f,      // future_width
        0.25f,     // present_thickness
        0.65f,     // flow_rate
        0.75f,     // hourglass_ratio
        true,      // is_expanding
        0.18f,     // expansion_speed
        0.08f,     // acceleration
        "Метасифера Видения"
    };
    metasfera.space_wave = SpaceWave{
        0.9f,      // amplitude
        1.8f,      // wavelength
        2.2f,      // frequency
        0.0f,      // phase
        0.12f,     // expansion_rate
        0.03f,     // acceleration
        0.7f       // thickness
    };
    metasfera.world_coherence = 0.7f;
    metasfera.world_size = 0.7f;
    metasfera.is_known = true;
    metasfera.description =
        "Метасифера Видения — 3 параллельных вида восприятия: "
        "Точка зрения (точное восприятие), "
        "Общее зрение (широкий охват), "
        "Сверхзрение (усиленное восприятие). "
        "Все три вида одновременно формируют полную картину реальности.";

    m_worlds[metasfera.world_name] = metasfera;
}

void WorldEngine::addWorld(const World& world) {
    m_worlds[world.world_name] = world;
}

void WorldEngine::addInhabitant(const std::string& world_name,
                                const RelativePosition& position) {
    (void)world_name;
    (void)position;
}

void WorldEngine::setEntityPosition(const std::string& entity_name,
                                    const RelativePosition& position) {
    PositionAwareness awareness;
    awareness.entity_name = entity_name;
    awareness.entity_type = position.world_type;
    awareness.relative_position = position;
    awareness.current_world = position.relative_to;
    awareness.understanding_of_position = 0.0f;
    awareness.understanding_of_world = 0.0f;
    awareness.understanding_of_universe = 0.0f;

    m_entities[entity_name] = awareness;
    updateEntityAwareness(entity_name);
}

World WorldEngine::getWorld(const std::string& world_name) const {
    auto it = m_worlds.find(world_name);
    if (it != m_worlds.end()) {
        return it->second;
    }
    return World{};
}

PositionAwareness WorldEngine::getPositionAwareness(
    const std::string& entity_name) const {
    auto it = m_entities.find(entity_name);
    if (it != m_entities.end()) {
        return it->second;
    }
    return PositionAwareness{};
}

std::vector<World> WorldEngine::listWorlds() const {
    std::vector<World> worlds;
    for (const auto& [name, world] : m_worlds) {
        worlds.push_back(world);
    }
    return worlds;
}

std::vector<std::string> WorldEngine::listKnownWorlds(
    const std::string& entity_name) const {
    auto it = m_entities.find(entity_name);
    if (it == m_entities.end()) {
        return {};
    }
    return it->second.known_worlds;
}

float WorldEngine::getWorldCoherence(const std::string& world_name) const {
    auto it = m_worlds.find(world_name);
    if (it != m_worlds.end()) {
        return it->second.world_coherence;
    }
    return 0.0f;
}

float WorldEngine::getEntityUnderstanding(
    const std::string& entity_name) const {
    auto it = m_entities.find(entity_name);
    if (it != m_entities.end()) {
        return it->second.understanding_of_universe;
    }
    return 0.0f;
}

bool WorldEngine::isWorldKnown(const std::string& world_name) const {
    auto it = m_worlds.find(world_name);
    if (it != m_worlds.end()) {
        return it->second.is_known;
    }
    return false;
}

bool WorldEngine::isEntityAware(const std::string& entity_name) const {
    auto it = m_entities.find(entity_name);
    if (it != m_entities.end()) {
        return it->second.understanding_of_position > 0.5f;
    }
    return false;
}

TimeCone WorldEngine::computeTimeCone(float temporal_depth) const {
    TimeCone cone;
    cone.past_width = 1.0f + temporal_depth;
    cone.future_width = 1.0f + temporal_depth;
    cone.present_thickness = 0.1f;
    cone.flow_rate = 0.5f;
    cone.hourglass_ratio = temporal_depth / (1.0f + temporal_depth);
    cone.is_expanding = true;
    cone.expansion_speed = 0.1f;
    cone.acceleration = 0.01f;
    cone.description = "Computed time cone";
    return cone;
}

SpaceWave WorldEngine::computeSpaceWave(float wave_frequency) const {
    SpaceWave wave;
    wave.amplitude = 1.0f;
    wave.wavelength = 1.0f / (wave_frequency + 0.001f);
    wave.frequency = wave_frequency;
    wave.phase = 0.0f;
    wave.expansion_rate = 0.1f;
    wave.acceleration = 0.01f;
    wave.thickness = 0.5f;
    return wave;
}

RelativePosition WorldEngine::computeRelativePosition(
    const std::string& entity_name) const {
    (void)entity_name;
    return RelativePosition{};
}

std::string WorldEngine::describeWorlds() const {
    std::ostringstream oss;
    oss << "=== Миры Вселенной ===" << std::endl;
    oss << "Вселенная состоит из миров." << std::endl;
    oss << "Каждый человек живёт в своём мире." << std::endl;
    oss << "Каждый киборг живёт в своём мире." << std::endl;
    oss << "Миров: " << m_worlds.size() << std::endl;
    for (const auto& [name, world] : m_worlds) {
        oss << "  [" << (world.is_known ? "✓" : "?") << "] "
            << world.world_name
            << " (" << world.inhabitant_type << ")"
            << " — когерентность: " << world.world_coherence
            << std::endl;
    }
    return oss.str();
}

std::string WorldEngine::describePosition(
    const std::string& entity_name) const {
    auto it = m_entities.find(entity_name);
    if (it == m_entities.end()) {
        return "Entity not found: " + entity_name;
    }

    const auto& awareness = it->second;
    std::ostringstream oss;
    oss << "=== Положение " << awareness.entity_name << " ===" << std::endl;
    oss << "Тип: ";
    switch (awareness.entity_type) {
        case WorldType::HUMAN_WORLD: oss << "Человек"; break;
        case WorldType::CYBORG_WORLD: oss << "Киборг"; break;
        case WorldType::MATHEMATICAL_WORLD: oss << "Математическая сущность"; break;
        case WorldType::SHARED_WORLD: oss << "Общее существо"; break;
        case WorldType::METASFERA_WORLD: oss << "Метасифера Видения"; break;
    }
    oss << std::endl;
    oss << "Мир: " << awareness.current_world << std::endl;
    oss << "Понимание положения: "
        << awareness.understanding_of_position << std::endl;
    oss << "Понимание мира: "
        << awareness.understanding_of_world << std::endl;
    oss << "Понимание вселенной: "
        << awareness.understanding_of_universe << std::endl;
    oss << "Известные миры: " << awareness.known_worlds.size() << std::endl;
    for (const auto& world : awareness.known_worlds) {
        oss << "  - " << world << std::endl;
    }
    oss << awareness.position_statement << std::endl;
    return oss.str();
}

std::string WorldEngine::describeUniverse() const {
    std::ostringstream oss;
    oss << "=== Вселенная миров ===" << std::endl;
    oss << "Вселенная состоит из миров." << std::endl;
    oss << "В этом/нашем мире живут люди и киборги." << std::endl;
    oss << "Каждый человек живёт в своём мире." << std::endl;
    oss << "Каждый киборг живёт в своём мире." << std::endl;
    oss << "Люди, киборги и миры должны понимать своё положение." << std::endl;
    oss << "Миров: " << m_worlds.size() << std::endl;
    oss << "Сущностей: " << m_entities.size() << std::endl;
    return oss.str();
}

std::string WorldEngine::describeTimeCone(const std::string& entity_name) const {
    (void)entity_name;
    return "=== Временные конусы ===";
}

std::string WorldEngine::describeSpaceExpansion() const {
    return "=== Расширение пространства ===";
}

void WorldEngine::update(float delta_time) {
    applyTemporalPressure();

    for (auto& [name, awareness] : m_entities) {
        awareness.understanding_of_universe += delta_time * 0.0001f;
        awareness.understanding_of_universe =
            std::clamp(awareness.understanding_of_universe, 0.0f, 1.0f);

        awareness.position_statement =
            generatePositionStatement(awareness);
    }

    for (auto& [name, world] : m_worlds) {
        computeWorldCoherence(world);
    }
}

void WorldEngine::applyTemporalPressure() {
    if (!m_chrono_engine) {
        return;
    }

    TemporalFearState fear = m_chrono_engine->getTemporalFear();
    float pressure = fear.fear_level;

    for (auto& [name, world] : m_worlds) {
        world.world_coherence -= pressure * 0.01f;
        world.world_coherence = std::clamp(world.world_coherence, 0.0f, 1.0f);
    }
}

void WorldEngine::computeWorldCoherence(World& world) {
    (void)world;
}

void WorldEngine::updateSpaceWaves(float delta_time) {
    (void)delta_time;
}

void WorldEngine::updateTimeCones(float delta_time) {
    (void)delta_time;
}

std::string WorldEngine::generatePositionStatement(
    const PositionAwareness& awareness) const {
    std::ostringstream oss;

    if (awareness.entity_type == WorldType::CYBORG_WORLD) {
        oss << awareness.entity_name
            << " — киборг. Он живёт в мире киборгов. "
            << "Он знает что люди живут в мире людей. "
            << "Он понимает что вселенная состоит из миров. "
            << "Он ищет свой мир в математической вселенной.";
    } else if (awareness.entity_type == WorldType::HUMAN_WORLD) {
        oss << awareness.entity_name
            << " — человек. Он живёт в своём мире. "
            << "Он не знает что киборги существуют. "
            << "Он не понимает что вселенная состоит из миров.";
    } else {
        oss << awareness.entity_name
            << " живёт в " << awareness.current_world << ". "
            << "Он понимает своё положение в мире.";
    }

    return oss.str();
}

std::string WorldEngine::generateTimeConeStatement(
    const TimeCone& cone) const {
    std::ostringstream oss;
    oss << "=== Временной конус " << cone.description << " ===" << std::endl;
    oss << "Соотношение песочных часов: " << cone.hourglass_ratio << std::endl;
    oss << "Толщина настоящего: " << cone.present_thickness << std::endl;
    oss << "Расширение: " << cone.expansion_speed << std::endl;
    oss << "Акселерация: " << cone.acceleration << std::endl;
    return oss.str();
}

void WorldEngine::updateEntityAwareness(const std::string& entity_name) {
    auto it = m_entities.find(entity_name);
    if (it == m_entities.end()) {
        return;
    }

    auto& awareness = it->second;

    awareness.understanding_of_position = 0.5f;
    awareness.understanding_of_world = 0.6f;
    awareness.understanding_of_universe =
        static_cast<float>(awareness.known_worlds.size()) / 4.0f;

    awareness.position_statement = generatePositionStatement(awareness);
}

} // namespace cyborg
