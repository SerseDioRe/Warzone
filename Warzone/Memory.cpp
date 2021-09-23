#include "Memory.h"

void Patch(BYTE* destination, BYTE* source, size_t size)
{
	DWORD oldProtect;
	VirtualProtect((LPVOID)destination, size, PAGE_EXECUTE_READWRITE, &oldProtect);
	memcpy(destination, source, size);
	VirtualProtect(destination, size, oldProtect, &oldProtect);
}

NopInternal::NopInternal(BYTE* destination, size_t size)
    :destination{ destination }, size{ size }, originalCode{ new BYTE[size] }, nopCode{ new BYTE[size] }, m_isEnabled{ false }
{
	memset(nopCode, 0x90, size);
	memcpy(originalCode, destination, size);
}

void NopInternal::enable()
{
	Patch(destination, nopCode, size);
    m_isEnabled = true;
}

void NopInternal::disable()
{
	Patch(destination, originalCode, size);
    m_isEnabled = false;
}

bool NopInternal::IsEnabled()
{
    return m_isEnabled;
}

NopInternal::~NopInternal()
{
	delete[] this->nopCode;
	delete[] this->originalCode;
}

PatchInternal::PatchInternal(BYTE* destination, BYTE* source, size_t size) : destination{ destination }, size{ size },
originalCode{ new BYTE[size] }, newCode{ new BYTE[size] }
{
	memcpy(newCode, source, size);
	memcpy(originalCode, destination, size);
}

void PatchInternal::enable()
{
	Patch(destination, newCode, size);
}

void PatchInternal::disable()
{
	Patch(destination, originalCode, size);
}

PatchInternal::~PatchInternal()
{
	delete[] this->originalCode;
	delete[] this->newCode;
}

Hook::Hook(void* pSource, void* pDestination, int dwLen) :m_pSource{ pSource }, m_pDestination{ pDestination },
m_dwLen{ dwLen }, m_address{ (DWORD_PTR)pSource }
{
    for (int i = 0; i < m_dwLen; i++)
    {
        m_originalCode.push_back(((BYTE*)(DWORD_PTR)m_pSource)[i]);
    }
}

void* Hook::Init()
{
    DWORD MinLen = 14;

    if (m_dwLen < MinLen) return NULL;

    BYTE stub[] = {
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
    };

    void* pTrampoline = VirtualAlloc(0, m_dwLen + sizeof(stub), MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

    DWORD dwOld = 0;
    VirtualProtect(m_pSource, m_dwLen, PAGE_EXECUTE_READWRITE, &dwOld);

    DWORD64 retto = (DWORD64)m_pSource + m_dwLen;

    // trampoline
    memcpy(stub + 6, &retto, 8);
    memcpy((void*)((DWORD_PTR)pTrampoline), m_pSource, m_dwLen);
    memcpy((void*)((DWORD_PTR)pTrampoline + m_dwLen), stub, sizeof(stub));

    // orig
    memcpy(stub + 6, &m_pDestination, 8);
    memcpy(m_pSource, stub, sizeof(stub));

    for (int i = MinLen; i < m_dwLen; i++)
    {
        *(BYTE*)((DWORD_PTR)m_pSource + i) = 0x90;
    }

    VirtualProtect(m_pSource, m_dwLen, dwOld, &dwOld);
    return (void*)((DWORD_PTR)pTrampoline);
}

void Hook::UnHook()
{
    DWORD dwOld = 0;
    VirtualProtect(m_pSource, m_dwLen, PAGE_EXECUTE_READWRITE, &dwOld);
    for (int i = 0; i < m_dwLen; i++)
    {
        ((BYTE*)(DWORD_PTR)m_address)[i] = m_originalCode[i];
    }
    VirtualProtect(m_pSource, m_dwLen, dwOld, &dwOld);
}