#pragma once
#include "pch.h"

typedef float vec_t;

struct vec2_t
{
	vec_t x, y;
};

class vec3_t
{
public:

	union
	{
		struct { vec_t x, y, z; };
		vec_t v[3];
	};

	vec3_t() { x = y = z = 0; }
	vec3_t(const vec_t x, const vec_t y, const vec_t z) : x(x), y(y), z(z) {}
	vec3_t operator + (const vec3_t& rhs) const { return vec3_t(x + rhs.x, y + rhs.y, z + rhs.z); }
	vec3_t operator - (const vec3_t& rhs) const { return vec3_t(x - rhs.x, y - rhs.y, z - rhs.z); }
	vec3_t operator * (const vec_t& rhs) const { return vec3_t(x * rhs, y * rhs, z * rhs); }
	vec3_t operator / (const vec_t& rhs) const { return vec3_t(x / rhs, y / rhs, z / rhs); }
	vec3_t& operator += (const vec3_t& rhs) { return *this = *this + rhs; }
	vec3_t& operator -= (const vec3_t& rhs) { return *this = *this - rhs; }
	vec3_t& operator *= (const vec_t& rhs) { return *this = *this * rhs; }
	vec3_t& operator /= (const vec_t& rhs) { return *this = *this / rhs; }
	vec_t dot() const { return x * x + y * y + z * z; }
	vec_t Length() const { return sqrtf(dot()); }
	vec3_t Normalize() const { return *this * (1 / Length()); }
	vec_t Distance(const vec3_t& rhs) const { return (*this - rhs).Length(); }
};

struct vec4_t
{
	vec_t x, y, z, w;
};

enum RadarMode : __int8
{
	RADAR_MODE_NORMAL = 0x0,
	RADAR_MODE_FAST = 0x1,
	RADAR_MODE_CONSTANT = 0x2,
	RADAR_MODE_SLOW = 0x3,
	RADAR_MODE_COUNT = 0x4,
};

class playerState_s
{
public:
	char pad_0000[48]; //0x0000
	vec3_t vecOrig; //0x0030
	char pad_003C[416]; //0x003C
	vec3_t viewAngle; //0x01DC
	char pad_01E8[116]; //0x01E8
	int32_t Health; //0x025C
	char pad_0260[164]; //0x0260
	bool radarEnabled; //0x0304
	bool radarBlocked; //0x0305
	bool radarShowEnemyDirection; //0x0306
	RadarMode radarMode; //0x0307
};