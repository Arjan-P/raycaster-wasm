#include <cmath>
#include <iostream>
#include "trig_lut.h"

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

extern "C" {

int32_t playerX = FloatToFixed(3.0f);
int32_t playerY = FloatToFixed(3.0f);
int playerA = 0;
int FOV = 256;

int32_t mapWidth = 8;
int32_t mapHeight = 8;
int32_t mapDepth = 8;

int32_t map[] = {
  1, 1, 1, 1, 1, 1, 1, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 0, 0, 0, 0, 0, 0, 1,
  1, 1, 1, 1, 1, 1, 1, 1
};

struct Column {
  int ceiling;
  int floor;
};

Column columns[800];

void move(float f)
{
	int32_t step = FloatToFixed(f);
	playerX += FixedMult(step, cosLUT[playerA]);
	playerY += FixedMult(step, sinLUT[playerA]);
}

void rotate(int a)
{ 
	playerA = (playerA + a) & ANGLE_MASK;
}

void render(int screenW, int screenH)
{
	std::cout << playerA << "\n" << std::endl;
	for (int x = 0; x < screenW; x++)
	{
		int rayA = playerA - (FOV / 2) + (x * FOV) / screenW; 
		rayA &= ANGLE_MASK;

		int32_t rayX = sinLUT[rayA];
		int32_t rayY = cosLUT[rayA];

		int32_t dist = 0;
		bool hit = false;

		while (!hit && dist < FloatToFixed(mapDepth))
		{
			int32_t rx = playerX + FixedMult(dist, rayX);
			int32_t ry = playerY + FixedMult(dist, rayY);

			int mx = rx >> FP_SHIFT;
			int my = ry >> FP_SHIFT;

			if(mx < 0 || mx >= mapWidth ||
				my < 0 || my >= mapHeight)
			{
				hit = true;
				break;
			}
			if (map[my * mapWidth + mx] == 1)
				hit = true;
			dist += FP_ONE / 10;
		}

		int angleDif = (rayA - playerA) & ANGLE_MASK;
		int32_t correctedDist = FixedMult(dist, cosLUT[angleDif]);
		if(correctedDist < FP_ONE / 8)
			correctedDist = FP_ONE / 8;

		int ceiling = (screenH / 2) - (screenH * FP_ONE / correctedDist);
		int floor = screenH - ceiling;

		columns[x] = {ceiling, floor};
	}
}

Column *get_columns() { return columns; }
}
