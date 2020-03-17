#pragma once

#include <Windows.h>

constexpr const char* Title = "ShitVM Developer Tool";
extern HINSTANCE Instance;
extern int CmdShow;

bool Initialize(HINSTANCE instance, int cmdShow) noexcept;