#pragma once
#include <cstdint>

constexpr int32_t FP_SHIFT = 16;
constexpr int32_t FP_ONE = 1 << FP_SHIFT;

inline int32_t FloatToFixed(float f)
{
	return (int32_t)(f * FP_ONE);
}

inline int32_t FixedMult(int32_t a, int32_t b)
{
	return (int32_t)(((int64_t)a * b) >> FP_SHIFT);
}

inline int32_t FixedDiv(int32_t a, int32_t b)
{
	return (int32_t)(((int64_t)a << FP_SHIFT) / b);
}

