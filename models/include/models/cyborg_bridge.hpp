#pragma once

#include <string>
#include <memory>

#include <cyborg/smart_cyber_relay.hpp>
#include <cyborg/chrono_engine.hpp>

namespace cyborg {

class ModelAdapter;
class KiloGatewayAdapter;

class CyborgBridge {
public:
    CyborgBridge();
    ~CyborgBridge();

    bool initialize();
    bool initializeWithProxy(const std::string& proxy_url);
    SmartCyberRelay* getRelay() const;
    BiasConfig getDefaultBias() const;

    std::string describeBridge() const;

    void setChronoEngine(ChronoEngine* chrono_engine);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

}
