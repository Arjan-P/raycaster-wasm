#include <cmath>
#include "trig_lut.h"
#include "fixedpoint_helpers.h"
#include "screen.h"

extern "C" {

int32_t playerX = FloatToFixed(16.0f);
int32_t playerY = FloatToFixed(16.0f);
int playerA = 0;
int FOV = 1024 / 8;

int32_t mapWidth = 32;
int32_t mapHeight = 32;
int32_t mapDepth = 16;

int32_t map[] = {
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,

};

uint32_t* getFramebuffer()
{
	return framebuffer;
}

int getWidth()
{
	return SCREEN_WIDTH;
}

int getHeight()
{
	return SCREEN_HEIGHT;
}

void clear()
{
	for(int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++)
	{
		framebuffer[i] = rgba(0,0,0,255);
	}
}

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

void render()
{
	clear();
	for (int x = 0; x < SCREEN_WIDTH; x++)
	{
		int rayA = (playerA - FOV / 2) + (x * (FOV) / SCREEN_WIDTH);
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
		int32_t wallX;
		int32_t wallY;
		int32_t frac;
		int a;
		if(side == 0)
		{
			perpDistance = sideDistX - deltaDistX;
			wallX = (mapX + (stepX < 0)) << FP_SHIFT;
			wallY = playerY + FixedMult(perpDistance, rayY);
			frac = wallY & (FP_ONE - 1);
			a = 255;
		}
		else
		{
			perpDistance = sideDistY - deltaDistY;
			wallY = (mapY + (stepY < 0)) << FP_SHIFT;
			wallX = playerX + FixedMult(perpDistance, rayX);
			frac = wallX & (FP_ONE - 1);
			a = 180;
		}
		if(perpDistance <= 0)
			perpDistance = FP_ONE * 0.5;

		int wallHeight = SCREEN_HEIGHT * FP_ONE / perpDistance;
		int ceiling = (SCREEN_HEIGHT/ 2) - (wallHeight / 2);
		if(ceiling < 0)
			ceiling = 0;
		int floor = (wallHeight / 2) + (SCREEN_HEIGHT / 2);
		if(floor > SCREEN_HEIGHT)
			floor = SCREEN_HEIGHT - 1;
		for(int y = 0; y < SCREEN_HEIGHT; y++)
		{
			if(y <= ceiling)	// shade sky
			{
				framebuffer[y * SCREEN_WIDTH + x] = rgba(0,0,0, 255);
			}
			else if(y <= floor)	// shade wall
			{
				framebuffer[y * SCREEN_WIDTH + x] = rgba(0, 180, 0, a);
			}
			else	// shade floor
			{
				framebuffer[y * SCREEN_WIDTH + x] = rgba(100, 100, 100, 255);
			}
		}
	}
}
}
