#pragma once

#include <Window.hpp>

#include <Windows.h>

class MainWindow final : public Window {
public:
	MainWindow() noexcept = default;
	MainWindow(const MainWindow&) = delete;
	virtual ~MainWindow() override = default;

public:
	MainWindow& operator=(const MainWindow&) = delete;
	bool operator==(const MainWindow&) = delete;
	bool operator!=(const MainWindow&) = delete;

public:
	virtual LRESULT Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) override;
	
	virtual void Initialize() override;
	virtual void Paint(HDC dc) override;
};