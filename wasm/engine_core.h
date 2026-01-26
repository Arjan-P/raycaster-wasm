#pragma once
#include "fixedpoint_helpers.h"
#include "trig_lut.h"
#include <cstdint>

const	int SCREEN_WIDTH = 600;
const	int SCREEN_HEIGHT = 450;
const int MAP_SCREEN_TILE = 20;

struct Player
{
	int32_t x;
	int32_t y;
	int a;
};

class Engine
{
public:
	Engine();
	Engine(int px, int py, int pa);
	~Engine();
	void move(float amnt);
	void rotate(int a);
	void render();
	uint8_t* getMapBuffer(int _mapWidth, int _mapHeight);
	void drawMapBg();
	uint32_t* getFramebuffer()
	{
		return framebuffer;
	}

	uint32_t* getTopDownFramebuffer()
	{
		return td_framebuffer;
	}

	int getScreenWidth()
	{
		return SCREEN_WIDTH;
	}

	int getScreenHeight()
	{
		return SCREEN_HEIGHT;
	}

	int getMapScreenWidth()
	{
		return map_screen_width;
	}

	int getMapScreenHeight()
	{
		return map_screen_height;
	}

	const Player& playerRef()
	{
		return player;
	}

private:
	Player player;
	int mapHeight;
	int mapWidth;
	uint8_t *map;
	int map_screen_height;
	int map_screen_width;
	uint32_t framebuffer[SCREEN_HEIGHT * SCREEN_WIDTH];
	uint32_t* td_framebuffer;
	uint32_t* btd_framebuffer;
	void clear();
	uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (a << 24) | (b << 16) | (g << 8) | r;
	}
};
