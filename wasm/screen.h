#pragma once
constexpr int SCREEN_WIDTH = 1200;
constexpr int SCREEN_HEIGHT = 900;

static uint32_t framebuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
inline uint32_t rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) {
    return (a << 24) | (b << 16) | (g << 8) | r;
}
