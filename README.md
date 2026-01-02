# Chunked Raycasting Engine

<img width="1196" height="894" alt="2026-01-02-085116_1196x894_escrotum" src="https://github.com/user-attachments/assets/392874cc-abc8-446a-ba41-f9c0d4cf580c" />

A **deterministic, chunk-based raycasting engine** written in **C++** with a focus on **low-level control**, **fixed‑point math**, and **procedural world generation**.
The engine renders a Wolfenstein‑style 3D view while supporting **infinite scrolling worlds** through chunk streaming and cellular automata–based terrain generation.

This project is intentionally minimal and educational: no STL heavy abstractions, no floating point at runtime, and no external rendering libraries.

---

## Features

- **Chunked world system** (32x32 tiles per chunk)
- **Infinite scrolling** via chunk panning (no reallocations)
- **Fast DDA raycasting** with fixed‑point math
- **Division‑free hot path** (bitwise chunk/local tracking)
- **Deterministic procedural generation** (SplitMix64)
- **Cellular Automata cave generation** across chunk borders
- **Software renderer** (framebuffer based)

---

## World Layout

- Each **chunk** is `32x32` tiles
- Active world is a `GRID×GRID` window centered on the player
- Chunks are addressed in two coordinate spaces:

### World Chunk Coordinates

Used for deterministic generation:

```text
(cx, cy) ∈ ℤ
```

### Grid Coordinates (Loaded Window)

Used for fast lookup during rendering:

```text
(gx, gy) ∈ [0, GRID)
```

Chunks are **panned** when the player crosses chunk boundaries:

- No reallocation
- No modulo during ray traversal
- Only edge chunks are regenerated

---

## Rendering Pipeline

### Raycasting

- Classic **DDA (Digital Differential Analyzer)**
- Fixed‑point math (`16.16`)
- Wall shading based on hit side

### Fast Chunk Traversal

During DDA stepping:

- Track **local tile coords** (`0..31`)
- Track **grid chunk indices** (`gx, gy`)
- Adjust only when crossing chunk edges

No `div32()` or `%32` in the hot loop

---

## Procedural Generation

### Random Fill

Each chunk is seeded deterministically:

```cpp
seed = GLOBAL_SEED ^ f(cx, cy)
```

This guarantees:

- Same world every run
- Infinite space
- No stored world state

### Cellular Automata (CA)

Used to turn noise into caves/walls.

Rule (example):

- Count 8 neighbors
- `>= 5 walls → wall`
- `< 5 → empty`

---

## Intent

- Implement Raystepping with DDA
- Learn and try out fixed point representation for the fixed point look
- learn how to generating Deterministic worlds

---

## Build

```bash
mkdir build && cd build
emcmake cmake ..
cmake --build .
```

Try in-browser demo: (https://arjan-p.github.io/raycaster-wasm)
