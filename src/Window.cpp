#include <Window.hpp>

#include <Application.hpp>

Window::operator bool() const noexcept {
	return Handle != nullptr;
}

void Window::Show(int value) noexcept {
	ShowWindow(Handle, value);
}

LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	void* address = reinterpret_cast<void*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	if (address) return static_cast<LRESULT(*)(HWND, UINT, WPARAM, LPARAM)>(address)(handle, message, wParam, lParam);
	else return DefWindowProc(handle, message, wParam, lParam);
}
bool RegisterWindow() noexcept {
	WNDCLASSEX form;
	form.cbSize = sizeof(WNDCLASSEX);
	form.style = CS_HREDRAW | CS_VREDRAW;
	form.lpfnWndProc = &WndProc;
	form.cbClsExtra = 0;
	form.cbWndExtra = 0;
	form.hInstance = Instance;
	form.hIcon = nullptr;
	form.hCursor = LoadCursor(nullptr, IDC_ARROW);
	form.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	form.lpszMenuName = nullptr;
	form.lpszClassName = "window";
	form.hIconSm = nullptr;

	return RegisterClassEx(&form);
}
Window MakeWindow(LRESULT(*proc)(HWND, UINT, WPARAM, LPARAM)) {
	const HWND result = CreateWindow("window", Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, Instance, nullptr);
	SetWindowLongPtr(result, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(proc));
	return { result };
}