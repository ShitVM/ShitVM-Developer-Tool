#include <Child.hpp>

#include <Application.hpp>
#include <Window.hpp>

#include <stdexcept>

Child& Child::Enable() {
	EnableWindow(Handle, true);
	return *this;
}
Child& Child::Disable() {
	EnableWindow(Handle, false);
	return *this;
}

Child& Child::SetFont(HFONT font) {
	SendMessage(Handle, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
	return *this;
}

Child CreateButton(const Window* window, const char* text, int x, int y, int w, int h, const Callback& callback) {
	const HWND handle = CreateWindow("button", text, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, x, y, w, h, window->Handle, nullptr, Instance, nullptr);
	if (!handle) throw std::runtime_error("Failed to create the child");
	return { handle, callback };
}