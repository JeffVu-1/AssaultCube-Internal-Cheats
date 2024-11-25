#include <Windows.h>
#include "Esp.h"

bool WorldToScreen(Vector3 pos, Vector2& screen, float matrix[16], int windowWidth, int windowHeight)
{
	Vector4 clipCoords;
	clipCoords.x = pos.x * matrix[0] + pos.y * matrix[4] + pos.z * matrix[8] + matrix[12];
	clipCoords.y = pos.x * matrix[1] + pos.y * matrix[5] + pos.z * matrix[9] + matrix[13];
	clipCoords.z = pos.x * matrix[2] + pos.y * matrix[6] + pos.z * matrix[10] + matrix[14];
	clipCoords.w = pos.x * matrix[3] + pos.y * matrix[7] + pos.z * matrix[11] + matrix[15];

	if (clipCoords.w < 0.1f)
		return false;

	Vector3 NDC;
	NDC.x = clipCoords.x / clipCoords.w;
	NDC.y = clipCoords.y / clipCoords.w;
	NDC.z = clipCoords.z / clipCoords.w;

	screen.x = (windowWidth / 2 * NDC.x) + (NDC.x + windowWidth / 2);
	screen.y = -(windowHeight / 2 * NDC.y) + (NDC.y + windowHeight / 2);
	return true;
}

void DrawFilledRect(int x, int y, int w, int h, HDC hdcAC_client, HBRUSH Brush)
{
	RECT rect = { x, y, x + w, y + h };
	FillRect(hdcAC_client, &rect, Brush);
}

void DrawBorderBox(int x, int y, int w, int h, int thickness, HDC hdcAC_client, HBRUSH Brush)
{

	DrawFilledRect(x, y, w, thickness, hdcAC_client, Brush);

	DrawFilledRect(x, y, thickness, h, hdcAC_client, Brush);

	DrawFilledRect((x + w), y, thickness, h, hdcAC_client, Brush);

	DrawFilledRect(x, y + h, w + thickness, thickness, hdcAC_client, Brush);
}

//void DrawLine(int targetX, int targetY)
//{
//	MoveToEx(hdcAC_client, 960, 1080, NULL);
//	LineTo(hdcAC_client, targetX, targetY);
//
//}
