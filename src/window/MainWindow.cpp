#include <window/MainWindow.hpp>

#include <Application.hpp>
#include <ShitVM.hpp>

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
		const bool result = FindShitVMProcess();
		if (result) {
			MessageBox(nullptr, "Succeed to find the ShitVM process", Title, MB_OK | MB_ICONINFORMATION);
		} else {
			MessageBox(nullptr, "Failed to find the ShitVM process", Title, MB_OK | MB_ICONERROR);
		}
		return 0;
	})).SetFont(static_cast<HFONT>(GetStockObject(DEFAULT_GUI_FONT)));
}