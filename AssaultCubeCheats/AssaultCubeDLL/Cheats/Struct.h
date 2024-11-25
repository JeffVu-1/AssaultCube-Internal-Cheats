#pragma once
template <typename T>
struct Vector {
	T* Buffer;
	int alen, ulen;
};

struct Vector2 {
	float x, y;
};

struct Vector3 {
	float x, y, z;
};

struct Vector4 {
	float x, y, z, w;
};
