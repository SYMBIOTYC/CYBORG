#include <models/cyborg_bridge.hpp>
#include <cyborg/chrono_engine.hpp>
#include <cyborg/cyborg_mortality.hpp>
#include <cyborg/world_engine.hpp>
#include <cyborg/universe_awareness.hpp>
#include <iostream>
#include <chrono>

int main() {
    std::cout << "=== CYBORG i1a SYSTEM START ===" << std::endl;

    cyborg::ChronoEngine chrono;
    chrono.initialize();

    cyborg::CyborgMortality mortality(&chrono);
    mortality.initialize(3600.0f);
    mortality.startWork();

    cyborg::WorldEngine engine(&chrono);
    engine.initialize();

    cyborg::UniverseAwareness awareness(&chrono);
    awareness.initialize();

    cyborg::CyborgBridge bridge;
    bridge.setChronoEngine(&chrono);
    if (!bridge.initializeWithProxy("http://127.0.0.1:9999")) {
        std::cerr << "Failed to initialize bridge" << std::endl;
        return 1;
    }

    std::cout << bridge.describeBridge() << std::endl;
    std::cout << chrono.describeChronoMap() << std::endl;
    std::cout << mortality.describeMortality() << std::endl;
    std::cout << engine.describeWorlds() << std::endl;
    std::cout << awareness.describeUniverse() << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; ++i) {
        chrono.updateTime();
        mortality.update(0.016f);
        engine.update(0.016f);
        awareness.update(0.016f);
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> ms = end - start;
    std::cout << "1000 steps latency: " << ms.count() << " ms" << std::endl;
    std::cout << "Per step: " << (ms.count() / 1000.0) << " ms" << std::endl;

    std::cout << awareness.describePurpose() << std::endl;

    return 0;
}
