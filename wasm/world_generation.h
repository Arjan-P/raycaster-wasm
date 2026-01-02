#pragma once

const uint64_t GLOBAL_SEED = 0xCAFEBABEDEADBEEFULL;

constexpr int viewRadius = 1;
constexpr int GRID = viewRadius * 2 + 1;
int32_t mapWidth = 16;
int32_t mapHeight = 16;
int32_t mapDepth = 16;

struct Chunk
{
	int tiles[16 * 16];
	int cx;
	int cy;
};

static inline uint64_t splitmix64(uint64_t* state)
{
    uint64_t z = (*state += 0x9E3779B97F4A7C15ULL);
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL;
    return z ^ (z >> 31);
}

inline int worldTileX(int32_t fx)
{
	return fx >> FP_SHIFT;
}

inline int worldTileY(int32_t fy)
{
	return fy >> FP_SHIFT;
}

inline int div32(int x)
{
	return (x >= 0) ? (x >> 5) : ((x - 31) >> 5);
}

inline int mod32(int x)
{
	return x & 31;
}

