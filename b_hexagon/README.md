
# b_hexagon

A C++17 project following SOLID principles and hexagonal architecture.

## Structure
- `src/domain`: Business logic and core domain models (e.g., `TrackData`)
- `src/application`: Application use cases
- `src/infrastructure`: External services (DB, APIs, etc.)
- `src/adapters`: Input/output adapters (UI, CLI, etc.)
- `include/`: Public headers

## Build

```sh
mkdir build && cd build
cmake ..
make
```

## Run

Add your executable targets in the adapters layer and run them from the build directory.

## Principles
- SOLID principles
- Hexagonal (Ports & Adapters) architecture
