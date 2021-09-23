#pragma once
#include "pch.h"

void Patch(BYTE* destination, BYTE* source, size_t size);

class NopInternal
{
private:
	BYTE* destination;
	size_t size;
	BYTE* originalCode;
	BYTE* nopCode;
	bool m_isEnabled;
public:
	NopInternal(BYTE* destination, size_t size);
	~NopInternal();
	void enable();
	void disable();
	bool IsEnabled();
};

class PatchInternal
{
private:
	BYTE* destination;
	size_t size;
	BYTE* originalCode;
	BYTE* newCode;
public:
	PatchInternal(BYTE* destination, BYTE* source, size_t size);
	~PatchInternal();
	void enable();
	void disable();
};

class Hook
{
private:
	std::vector<BYTE> m_originalCode;
	void* m_pSource;
	void* m_pDestination;
	int m_dwLen;
	DWORD_PTR m_address;
public:
	Hook(void* pSource, void* pDestination, int dwLen);
	void* Init();
	void UnHook();
};
