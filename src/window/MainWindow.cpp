#include <window/MainWindow.hpp>

#include <cstdlib>

LRESULT MainWindow::Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
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
	return Window::Callback(handle, message, wParam, lParam);
}

void MainWindow::Initialize() {
	AddChild(CreateButton(this, "Find ShitVM Process", 15, 15, 200, 50, CallbackLambda() {
		MessageBox(nullptr, "test", "test", MB_OK);
		return 0;
	})).SetFont(static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));
}