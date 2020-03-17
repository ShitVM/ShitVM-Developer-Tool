#pragma once

#include <functional>

#include <Windows.h>

class Window;

using Callback = std::function<LRESULT(Window*, UINT, WPARAM, LPARAM)>;

struct Child final {
	HWND Handle = nullptr;
	::Callback Callback;

	Child& Enable() noexcept;
	Child& Disable() noexcept;

	Child& SetFont(HFONT font) noexcept;
	Child& SetWndProc(WNDPROC newProc) noexcept;
};

Child CreateButton(const Window* window, const char* text, int x, int y, int w, int h, const Callback& callback);
Child CreateTextBox(const Window* window, int x, int y, int w, int h, const Callback& callback);
Child CreateCheckBox(const Window* window, const char* text, int x, int y, int w, int h, const Callback& callback);

#define CallbackLambda(...) [__VA_ARGS__](Window* window, UINT message, WPARAM wParam, LPARAM lParam) -> LRESULT