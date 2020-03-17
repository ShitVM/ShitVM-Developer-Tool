#pragma once

#include <Window.hpp>

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
};