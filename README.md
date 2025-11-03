# Shelf-Selection-MCF

C++ implementation of a Shelf Selection optimization system using Minimum Cost Flow algorithms for warehouse logistics.

## Project Structure

```
Shelf-Selection-MCF/
├── include/              # Header files (.h) - Class declarations
│   ├── order.h          # Order class
│   ├── task.h           # Task class
│   ├── publisher.h      # Publisher (order queue manager)
│   ├── shelf_selection.h # Core MCF optimization
│   └── task_manager.h   # Task execution manager
├── src/                 # Implementation files (.cpp)
│   ├── main.cpp         # Entry point + main simulation logic
│   ├── order.cpp        # Order implementation
│   ├── task.cpp         # Task implementation
│   ├── publisher.cpp    # Publisher implementation
│   ├── shelf_selection.cpp # Shelf selector implementation
│   └── task_manager.cpp # Task manager implementation
├── tests/               # Unit tests
│   ├── test_order.cpp   # Order class tests
│   ├── test_task.cpp    # Task class tests
│   ├── test_publisher.cpp # Publisher tests
│   └── README.md        # Test documentation
├── data/
│   ├── raw/            # Input data (backlog.json, stock.json)
│   └── output/         # Simulation results
├── my_MCF_db/          # Database schema and examples
│   ├── schema.sql      # PostgreSQL database schema
│   ├── insert_example.sql
│   └── NOTES.md
├── CMakeLists.txt      # CMake build configuration
├── Makefile            # Convenience makefile wrapper
├── build.sh            # Build automation script
├── BUILD_GUIDE.md      # Quick reference for building
└── README.md           # This file
```

## Dependencies

### Required
- **C++17 compiler** (GCC 7+ or Clang 5+)
- **CMake** 3.10+
- **nlohmann-json** 3.2.0+ - JSON parsing
- **libpqxx** - PostgreSQL C++ client
- **PostgreSQL** 12+ - Database

### Optional
- **OR-Tools** - Google's optimization tools

## Quick Start

### 1. Install Dependencies
```bash
./build.sh deps
```

### 2. Setup Database
```bash
# Create database
sudo -u postgres psql -c "CREATE DATABASE my_MCF_db;"
sudo -u postgres psql -c "CREATE USER aaron WITH PASSWORD 'password';"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE my_MCF_db TO aaron;"

# Initialize schema
psql -U aaron -d my_MCF_db -f my_MCF_db/schema.sql
```

### 3. Build
```bash
./build.sh build
```

### 4. Run
```bash
./build.sh run-publisher  # Publish orders to database
./build.sh run-main       # Run main application
./build.sh run-tests      # Run tests
```

## Build Commands

See [BUILD_GUIDE.md](BUILD_GUIDE.md) for detailed build instructions.

**Common commands:**
```bash
./build.sh build          # Build everything
./build.sh main           # Build main only
./build.sh publisher      # Build publisher only
./build.sh tests          # Build tests
./build.sh clean          # Clean build
./build.sh --debug build  # Debug build
```

## Data Files

Place JSON data files in `data/raw/`:

**backlog.json** - Order backlog:
```json
{
  "orders": [
    {
      "order_id": "ORD_013387_LXJY4YBS_000",
      "item_id": "LXJY4YBSWCX3KBF",
      "quantity": 1,
      "creation_date": "2025-10-09T00:00:04.885077",
      "due_date": "2025-10-09T21:00:00.885077"
    }
  ]
}
```

**stock.json** - Warehouse configuration

## Architecture

Core components in the `SS` namespace:

- **Order**: Customer order with item, quantity, dates
- **Task**: Grouped orders for picking
- **Publisher**: Publishes orders to PostgreSQL from JSON backlog
- **ShelfSelector**: MCF-based optimization for shelf selection
- **TaskManager**: Task execution management
- **Stock/Rack**: Warehouse structure

## Troubleshooting

**Missing dependencies:**
```bash
sudo apt-get install nlohmann-json3-dev libpq-dev libpqxx-dev pkg-config
```

