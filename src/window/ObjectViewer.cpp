#include <window/ObjectViewer.hpp>

LRESULT ObjectView::Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) {
	return Window::Callback(handle, message, wParam, lParam);
}

void ObjectView::Initialize() {}
void ObjectView::Paint(HDC dc) {}