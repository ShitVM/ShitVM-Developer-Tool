#include <Child.hpp>

#include <Application.hpp>
#include <Window.hpp>

#include <stdexcept>

Child& Child::Enable() noexcept {
	EnableWindow(Handle, true);
	return *this;
}
Child& Child::Disable() noexcept {
	EnableWindow(Handle, false);
	return *this;
}

Child& Child::SetFont(HFONT font) noexcept {
	SendMessage(Handle, WM_SETFONT, reinterpret_cast<WPARAM>(font), true);
	return *this;
}
Child& Child::SetWndProc(WNDPROC newProc) noexcept {
	SetWindowLongPtr(Handle, GWLP_USERDATA, SetWindowLongPtr(Handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(newProc)));
	return *this;
}

namespace {
	Child CreateChild(const Window* window, const char* className, const char* text, DWORD style, int x, int y, int w, int h, const Callback& callback) {
		const HWND handle = CreateWindow(className, text, WS_CHILD | WS_VISIBLE | style, x, y, w, h, window->Handle, nullptr, Instance, nullptr);
		if (!handle) throw std::runtime_error("Failed to create the child");
		return { handle, callback };
	}
}

Child CreateButton(const Window* window, const char* text, int x, int y, int w, int h, const Callback& callback) {
	return CreateChild(window, "button", text, BS_PUSHBUTTON, x, y, w, h, callback);
}
Child CreateTextBox(const Window* window, int x, int y, int w, int h, const Callback& callback) {
	return CreateChild(window, "edit", nullptr, WS_BORDER, x, y, w, h, callback);
}
Child CreateCheckBox(const Window* window, const char* text, int x, int y, int w, int h, const Callback& callback) {
	return CreateChild(window, "button", text, BS_AUTOCHECKBOX, x, y, w, h, callback);
}