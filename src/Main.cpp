#include <cstdlib>

#include <Windows.h>

constexpr const char* Title = "ShitVM Developer Tool";
HINSTANCE Instance;
int CmdShow;

HWND MainWindow;
LRESULT MainWindowProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

bool Initialize(HINSTANCE instance, int cmdShow) noexcept;
LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);
HWND MakeWindow(LRESULT(*proc)(HWND, UINT, WPARAM, LPARAM));

int APIENTRY WinMain(HINSTANCE instance, HINSTANCE, LPSTR, int cmdShow) {
	if (!Initialize(instance, cmdShow)) {
		MessageBox(nullptr, "Failed to initialize.", Title, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	if (!(MainWindow = MakeWindow(MainWindowProc))) {
		MessageBox(nullptr, "Failed to create the main window.", Title, MB_OK | MB_ICONERROR);
		return EXIT_FAILURE;
	}
	ShowWindow(MainWindow, cmdShow);

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

bool Initialize(HINSTANCE instance, int cmdShow) noexcept {
	Instance = instance;
	CmdShow = cmdShow;

	WNDCLASSEX form;
	form.cbSize = sizeof(WNDCLASSEX);
	form.style = CS_HREDRAW | CS_VREDRAW;
	form.lpfnWndProc = &WndProc;
	form.cbClsExtra = 0;
	form.cbWndExtra = 0;
	form.hInstance = instance;
	form.hIcon = nullptr;
	form.hCursor = LoadCursor(nullptr, IDC_ARROW);
	form.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	form.lpszMenuName = nullptr;
	form.lpszClassName = "form";
	form.hIconSm = nullptr;

	if (!RegisterClassEx(&form)) return false;

	return true;
}
LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	void* address = reinterpret_cast<void*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	if (address) return static_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(address)(handle, message, wParam, lParam);
	else return DefWindowProc(handle, message, wParam, lParam);
}
HWND MakeWindow(LRESULT(*proc)(HWND, UINT, WPARAM, LPARAM)) {
	const HWND result = CreateWindow("form", Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, Instance, nullptr);
	SetWindowLongPtr(result, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(proc));
	return result;
}