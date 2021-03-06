#include <window/MainWindow.hpp>

#include <Application.hpp>
#include <ShitVM.hpp>
#include <window/ObjectViewer.hpp>

#include <cstdlib>

#ifdef _MSC_VER
#	pragma warning(disable: 4100)
#endif

LRESULT MainWindow::Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_GETMINMAXINFO: {
		LPMINMAXINFO min = reinterpret_cast<LPMINMAXINFO>(lParam);
		min->ptMinTrackSize.x = 510;
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
	AddChild(CreateButton(this, "Attach to ShitVM", 15, 40, 200, 50, CallbackLambda() {
		const int result = FindShitVMProcess();
		window->Invalidate();
		switch (result) {
		case 0:
			window->Children[1].Enable();
			MessageBox(window->Handle, "Successfully attached to the process", Title, MB_OK | MB_ICONINFORMATION);
			break;

		case 1:
			window->Children[1].Disable();
			MessageBox(window->Handle, "Failed to find the process", Title, MB_OK | MB_ICONERROR);
			break;

		case -1:
			window->Children[1].Disable();
			MessageBox(window->Handle, "Failed to attach to the process\n(Incompatible architecture)", Title, MB_OK | MB_ICONERROR);
			break;
		}
		return 0;
	}));

	AddChild(CreateButton(this, "Open Object Viewer", 15, 145, 200, 50, CallbackLambda() {
		static ObjectView viewer;
		viewer.Create("Object Viewer");
		viewer.Show(SW_SHOWNORMAL);
		return 0;
	})).Disable();
}
void MainWindow::Paint(HDC dc) {
	TextOut(dc, 15, 15, ShitVMProcessIdString.c_str(), static_cast<int>(ShitVMProcessIdString.size()));
	TextOut(dc, 15, 120, "Tools", 5);
}