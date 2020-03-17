#pragma once

#include <Child.hpp>

#include <vector>

#include <Windows.h>

class Window {
public:
	HWND Handle = nullptr;
	std::vector<Child> Children;

public:
	Window() noexcept = default;
	Window(Window&& window) noexcept;
	virtual ~Window();

public:
	Window& operator=(Window&& window) noexcept;
	bool operator==(const Window&) = delete;
	bool operator!=(const Window&) = delete;
	operator bool() const noexcept;

public:
	virtual LRESULT Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	void Create();
	virtual void Initialize() = 0;
	void Show(int value);

	Child& AddChild(const Child& child);
};

bool RegisterWindow() noexcept;
LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);