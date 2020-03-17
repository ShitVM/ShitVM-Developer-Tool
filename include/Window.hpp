#pragma once

#include <vector>

#include <Windows.h>

class Window {
public:
	HWND Handle = nullptr;
	std::vector<HWND> Children;

public:
	Window();
	Window(Window&& window) noexcept;
	virtual ~Window();

public:
	Window& operator=(Window&& window) noexcept;
	bool operator==(const Window&) = delete;
	bool operator!=(const Window&) = delete;
	operator bool() const noexcept;

public:
	virtual LRESULT Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	void Show(int value) noexcept;
};

bool RegisterWindow() noexcept;
LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);