cmake_minimum_required(VERSION 3.14)
project(cyborg-core VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cyborg-core SHARED)

set_target_properties(cyborg-core PROPERTIES EXPORT_NAME cyborg-core)

target_sources(cyborg-core
    PRIVATE
        src/tensor.cpp
        src/model_loader.cpp
        src/merge_engine.cpp
        src/ensemble_router.cpp
        src/lifecycle.cpp
        src/knowledge_transfer.cpp
        src/smart_cyber_relay.cpp
        src/cyborg_inference.cpp
        src/chrono_engine.cpp
        src/cyborg_mortality.cpp
        src/universe_awareness.cpp
        src/universal_tensor.cpp
        src/world_engine.cpp
)

target_include_directories(cyborg-core
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(cyborg-core PUBLIC cxx_std_17)

install(TARGETS cyborg-core
    EXPORT cyborg-core-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
)

install(DIRECTORY include/
    DESTINATION include
)

install(EXPORT cyborg-core-targets
    FILE cyborg-core-targets.cmake
    NAMESPACE cyborg::
    DESTINATION lib/cmake/cyborg-core
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/cyborg-core-config-version.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY AnyNewerVersion
)

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cyborg-core-config-version.cmake
    DESTINATION lib/cmake/cyborg-core
)