#pragma once

#include <vector>

#include <Windows.h>

struct Window final {
	HWND Handle = nullptr;
	std::vector<HWND> Children;

	operator bool() const noexcept;

	void Show(int value) noexcept;
};

bool RegisterWindow() noexcept;
LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
Window MakeWindow(LRESULT(*proc)(HWND, UINT, WPARAM, LPARAM));