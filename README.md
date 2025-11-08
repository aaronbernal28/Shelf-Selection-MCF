# Shelf-Selection-MCF

C++ implementation of a Shelf Selection optimization system using Minimum Cost Flow algorithms for warehouse logistics.

## Project Structure

```
Shelf-Selection-MCF/
├── src/                    # Shared source files
│   ├── order.cpp/h         # Order data structure
│   ├── db_connector.cpp/h  # Database connection management
│   ├── rack.h              # Warehouse rack definitions
│   └── types.h             # Common type definitions
├── WMS/                    # Warehouse Management System
│   ├── src/
│   │   ├── wms.cpp         # WMS main entry point
│   │   └── publisher.cpp/h # Order publisher to database
│   └── CMakeLists.txt
├── WES/                    # Warehouse Execution System
│   ├── src/
│   │   ├── wes.cpp                 # WES main entry point
│   │   ├── shelf_selection.cpp/h   # Shelf selection logic
│   │   ├── stock.cpp/h             # Stock management
│   │   └── task_manager.cpp/h      # Task execution
│   ├── include/                    # WES headers
│   └── CMakeLists.txt
├── data/
│   ├── raw/            # Input data (backlog.json)
│   └── output/         # Simulation results
├── my_MCF_db/          # Database schema
│   └── schema.sql
└── example.env         # Environment configuration template
```

## Dependencies

- **C++17** compiler (GCC 7+ or Clang 5+)
- **CMake** 3.10+
- **nlohmann-json** 3.2.0+ - JSON parsing
- **libpqxx** - PostgreSQL C++ client
- **PostgreSQL** - Database

Install on Ubuntu/Debian:
```bash
sudo apt-get install build-essential cmake nlohmann-json3-dev libpq-dev libpqxx-dev pkg-config
```

## Setup

### 1. Configure Environment
Create a `.env` file based on `example.env` with your database credentials.

Required environment variables:
- `DB_NAME` - Database name
- `DB_USER` - Database user
- `DB_HOST` - Database host
- `DB_PORT` - Database port
- `DB_PASSWORD` - Database password (optional)

### 2. Setup Database
Check `mcf_db/NOTES.md` for instructions on initializing the PostgreSQL database schema.

### 3. Build

**Build WMS:**
```bash
cd WMS
mkdir build && cd build
cmake ..
make
```

**Build WES:**
```bash
cd WES
mkdir build && cd build
cmake ..
make
```

### 4. Run
```bash
# From WMS/build/
./wms

# From WES/build/
./wes
```

## Components

**WMS (Warehouse Management System)**
- Reads order backlog from JSON (`data/raw/backlog.json`)
- Publishes orders to PostgreSQL database
- Simulates order arrival with configurable speed-up factor

**WES (Warehouse Execution System)**
- Consumes orders from database
- Performs shelf selection optimization
- Manages stock and task execution

**DBConnector** (shared)
- Centralized database connection management
- Loads configuration from environment variables

## Data Format

Orders in `data/raw/backlog.json`:
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

