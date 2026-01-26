#include "engine_core.h"
#include <cstdint>

using namespace std;

static Engine engine;

extern "C" {

void init(int px, int py, int pa)
{
	engine = Engine(px, py, pa);
}

uint32_t* engine_screenbuffer()
{
	return engine.getFramebuffer();
}

int engine_screen_width()
{
	return engine.getScreenWidth();	
}


int engine_screen_height()
{
	return engine.getScreenHeight();
}

uint32_t* engine_top_down_screenbuffer()
{
	return engine.getTopDownFramebuffer();
}

int engine_map_screen_width()
{
	return engine.getMapScreenWidth();
}

int engine_map_screen_height()
{
	return engine.getMapScreenHeight();
}

void engine_draw_map_bg()
{
	engine.drawMapBg();
}

uint8_t* engine_map_buffer(int mapWidth, int mapHeight)
{
	return engine.getMapBuffer(mapWidth, mapHeight);
}

void engine_move(float amount)
{
	engine.move(amount);
}

void engine_rotate(int amount)
{
	engine.rotate(amount);
}

void engine_render()
{
	engine.render();
}

}
