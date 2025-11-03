cmake_minimum_required(VERSION 3.10)
project(ShelfSelection VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# Find required packages
find_package(PkgConfig REQUIRED)

# Find nlohmann_json
find_package(nlohmann_json 3.2.0 REQUIRED)

# Find PostgreSQL C++ library (libpqxx)
pkg_check_modules(PQXX REQUIRED libpqxx)

# Find OR-Tools (optional, uncomment if using)
# find_package(ortools CONFIG REQUIRED)

# Include directories
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PQXX_INCLUDE_DIRS})

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)

# Source files for the library (shared code)
set(LIB_SOURCES
    src/order.cpp
    src/stock.cpp
    src/task_manager.cpp
    # src/shelf_selection.cpp  # Commented out - requires OR-Tools
)

# Create a shared library with common functionality
add_library(shelf_selection_lib STATIC ${LIB_SOURCES})
target_include_directories(shelf_selection_lib PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(shelf_selection_lib 
    nlohmann_json::nlohmann_json
    ${PQXX_LIBRARIES}
    # ortools::ortools  # Uncomment if using OR-Tools
)
target_link_directories(shelf_selection_lib PUBLIC ${PQXX_LIBRARY_DIRS})

# ============================================
# EXECUTABLE: Main Application
# ============================================
add_executable(main_app src/main.cpp)
target_link_libraries(main_app shelf_selection_lib)
set_target_properties(main_app PROPERTIES OUTPUT_NAME "shelf_selection_main")

# ============================================
# EXECUTABLE: Publisher Node
# ============================================
add_executable(publisher_node src/publisher.cpp)
target_link_libraries(publisher_node shelf_selection_lib)
set_target_properties(publisher_node PROPERTIES OUTPUT_NAME "shelf_selection_publisher")

# ============================================
# TESTS
# ============================================
# enable_testing()

# # Test: Order
# add_executable(test_order tests/test_order.cpp)
# target_link_libraries(test_order shelf_selection_lib)
# add_test(NAME OrderTest COMMAND test_order)

# # Test: Task
# add_executable(test_task tests/test_task.cpp)
# target_link_libraries(test_task shelf_selection_lib)
# add_test(NAME TaskTest COMMAND test_task)

# # Test: Publisher
# add_executable(test_publisher tests/test_publisher.cpp)
# target_link_libraries(test_publisher shelf_selection_lib)
# add_test(NAME PublisherTest COMMAND test_publisher)

# ============================================
# Custom targets for convenience
# ============================================

# Run all tests
# add_custom_target(run_tests
#     COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
#     DEPENDS test_order test_task test_publisher
#     COMMENT "Running all tests..."
# )

# Clean build artifacts
add_custom_target(clean_all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJECT_SOURCE_DIR}/build
    COMMENT "Cleaning build directory..."
)

# Print build info
message(STATUS "==============================================")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "==============================================")
