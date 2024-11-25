#include <cmath>
#include "Aimbot.h"
#define M_PI 3.14159265358979323846

std::pair<float, float> AimbotAngleYawAndPitch(Vector3 enemy, Vector3 Player) {
	Vector3 delta = { enemy.x - Player.x, enemy.y - Player.y, enemy.z - Player.z };

	float hyp = std::sqrt(delta.x * delta.x + delta.y * delta.y);

	float degreeYaw = (std::atan2(delta.x, -delta.y)) * (180 / M_PI);
	float degreePitch = (std::atan2(delta.z, hyp)) * (180 / M_PI);

	if (degreeYaw < 0) {
		degreeYaw += 360;
	}

	return { degreeYaw, degreePitch };
}