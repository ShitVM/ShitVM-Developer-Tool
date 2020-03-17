#pragma once

#include <string>

#include <Windows.h>

extern HANDLE ShitVMProcess;
extern std::string ShitVMProcessIdString;

bool FindShitVMProcess() noexcept;
bool CheckShitVMProcessValid() noexcept;