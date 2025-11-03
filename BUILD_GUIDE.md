# Quick Build Reference

## First Time Setup

```bash
# 1. Install system dependencies
sudo apt-get update
sudo apt-get install -y build-essential cmake g++ make pkg-config

# 2. Install PostgreSQL development libraries
sudo apt-get install -y libpq-dev libpqxx-dev

# 3. Install nlohmann-json library
sudo apt-get install -y nlohmann-json3-dev

# 4. (Optional) Install OR-Tools for optimization
# Option A: Using package manager (if available)
# sudo apt-get install -y or-tools
# Option B: Build from source - see https://developers.google.com/optimization/install/cpp

# 5. Make build script executable
chmod +x build.sh
```

## Dependencies

This project requires the following libraries:

### Required
- **nlohmann-json** (v3.2.0+): JSON parsing library
  - Header-only library for modern C++
  - Used for reading backlog and stock data files
  
- **libpqxx**: PostgreSQL C++ client library
  - Used for database connections and operations
  - Requires libpq-dev (PostgreSQL development files)

### Optional
- **OR-Tools**: Google's Operations Research tools
  - Used for optimization algorithms (MCF, routing, etc.)
  - Only needed if using optimization features
  - Can be installed separately or commented out in CMakeLists.txt

### Quick Install (Ubuntu/Debian)
```bash
# Install all required dependencies
./build.sh deps

# Or manually:
sudo apt-get install -y libpq-dev libpqxx-dev nlohmann-json3-dev
```

## Common Commands

### Build

```bash
# Build everything (Release mode)
./build.sh build
make build

# Build with debug symbols
./build.sh --debug build
make BUILD_TYPE=Debug build

# Build specific targets
./build.sh main
./build.sh publisher
./build.sh tests
```

### Run

```bash
# Main application
./build.sh run-main
make run_main
./build/bin/shelf_selection_main

# Publisher node
./build.sh run-publisher
make run_publisher
./build/bin/shelf_selection_publisher

# Tests
./build.sh run-tests
make run_tests
cd build && ctest --output-on-failure
```

### Clean

```bash
# Clean build artifacts
./build.sh clean
make clean

# Rebuild from scratch
./build.sh rebuild
make rebuild
```

## Project Structure

```
build/bin/                           # All executables
├── shelf_selection_main             # Main application
├── shelf_selection_publisher        # Publisher node
├── test_order                       # Order tests
├── test_task                        # Task tests
└── test_publisher                   # Publisher tests
```

## Development Workflow

```bash
# 1. Make changes to source files
vim src/main.cpp

# 2. Build specific target
./build.sh main

# 3. Run to test
./build.sh run-main

# 4. Run tests
./build.sh run-tests
```

## Tips

- Use `./build.sh --debug` for debugging with gdb
- Use `make -j$(nproc)` for parallel builds
- Executables are in `build/bin/`
- Use `make help` to see all available targets
