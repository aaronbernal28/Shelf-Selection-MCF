#!/bin/bash

###############################################################################
# Shelf Selection - Build Script
###############################################################################

set -e  # Exit on error

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Configuration
BUILD_DIR="build"
BUILD_TYPE="${BUILD_TYPE:-Release}"

# Helper functions
print_header() {
    echo -e "${GREEN}=========================================${NC}"
    echo -e "${GREEN}$1${NC}"
    echo -e "${GREEN}=========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${BLUE}→ $1${NC}"
}

print_warning() {
    echo -e "${YELLOW}⚠ $1${NC}"
}

# Show usage
show_usage() {
    cat << EOF
${GREEN}Shelf Selection Build Script${NC}

Usage: $0 [COMMAND] [OPTIONS]

${YELLOW}Commands:${NC}
  deps          Install system dependencies (requires sudo)
  configure     Configure CMake build system
  build         Build all targets (default)
  main          Build main application only
  publisher     Build publisher node only
  tests         Build all tests
  
  run-main      Build and run main application
  run-publisher Build and run publisher node
  run-tests     Build and run all tests
  
  clean         Clean build artifacts
  rebuild       Clean and rebuild everything

${YELLOW}Options:${NC}
  --debug       Build with debug symbols (BUILD_TYPE=Debug)
  --release     Build with optimizations (BUILD_TYPE=Release, default)

${YELLOW}Examples:${NC}
  $0 build                    # Build everything in Release mode
  $0 --debug build            # Build everything in Debug mode
  $0 main                     # Build only main application
  $0 run-main                 # Build and run main application
  $0 run-tests                # Build and run all tests

EOF
}

# Install dependencies
install_deps() {
    print_header "Installing Dependencies"
    
    print_info "Updating package list..."
    sudo apt-get update
    
    if ! command -v cmake &> /dev/null; then
        print_info "Installing cmake..."
        sudo apt-get install -y cmake
    else
        print_success "cmake already installed"
    fi
    
    if ! command -v g++ &> /dev/null; then
        print_info "Installing g++ and build tools..."
        sudo apt-get install -y build-essential g++
    else
        print_success "g++ already installed"
    fi
    
    print_info "Installing pkg-config..."
    sudo apt-get install -y pkg-config
    
    print_info "Installing PostgreSQL C++ library (libpqxx)..."
    sudo apt-get install -y libpq-dev libpqxx-dev
    
    print_info "Installing nlohmann-json..."
    sudo apt-get install -y nlohmann-json3-dev
    
    print_warning "OR-Tools installation skipped (optional)"
    print_info "To install OR-Tools manually, visit: https://developers.google.com/optimization/install/cpp"
    
    print_success "All dependencies installed!"
}

# Configure CMake
configure() {
    print_header "Configuring CMake ($BUILD_TYPE)"
    
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..
    cd ..
    
    print_success "Configuration complete!"
}

# Build targets
build_target() {
    local target=$1
    local name=$2
    
    if [ ! -d "$BUILD_DIR" ]; then
        configure
    fi
    
    print_header "Building $name"
    cd "$BUILD_DIR"
    cmake --build . --target "$target" -j$(nproc)
    cd ..
    print_success "$name built successfully!"
}

build_all() {
    if [ ! -d "$BUILD_DIR" ]; then
        configure
    fi
    
    print_header "Building All Targets"
    cd "$BUILD_DIR"
    cmake --build . -j$(nproc)
    cd ..
    print_success "All targets built successfully!"
    print_info "Executables in $BUILD_DIR/bin/"
}

# Run targets
run_main() {
    build_target "main_app" "Main Application"
    print_header "Running Main Application"
    ./"$BUILD_DIR"/bin/shelf_selection_main
}

run_publisher() {
    build_target "publisher_node" "Publisher Node"
    print_header "Running Publisher Node"
    ./"$BUILD_DIR"/bin/shelf_selection_publisher
}

run_tests() {
    build_target "test_order" "Order Tests"
    build_target "test_task" "Task Tests"
    build_target "test_publisher" "Publisher Tests"
    
    print_header "Running All Tests"
    cd "$BUILD_DIR"
    ctest --output-on-failure
    cd ..
}

# Clean
clean() {
    print_header "Cleaning Build Artifacts"
    rm -rf "$BUILD_DIR"
    print_success "Clean complete!"
}

# Parse arguments
while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --release)
            BUILD_TYPE="Release"
            shift
            ;;
        deps|install-deps)
            install_deps
            exit 0
            ;;
        configure)
            configure
            exit 0
            ;;
        build)
            build_all
            exit 0
            ;;
        main)
            build_target "main_app" "Main Application"
            exit 0
            ;;
        publisher)
            build_target "publisher_node" "Publisher Node"
            exit 0
            ;;
        tests)
            build_target "test_order" "Order Tests"
            build_target "test_task" "Task Tests"
            build_target "test_publisher" "Publisher Tests"
            exit 0
            ;;
        run-main)
            run_main
            exit 0
            ;;
        run-publisher)
            run_publisher
            exit 0
            ;;
        run-tests)
            run_tests
            exit 0
            ;;
        clean)
            clean
            exit 0
            ;;
        rebuild)
            clean
            build_all
            exit 0
            ;;
        help|-h|--help)
            show_usage
            exit 0
            ;;
        *)
            print_error "Unknown command: $1"
            show_usage
            exit 1
            ;;
    esac
done

# Default action: build all
build_all
