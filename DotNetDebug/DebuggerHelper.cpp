#include "DebuggerHelper.h"

HRESULT DebuggerHelper::GetProcessList(std::vector<std::pair<DWORD, std::wstring>> **processes)
{
  std::vector<std::pair<DWORD, std::wstring>> *_processes = new std::vector<std::pair<DWORD, std::wstring>>();

  PROCESSENTRY32 pe32;
  HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
  if (hProcessSnap == INVALID_HANDLE_VALUE)
  {
    return 1;
  }

  pe32.dwSize = sizeof(PROCESSENTRY32);
  Process32First(hProcessSnap, &pe32);

  do
  {
    _processes->push_back(std::pair<DWORD, std::wstring>(pe32.th32ProcessID, std::wstring(pe32.szExeFile)));
  } while (Process32Next(hProcessSnap, &pe32));

  *processes = _processes;

  return S_OK;
}

std::wstring DebuggerHelper::StringToWString(const std::string &input)
{
  std::wstring wsTmp(input.begin(), input.end());
  return wsTmp;
}