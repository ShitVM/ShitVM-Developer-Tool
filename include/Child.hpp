#pragma once

#include <functional>

#include <Windows.h>

class Window;

struct Child final {
	HWND Handle = nullptr;
	LRESULT(*Callback)(Window* window, UINT message, WPARAM wParam, LPARAM lParam) = nullptr;

	Child& SetFont(HFONT font);
};

Child CreateButton(const Window* window, const char* text, int x, int y, int w, int h, LRESULT(*callback)(Window* window, UINT message, WPARAM wParam, LPARAM lParam));

#define CallbackLambda(c) [c](Window* window, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT