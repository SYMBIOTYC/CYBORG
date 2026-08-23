add_library(cyborg_core SHARED)

target_sources(cyborg_core
    PRIVATE
        tensor.cpp
        model_loader.cpp
        merge_engine.cpp
        ensemble_router.cpp
        lifecycle.cpp
        knowledge_transfer.cpp
        smart_cyber_relay.cpp
        cyborg_inference.cpp
        chrono_engine.cpp
        cyborg_mortality.cpp
        universe_awareness.cpp
        universal_tensor.cpp
        world_engine.cpp
)

target_include_directories(cyborg_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(cyborg_core PUBLIC cxx_std_17)