#pragma once
#include <ShitVM.hpp>

template<typename T>
bool ReadShitVMMemory(T& buffer, unsigned long long address) noexcept {
	return ReadShitVMMemory<T>(buffer, reinterpret_cast<const void*>(address));
}
template<typename T>
bool ReadShitVMMemory(T& buffer, const void* address) noexcept {
	return ReadProcessMemory(ShitVMProcess, address, &buffer, sizeof(buffer), nullptr);
}