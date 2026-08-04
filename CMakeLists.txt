cmake_minimum_required(VERSION 3.20)
project(cyborg-main LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX /usr/local)

find_package(cyborg-core REQUIRED)
find_package(models REQUIRED)

add_executable(cyborg-main main.cpp)

target_include_directories(cyborg-main PRIVATE
    ${CMAKE_SOURCE_DIR}/cyborg-core/include
    ${CMAKE_SOURCE_DIR}/models/include
)

target_link_libraries(cyborg-main PRIVATE
    cyborg-core
    cyborg_bridge
    model_registry
    model_manager
    qwen_adapter
    llama_adapter
    deepseek-adapter
    nemotron_adapter
    curl
)

install(TARGETS cyborg-main DESTINATION bin)