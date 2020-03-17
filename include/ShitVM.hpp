#pragma once

#include <cstddef>
#include <string>

#include <Windows.h>

extern HANDLE ShitVMProcess;
extern std::string ShitVMProcessIdString;

int FindShitVMProcess() noexcept;
bool CheckShitVMProcessValid() noexcept;
template<typename T>
bool ReadShitVMMemory(T& buffer, unsigned long long address) noexcept;
template<typename T>
bool ReadShitVMMemory(T& buffer, const void* address) noexcept;
bool ReadShitVMMemory(void* buffer, const void* address, std::size_t size);

#include "detail/impl/ShitVM.hpp"