#pragma once

#include <Window.hpp>

#include <Windows.h>

class ObjectView final : public Window {
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
};