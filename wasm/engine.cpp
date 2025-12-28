#include <math.h>

extern "C" {

float playerX = 3.0f;
float playerY = 3.0f;
float playerA = 0.0f;
float FOV = 3.14159f / 4.0f;

int mapWidth = 8;
int mapHeight = 8;
int mapDepth = 8;

int map[] = {
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

Column columns[160];

void move(float f) {
  playerX += f * cosf(playerA);
  playerY += f * sinf(playerA);
}

void rotate(float a) { playerA += a; }

void render(int screenW, int screenH) {
  for (int x = 0; x < screenW; x++) {
    float rayA = (playerA - FOV / 2.0f) + ((float)x / screenW) * FOV;

    float rayX = cosf(rayA);
    float rayY = sinf(rayA);

    float dist = 0;
    bool hit = false;

    while (!hit && dist < mapDepth) {
      int rx = (int)(playerX + dist * rayX);
      int ry = (int)(playerY + dist * rayY);

      if (map[ry * mapWidth + rx] == 1)
        hit = true;
      dist += 0.1f;
    }

    float correctedDist = dist * cosf(rayA - playerA);
    if (correctedDist < 0.0001f)
      correctedDist = 0.0001f;
    int ceiling = screenH / 2 - screenH / correctedDist;
    int floor = screenH - ceiling;

    columns[x] = {ceiling, floor};
  }
}

Column *get_columns() { return columns; }
}
