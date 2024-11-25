#pragma once
#include "Struct.h"

bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight);

void DrawFilledRect(int x, int y, int w, int h, HDC hdcAC_client, HBRUSH Brush);

void DrawBorderBox(int x, int y, int w, int h, int thickness, HDC hdcAC_client, HBRUSH Brush);

//void DrawLine(int targetX, int targetY);

