cmake_minimum_required(VERSION 3.10)
project(WES VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find required packages
find_package(PkgConfig REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/../src)
include_directories(${PQXX_INCLUDE_DIRS})

# WES source files
set(WES_SOURCES
    src/stock.cpp
    src/task_manager.cpp
    src/shelf_selection.cpp
    src/order_manager.cpp
    ../src/utils.cpp
    ../src/db_connector.cpp
)

# Create WES library
add_library(wes_lib STATIC ${WES_SOURCES})
target_link_libraries(wes_lib
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
)

# Create WES executable
add_executable(wes src/wes.cpp)
target_link_libraries(wes
    wes_lib
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
)