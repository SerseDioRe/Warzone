#pragma once
#include "pch.h"

class ArrayNames
{
public:
	char name[16]; //0x0000
	char pad_0010[120]; //0x0010
}; //Size: 0x0088
static_assert(sizeof(ArrayNames) == 0x88);

class Entity
{
public:
	char name[16];
};