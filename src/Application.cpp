#include <Application.hpp>

#include <ShitVM.hpp>
#include <Window.hpp>

HINSTANCE Instance = nullptr;
int CmdShow = 0;

bool Initialize(HINSTANCE instance, int cmdShow) noexcept {
	Instance = instance;
	CmdShow = cmdShow;

	return RegisterWindow();
}

int Run(void(*other)()) {
	MSG message;
	while (true) {
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE)) {
			if (message.message == WM_QUIT) break;

			TranslateMessage(&message);
			DispatchMessage(&message);
		} else if (other) {
			other();
		}
	}

	return static_cast<int>(message.wParam);
}
