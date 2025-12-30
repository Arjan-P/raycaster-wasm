#include <cmath>
#include "trig_lut.h"
#include "fixedpoint_helpers.h"

extern "C" {

int32_t playerX = FloatToFixed(7.0f);
int32_t playerY = FloatToFixed(7.0f);
int playerA = 0;
int FOV = 1024 / 8;

int32_t mapWidth = 16;
int32_t mapHeight = 16;
int32_t mapDepth = 16;

int32_t map[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
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
	playerA = (playerA + a);
	playerA &= ANGLE_MASK;
}

void render(int screenW, int screenH)
{
	for (int x = 0; x < screenW; x++)
	{
		int rayA = (playerA - FOV / 2) + (x * (FOV) / screenW);
		rayA &= ANGLE_MASK;

		int32_t rayX = cosLUT[rayA];
		int32_t rayY = sinLUT[rayA];
		int32_t deltaDistX = (rayX == 0) ? INT32_MAX : FixedDiv(FP_ONE, abs(rayX));	// dist traveled by ray when stepping one unit along x axis
		int32_t deltaDistY = (rayY == 0) ? INT32_MAX : FixedDiv(FP_ONE, abs(rayY));	// dist traveled by ray when stepping one unit along y axix
		int stepX;
		int stepY;
		int32_t sideDistX;
		int32_t sideDistY;
		int mapX = playerX >> FP_SHIFT;
		int mapY = playerY >> FP_SHIFT;
		if(rayX < 0)
		{
			stepX = -1;
			// dist to immediate left grid line
			sideDistX = FixedMult(playerX & (FP_ONE - 1), deltaDistX);
		}
		else
		{
			stepX = 1;
			// dist to immediate right grid line
			sideDistX = FixedMult(FP_ONE - (playerX & (FP_ONE - 1)), deltaDistX);
		}
		if(rayY < 0)
		{
			stepY = -1;
			// dist to immediate south grid line
			sideDistY = FixedMult(playerY & (FP_ONE - 1), deltaDistY);
		}
		else
		{
			stepY = 1;
			// dist to immediate north grid line
			sideDistY = FixedMult(FP_ONE - (playerY & (FP_ONE - 1)), deltaDistY);
		}

		int side;
		while(true)
		{
			if(sideDistX < sideDistY)
			{
				// walk along x axis
				sideDistX += deltaDistX;
				mapX += stepX;
				side = 0;	// hit veritical wall 
			}
			else
			{
				// walk along y axix
				sideDistY += deltaDistY;
				mapY += stepY;
				side = 1;	// hit horizontal wall
			}
			if(mapX < 0 || mapX >= mapWidth || mapY < 0 || mapY >= mapHeight)
				break;
			if(map[mapY * mapWidth + mapX])
				break;
		}
		// find perpendicular dist from camera plane to wall
		int32_t perpDistance;
		if(side == 0)
			perpDistance = sideDistX - deltaDistX;
		else
			perpDistance = sideDistY - deltaDistY;

		int wallHeight = screenH * FP_ONE / perpDistance;
		int ceiling = (screenH / 2) - (wallHeight);
		int floor = screenH - ceiling;

		columns[x] = {ceiling, floor};
	}
}

Column *get_columns() { return columns; }
}
