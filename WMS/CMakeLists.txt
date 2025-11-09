cmake_minimum_required(VERSION 3.10)
project(WMS VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Include directories from parent project
include_directories(${PROJECT_SOURCE_DIR}/../src)
include_directories(${PQXX_INCLUDE_DIRS})

# WMS source files
set(WMS_SOURCES
    src/publisher.cpp
    ${PROJECT_SOURCE_DIR}/../src/db_connector.cpp
    ${PROJECT_SOURCE_DIR}/../src/utils.cpp
)

# Create WMS library
add_library(wms_lib STATIC ${WMS_SOURCES})
target_link_libraries(wms_lib
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
)

# Create WMS executable
add_executable(wms src/wms.cpp)
target_link_libraries(wms
    wms_lib
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
)