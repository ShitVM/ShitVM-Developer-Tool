#include <Application.hpp>

#include <Window.hpp>

HINSTANCE Instance = nullptr;
int CmdShow = 0;

bool Initialize(HINSTANCE instance, int cmdShow) noexcept {
	Instance = instance;
	CmdShow = cmdShow;

	return RegisterWindow();
}

int Run() {
	MSG message;
	while (GetMessage(&message, nullptr, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return static_cast<int>(message.wParam);
}
