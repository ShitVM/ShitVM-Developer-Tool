#include <Window.hpp>

#include <Application.hpp>

#include <stdexcept>
#include <utility>

Window::Window(Window&& window) noexcept
	: Handle(window.Handle), Children(std::move(window.Children)) {
	window.Handle = nullptr;

	if (Handle) {
		SetWindowLongPtr(Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}
}
Window::~Window() {
	if (Handle) {
		DestroyWindow(Handle);
	}
}

Window& Window::operator=(Window&& window) noexcept {
	Handle = window.Handle;
	Children = std::move(window.Children);

	window.Handle = nullptr;

	if (Handle) {
		SetWindowLongPtr(Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
	}

	return *this;
}
Window::operator bool() const noexcept {
	return Handle != nullptr;
}

LRESULT Window::Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
	case WM_COMMAND: return Children[LOWORD(wParam)].Callback(this, message, wParam, lParam);
	}

	return DefWindowProc(handle, message, wParam, lParam);
}

void Window::Create() {
	if (!Handle) {
		Handle = CreateWindow("window", Title, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, nullptr, nullptr, Instance, nullptr);
		if (!Handle) throw std::runtime_error("Failed to create the window");
		SetWindowLongPtr(Handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

		Initialize();
	}
}
void Window::Show(int value) {
	Create();
	ShowWindow(Handle, value);
}

Child& Window::AddChild(const Child& child) {
	Child& result = Children.emplace_back(child);
	SetParent(child.Handle, Handle);
	SetMenu(child.Handle, reinterpret_cast<HMENU>(Children.size() - 1));
	return result;
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
LRESULT CALLBACK WndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	Window* const window = reinterpret_cast<Window*>(GetWindowLongPtr(handle, GWLP_USERDATA));
	if (window) return window->Callback(handle, message, wParam, lParam);
	else return DefWindowProc(handle, message, wParam, lParam);
}