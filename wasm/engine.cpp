#include <cmath>
#include <stdio.h>
#include "trig_lut.h"
#include "fixedpoint_helpers.h"
#include "screen.h"
#include "world_generation.h"

using namespace std;

extern "C" {

int32_t playerX = 0;	// world space x
int32_t playerY = 0;	// world space y
int last_chunk_x = 0;
int last_chunk_y = 0;
int playerA = 0;
int FOV = 1024 / 8;

Chunk chunks[GRID][GRID];
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
		framebuffer[i] = rgba(0,0,0,0);
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

void randomFill(Chunk &c)
{
	uint64_t seed = GLOBAL_SEED;
	seed ^= (uint64_t)c.cx * 0x9E3779B97F4A7C15ULL;
	seed ^= (uint64_t)c.cy * 0xBF58476D1CE4E5B9ULL;

	for(int y = 0; y < mapHeight; y++)
	{
		for(int x = 0; x < mapWidth; x++)
		{
			uint32_t r = (uint32_t)splitmix64(&seed);
			c.tiles[y * mapWidth + x] = (r % 100 < 45) ? 1 : 0;
		}
	}
}

void evolve(int gridX, int gridY)
{
	Chunk& c = chunks[gridY][gridX];
	auto sampleCell = [&](int x, int y) -> int
	{
		int ngx = gridX;
		int ngy = gridY;

		if (x < 0)        { x += 32; ngx--; }
		else if (x >= 32){ x -= 32; ngx++; }

		if (y < 0)        { y += 32; ngy--; }
		else if (y >= 32){ y -= 32; ngy++; }

		// out-of-grid = solid wall
		if ((unsigned)ngx >= GRID || (unsigned)ngy >= GRID)
		return 1;

		return chunks[ngy][ngx].tiles[y * 32 + x];
	};

	int newState[mapHeight * mapWidth];
	static const int offset[8][2] =
	{
		{-1,  0}, // N
		{-1,  1}, // NE
		{ 0,  1}, // E
		{ 1,  1}, // SE
		{ 1,  0}, // S
		{ 1, -1}, // SW
		{ 0, -1}, // W
		{-1, -1}  // NW
	};
	for(int y = 0; y < mapHeight; y++)
	{
		for(int x = 0; x < mapWidth; x++)
		{
			int wallCount = 0;
			for(auto &p : offset)
			{
				wallCount += sampleCell(y + p[0], x + p[1]);
			}
			newState[y * mapWidth + x] = (wallCount >= 5) ? 1 : 0;
		}
	}
	for(int y = 0; y < mapHeight; y++)
		for(int x = 0; x < mapWidth; x++)
			c.tiles[y * mapWidth + x] = newState[y * mapWidth + x];
}

void initChunks()
{

	for (int gy = 0; gy < GRID; gy++)
	{
		for (int gx = 0; gx < GRID; gx++)
		{
			Chunk &c = chunks[gy][gx];

			c.cx = last_chunk_x + (gx - viewRadius);
			c.cy = last_chunk_y + (gy - viewRadius);

			randomFill(c);
			//for(int i = 0; i < 3; i++)
				evolve(gx, gy);
		}
	}
}

void panRight()
{
	/*
	ABC	   BC.
	DEF	-> EF.
	GHI	   HI.
	 */
	for(int cy = 0; cy < GRID; cy++)
	{
		for(int cx = 0; cx < GRID - 1; cx++)
		{
			chunks[cy][cx] = chunks[cy][cx + 1];
		}
	}
	int newCX = last_chunk_x + viewRadius + 1;
	for(int cy = 0; cy < GRID; cy++)
	{
		Chunk& c = chunks[cy][GRID - 1];
		c.cx = newCX;
		c.cy = last_chunk_y + (cy - viewRadius);
		randomFill(c);
		//for(int i = 0; i < 3; i++)
			evolve(GRID - 1, cy);
	}
	last_chunk_x++;
}

void panLeft()
{
	/*
	ABC	   .AB
	DEF	-> .DE
	GHI	   .GH
	 */
	for(int cy = 0; cy < GRID; cy++)
	{
		for(int cx = GRID - 1; cx > 0; cx--)
		{
			chunks[cy][cx] = chunks[cy][cx - 1];
		}
	}
	int newCX = last_chunk_x - viewRadius - 1;
	for(int cy = 0; cy < GRID; cy++)
	{
		Chunk& c = chunks[cy][0];
		c.cx = newCX;
		c.cy = last_chunk_y + (cy - viewRadius);
		randomFill(c);
		//for(int i = 0; i < 3; i++)
			evolve(0, cy);
	}
	last_chunk_x--;
}

void panUp()
{
	/*
	ABC	   ...
	DEF	-> ABC
	GHC	   DEF
	 */
	for(int cx = 0; cx < GRID; cx++)
	{
		for(int cy = GRID - 1; cy > 0; cy--)
		{
			chunks[cy][cx] = chunks[cy - 1][cx];
		}
	}
	int newCY = last_chunk_y - viewRadius - 1;
	for(int cx = 0; cx < GRID; cx++)
	{
		Chunk& c = chunks[0][cx];
		c.cx = last_chunk_x + cx - viewRadius;
		c.cy = newCY;

		randomFill(c);
		//for(int i = 0; i < 3; i++)
			evolve(cx, 0);
	}
	last_chunk_y--;
}

void panDown()
{
	/*
	ABC	   DEF
	DEF	-> GHC
	GHC	   ...
	 */
	for(int cx = 0; cx < GRID; cx++)
	{
		for(int cy = 0; cy < GRID - 1; cy++)
		{
			chunks[cy][cx] = chunks[cy + 1][cx];
		}
	}
	int newCY = last_chunk_y + viewRadius + 1;
	for(int cx = 0; cx < GRID; cx++)
	{
		Chunk& c = chunks[GRID - 1][cx];
		c.cx = last_chunk_x + cx - viewRadius;
		c.cy = newCY;

		randomFill(c);
		//for(int i = 0; i < 3; i++)
			evolve(cx, GRID - 1);
	}
	last_chunk_y++;
}

void updateChunks()
{
	int player_chunk_x = div32(worldTileX(playerX));
	int player_chunk_y = div32(worldTileY(playerY));

	while(player_chunk_x > last_chunk_x)
	{
		// pan right
		panRight();
	}

	while(player_chunk_x < last_chunk_x)
	{
		// pan left
		panLeft();
	}

	while(player_chunk_y > last_chunk_y)
	{
		// pan down
		panDown();
	}

	while(player_chunk_y < last_chunk_y)
	{
		// pan up
		panUp();
	}
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
		// world coordinates
		int worldX = worldTileX(playerX);
		int worldY = worldTileY(playerY);
		// chunk coordinates
		int chunkX = div32(worldX);
		int chunkY = div32(worldY);
		// chunk local coordinates i.e 0 .. 31
		int localX = mod32(worldX);
		int localY = mod32(worldY);
		// chunks[gy][gx]
		int gridX = viewRadius;
		int gridY = viewRadius;

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
		bool wallHit = false;
		while(true)
		{
			if(sideDistX < sideDistY)
			{
				// walk along x axis
				sideDistX += deltaDistX;
				worldX += stepX;
				localX += stepX;
				side = 0;	// hit veritical wall 
				// handle crossing chunk boundry
				if(localX == 32)
				{
					localX = 0;
					gridX++;
					chunkX++;
				}
				else if(localX == -1)
				{
					localX = 31;
					gridX--;
					chunkX--;
				}
			}
			else
			{
				// walk along y axix
				sideDistY += deltaDistY;
				worldY += stepY;
				localY += stepY;
				side = 1;	// hit horizontal wall
				// handle crossing grid boundry
				if(localY == 32)
				{
					localY = 0;
					gridY++;
					chunkY++;
				}
				else if(localY == -1)
				{
					localY = 31;
					gridY--;
					chunkY--;
				}
			}
			if ((unsigned)gridX >= GRID || (unsigned)gridY >= GRID)
			{
				wallHit = false;   // or true with max distance
				break;
			}

			if(chunks[gridY][gridX].tiles[localY * mapWidth + localX])
			{
				wallHit = true;
				break;
			}
		}
		// find perpendicular dist from camera plane to wall
		int32_t perpDistance;
		int32_t wallX;
		int32_t wallY;
		int wallShade;
		if(side == 0)
		{
			perpDistance = sideDistX - deltaDistX;
			wallX = (worldX + (stepX < 0)) << FP_SHIFT;
			wallY = playerY + FixedMult(perpDistance, rayY);
			wallShade = 150;
		}
		else
		{
			perpDistance = sideDistY - deltaDistY;
			wallY = (worldY + (stepY < 0)) << FP_SHIFT;
			wallX = playerX + FixedMult(perpDistance, rayX);
			wallShade = 200;
		}

		if(!wallHit)
			wallShade = 0;
		if(perpDistance <= 0)
			perpDistance = FP_ONE / 10;
		int wallHeight = SCREEN_HEIGHT * FP_ONE / perpDistance;
		int ceiling = (SCREEN_HEIGHT/ 2) - (wallHeight / 2);
		if(ceiling < 0)
			ceiling = 0;
		int floor = (wallHeight / 2) + (SCREEN_HEIGHT / 2);
		if(floor > SCREEN_HEIGHT)
			floor = SCREEN_HEIGHT - 1;
		for(int y = 0; y < SCREEN_HEIGHT; y++)
		{
			if(y <= ceiling)
				framebuffer[y * SCREEN_WIDTH + x] = rgba(0,0,0, 255);
			else if(y <= floor)
				framebuffer[y * SCREEN_WIDTH + x] = rgba(wallShade, wallShade, wallShade, 255);
			else
				framebuffer[y * SCREEN_WIDTH + x] = rgba(100, 100, 100, 255);
		}
	}
}
}
