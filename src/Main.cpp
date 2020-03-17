#include <Application.hpp>
#include <Window.hpp>

#include <cstdlib>

#include <Windows.h>

Window MainWindow;
LRESULT MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int cmdShow) {
	if (!Initialize(instance, cmdShow)) {
		MessageBox(nullptr, "Failed to initialize.", Title, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	if (!(MainWindow = MakeWindow(MainWindowProc))) {
		MessageBox(nullptr, "Failed to create the main window.", Title, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	MainWindow.Show(cmdShow);

	MSG message;
	while (GetMessage(&message, nullptr, 0, 0)) {
		TranslateMessage(&message);
		DispatchMessage(&message);
	}

	return static_cast<int>(message.wParam);
}

LRESULT MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO: {
		LPMINMAXINFO min = reinterpret_cast<LPMINMAXINFO>(lParam);
		min->ptMinTrackSize.x = 640;
		min->ptMinTrackSize.y = 480;
		return 0;
	}

	case WM_DESTROY:
		PostQuitMessage(EXIT_SUCCESS);
		return 0;
	}
	return DefWindowProc(handle, message, wParam, lParam);
}