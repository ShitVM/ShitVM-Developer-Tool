#include <Application.hpp>

#include <Window.hpp>

HINSTANCE Instance = nullptr;
int CmdShow = 0;

bool Initialize(HINSTANCE instance, int cmdShow) noexcept {
	Instance = instance;
	CmdShow = cmdShow;

	return RegisterWindow();
}