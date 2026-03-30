cmake_minimum_required(VERSION 3.20)
project(collab-core LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CMakePackageConfigHelpers)

find_package(fmt CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(platform_folders CONFIG REQUIRED)

add_library(collab-core STATIC src/core.cpp)
target_include_directories(collab-core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(collab-core PUBLIC fmt::fmt spdlog::spdlog sago::platform_folders)

install(TARGETS collab-core EXPORT collab-core-targets ARCHIVE DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)
install(EXPORT collab-core-targets
    FILE collab-core-targets.cmake
    NAMESPACE collab::
    DESTINATION share/collab-core
)

configure_package_config_file(
    cmake/collab-core-config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/collab-core-config.cmake"
    INSTALL_DESTINATION share/collab-core
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/collab-core-config.cmake"
    DESTINATION share/collab-core
)
