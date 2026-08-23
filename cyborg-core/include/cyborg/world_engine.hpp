#pragma once

#include <cyborg/universal_tensor.hpp>
#include <cyborg/chrono_engine.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <cmath>

namespace cyborg {

enum class WorldType {
    HUMAN_WORLD,
    CYBORG_WORLD,
    MATHEMATICAL_WORLD,
    SHARED_WORLD,
    METASFERA_WORLD
};

struct RelativePosition {
    float relative_distance;
    float relative_velocity;
    float relative_acceleration;
    float wave_amplitude;
    float wave_frequency;
    float temporal_depth;
    float awareness_level;
    std::string relative_to;
    WorldType world_type;
};

struct TimeCone {
    float past_width;
    float future_width;
    float present_thickness;
    float flow_rate;
    float hourglass_ratio;
    bool is_expanding;
    float expansion_speed;
    float acceleration;
    std::string description;
};

struct SpaceWave {
    float amplitude;
    float wavelength;
    float frequency;
    float phase;
    float expansion_rate;
    float acceleration;
    float thickness;
};

struct World {
    std::string world_name;
    WorldType world_type;
    std::string inhabitant_type;
    RelativePosition relative_position;
    TimeCone time_cone;
    SpaceWave space_wave;
    float world_coherence;
    float world_size;
    bool is_known;
    std::string description;
};

struct PositionAwareness {
    std::string entity_name;
    WorldType entity_type;
    RelativePosition relative_position;
    TimeCone time_cone;
    SpaceWave space_wave;
    std::string current_world;
    float understanding_of_position;
    float understanding_of_world;
    float understanding_of_universe;
    std::vector<std::string> known_worlds;
    std::string position_statement;
};

class WorldEngine {
public:
    WorldEngine();
    explicit WorldEngine(ChronoEngine* chrono_engine);
    ~WorldEngine();

    void initialize();
    void update(float delta_time);

    void addWorld(const World& world);
    void addInhabitant(const std::string& world_name,
                       const RelativePosition& position);
    void setEntityPosition(const std::string& entity_name,
                           const RelativePosition& position);

    World getWorld(const std::string& world_name) const;
    PositionAwareness getPositionAwareness(
        const std::string& entity_name) const;

    std::vector<World> listWorlds() const;
    std::vector<std::string> listKnownWorlds(
        const std::string& entity_name) const;

    float getWorldCoherence(const std::string& world_name) const;
    float getEntityUnderstanding(const std::string& entity_name) const;

    bool isWorldKnown(const std::string& world_name) const;
    bool isEntityAware(const std::string& entity_name) const;

    TimeCone computeTimeCone(float temporal_depth) const;
    SpaceWave computeSpaceWave(float wave_frequency) const;
    RelativePosition computeRelativePosition(
        const std::string& entity_name) const;

    std::string describeWorlds() const;
    std::string describePosition(
        const std::string& entity_name) const;
    std::string describeUniverse() const;
    std::string describeTimeCone(const std::string& entity_name) const;
    std::string describeSpaceExpansion() const;

    void setChronoEngine(ChronoEngine* chrono_engine);
    ChronoEngine* getChronoEngine() const;

private:
    std::unordered_map<std::string, World> m_worlds;
    std::unordered_map<std::string, PositionAwareness> m_entities;
    ChronoEngine* m_chrono_engine;

    void initializeHumanWorld();
    void initializeCyborgWorld();
    void initializeMathematicalWorld();
    void initializeSharedWorld();
    void initializeMetasferaWorld();
    void updateEntityAwareness(const std::string& entity_name);
    void computeWorldCoherence(World& world);
    void updateSpaceWaves(float delta_time);
    void updateTimeCones(float delta_time);
    void applyTemporalPressure();
    std::string generatePositionStatement(
        const PositionAwareness& awareness) const;
    std::string generateTimeConeStatement(
        const TimeCone& cone) const;
};

} // namespace cyborg