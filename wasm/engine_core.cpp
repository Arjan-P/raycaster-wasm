#include <cmath>
#include <cstdint>
#include "engine_core.h"
#include "fixedpoint_helpers.h"

Engine::Engine()
{
	player.x = 8 * FP_ONE;
	player.y = 8 * FP_ONE;
	player.a = 0;
	FOV = 1024 / 4;
}

Engine::~Engine()
{
	delete[] td_framebuffer;
}

void Engine::move(int32_t f)
{
	player.x += FixedMult(f, sinLUT[player.a]);
	player.y += FixedMult(f, cosLUT[player.a]);
}

void Engine::rotate(int a)
{
	player.a = (player.a - a);
	player.a &= ANGLE_MASK;
}

void Engine::setMap(int _mapWidth, int _mapHeight, uint8_t* _map)
{
	mapHeight = _mapHeight;
	mapWidth = _mapWidth;
	map_screen_height = mapHeight * MAP_SCREEN_TILE;
	map_screen_width = mapWidth * MAP_SCREEN_TILE;
	map = _map;
	td_framebuffer = new uint32_t[map_screen_width * map_screen_height];
}


void Engine::clear()
{
	for(int i = 0; i < SCREEN_HEIGHT * SCREEN_WIDTH; i++)
	{
		if(i < map_screen_height * map_screen_width)
			td_framebuffer[i] = rgba(0,0,0,255);
		framebuffer[i] = rgba(0,0,0,255);
	}
}

void Engine::render()
{
	clear();
	static int leftRay[2];
	static int rightRay[2];
	int32_t dirX = sinLUT[player.a];
	int32_t dirY = cosLUT[player.a];

	constexpr int32_t PLANE_SCALE = (int32_t)(0.66 * FP_ONE);

	int32_t planeX = FixedMult( -dirY, PLANE_SCALE);
	int32_t planeY = FixedMult( dirX, PLANE_SCALE);

	for(int x = 0; x < SCREEN_WIDTH; x++)
	{
		int32_t cameraX = ((int32_t)x * 2 * FP_ONE) / SCREEN_WIDTH - FP_ONE;

		// x component of ray vector
		int32_t rayX = dirX + FixedMult(planeX, cameraX);
		// y component of ray vector
		int32_t rayY = dirY + FixedMult(planeY, cameraX);
		// change in ray distance by one step along x axis
		int32_t deltaDistY = (rayY == 0) ? INT32_MAX : FixedDiv(FP_ONE, abs(rayY));
		// change in ray distance by one step along y axis
		int32_t deltaDistX = (rayX == 0) ? INT32_MAX : FixedDiv(FP_ONE, abs(rayX));
		// step in +x or -x
		int stepX;
		// step in +y or -y
		int stepY;
		// ray dist to immediate next x grid line
		int32_t sideDistX;
		// ray dist to immediate next y grid line
		int32_t sideDistY;
		int mapX = player.x >> FP_SHIFT;
		int mapY = player.y >> FP_SHIFT;
		if(rayX >= 0)
		{
			stepX = 1;
			// 1 - (player.x - mapX);
			sideDistX = FixedMult(FP_ONE - (player.x & (FP_ONE - 1)), deltaDistX);
		}
		else
		{
			stepX = -1;
			// player.x - mapX
			sideDistX = FixedMult(player.x & (FP_ONE - 1), deltaDistX);
		}
		if(rayY >= 0)
		{
			stepY = 1;
			// 1 - (player.x - mapX)
			sideDistY = FixedMult(FP_ONE - (player.y & (FP_ONE - 1)), deltaDistY);
		}
		else
		{
			stepY = -1;
			// player.y - mapY
			sideDistY = FixedMult(player.y & (FP_ONE - 1), deltaDistY);
		}

		bool wallHit = false;
		int wallSide = 0;
		for(int i = 0; i < 1024; i++)
		{
			if(sideDistX < sideDistY)
			{
				sideDistX += deltaDistX;
				mapX += stepX;
				wallSide = 0;
			}
			else
			{
				sideDistY += deltaDistY;
				mapY += stepY;
				wallSide = 1;
			}
			if(mapX < 0 || mapX >= mapWidth || mapY < 0 || mapY >= mapHeight)
			{
				break;
			}
			if(map[mapY * mapWidth + mapX])
			{
				wallHit = true;
				break;
			}
		}

		int32_t perpDistance;
		int32_t wallX;
		int32_t wallY;
		if(wallSide)
		{
			// hit y boundry
			perpDistance = sideDistY - deltaDistY;
			wallY = (mapY + (stepY < 0)) << FP_SHIFT;
			wallX = player.x + FixedMult(perpDistance, rayX);
		}
		else
		{
			// hit x boundry
			perpDistance = sideDistX - deltaDistX;
			wallX = (mapX + (stepX < 0)) << FP_SHIFT;
			wallY = player.y + FixedMult(perpDistance, rayY);
		}
		if(x == 0)
		{
			leftRay[0] = wallX;
			leftRay[1] = wallY;
		}
		if(x == SCREEN_WIDTH - 1)
		{
			rightRay[0] = wallX;
			rightRay[1] = wallY;
		}
		uint32_t wallShade;
		if(!wallHit)
		{
			wallShade = rgba(0, 0, 0);
		}
		else
		{
			if(wallSide)
				wallShade = rgba(0, 50, 150, 255);
			else
				wallShade = rgba(0, 25, 125, 255);
		}

		if (perpDistance < FP_ONE / 64)
    	perpDistance = FP_ONE / 64;

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
				framebuffer[y * SCREEN_WIDTH + x] = rgba(0, 0, 0, 255);
			}
			else if(y <= floor)	// shade wall
			{
				// framebuffer[y * SCREEN_WIDTH + x] = rgba(50, 100, 150, 255);
				framebuffer[y * SCREEN_WIDTH + x] = wallShade;
			}
			else
			{
				framebuffer[y * SCREEN_WIDTH + x] = rgba(100, 100, 100, 255);
			}
		}		
	}
	auto drawLine = [&](int x0, int y0, int x1, int y1, uint32_t color)
	{
	    int dx = abs(x1 - x0);
	    int dy = abs(y1 - y0);

	    int sx = (x0 < x1) ? 1 : -1;
	    int sy = (y0 < y1) ? 1 : -1;

	    int err = dx - dy;

	    while (true)
	    {
	        if (x0 >= 0 && x0 < map_screen_width &&
	            y0 >= 0 && y0 < map_screen_height)
	        {
	            td_framebuffer[y0 * map_screen_width + x0] = color;
	        }

	        if (x0 == x1 && y0 == y1)
	            break;

	        int e2 = err << 1;
	        if (e2 > -dy) { err -= dy; x0 += sx; }
	        if (e2 <  dx) { err += dx; y0 += sy; }
	    }
	};

	for(int y = 0; y < map_screen_height; y++)
	{
		for(int x = 0; x < map_screen_width; x++)
		{
			int coarseX = x / MAP_SCREEN_TILE;
			int coarseY = y / MAP_SCREEN_TILE;
			int fineX = x % MAP_SCREEN_TILE;
			int fineY = y % MAP_SCREEN_TILE;
			int px = (player.x * MAP_SCREEN_TILE) >> FP_SHIFT;
			int py = (player.y * MAP_SCREEN_TILE) >> FP_SHIFT;

			int rlx = (leftRay[0] * MAP_SCREEN_TILE) >> FP_SHIFT;
			int rly = (leftRay[1] * MAP_SCREEN_TILE) >> FP_SHIFT;
			int rrx = (rightRay[0] * MAP_SCREEN_TILE) >> FP_SHIFT;
			int rry = (rightRay[1] * MAP_SCREEN_TILE) >> FP_SHIFT;

 			td_framebuffer[y * map_screen_width + x] = rgba(100, 100, 100, 255);

			// draw walls
			if(map[coarseY * mapWidth + coarseX])
				td_framebuffer[y * map_screen_width + x] = rgba(50, 100, 150, 255);

			// draw player
			for(int i = py - 1; i < py + 1; i++)
			{
				for(int j = px - 1; j < px + 1; j++)
				{
					td_framebuffer[i * map_screen_width + j] = rgba(255, 0, 0, 255);
				}
			}

			drawLine(px, py, rlx, rly, rgba(255, 255, 0, 255));
			drawLine(px, py, rrx, rry, rgba(255, 255, 0, 255));
			
			// draw boundries
			if(y % MAP_SCREEN_TILE == 0 || x % MAP_SCREEN_TILE == 0)
				td_framebuffer[y * map_screen_width + x] = rgba(0, 0, 0, 255);
		}
	}
}
