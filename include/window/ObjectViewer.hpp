#pragma once

#include <Window.hpp>

#include <string>

#include <Windows.h>

class ObjectView final : public Window {
private:
	HBRUSH m_Brush = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));

	std::string m_ObjectType = "Type: (Not searched)";
	std::string m_ObjectValue;

public:
	ObjectView() noexcept = default;
	ObjectView(const ObjectView&) = delete;
	virtual ~ObjectView() override = default;

public:
	ObjectView& operator=(const ObjectView&) = delete;
	bool operator==(const ObjectView&) = delete;
	bool operator!=(const ObjectView&) = delete;

public:
	virtual LRESULT Callback(HWND handle, UINT message, WPARAM wParam, LPARAM lParam) override;

	virtual void Initialize() override;
	virtual void Paint(HDC dc) override;

private:
	static LRESULT CALLBACK AddressWndProc(HWND handle, UINT message, WPARAM wParam, LPARAM lParam);

	void SearchObject();
};