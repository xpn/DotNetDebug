#pragma once

#include "Common.h"

class DebuggerHelper {
public:
    static HRESULT GetProcessList(std::vector<std::pair<DWORD, std::wstring>> **processes);
    static std::wstring StringToWString(const std::string &input);
};