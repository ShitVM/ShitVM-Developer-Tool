#pragma once

#include <functional>

#include <Windows.h>

class Window;

using Callback = std::function<LRESULT(Window*, UINT, WPARAM, LPARAM)>;

struct Child final {
	HWND Handle = nullptr;
	::Callback Callback;

	Child& Enable();
	Child& Disable();

	Child& SetFont(HFONT font);
};

Child CreateButton(const Window* window, const char* text, int x, int y, int w, int h, const Callback& callback);

#define CallbackLambda(c) [c](Window* window, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT