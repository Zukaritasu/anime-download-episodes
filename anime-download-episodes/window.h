#pragma once

#include "framework.h"

class Window
{
public:
	Window(const Window&) = delete;
	Window();

	bool create() noexcept;

private:
	bool createHandle() noexcept;
	bool createTable() noexcept;
	bool createToolBar() noexcept;
	bool createStatusBar() noexcept;
	bool registerClass() noexcept;
	bool layoutComponents() noexcept;
	void initComponents();
	HIMAGELIST createToolBarImageList();
	
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT onCommand(int cmd, WPARAM wParam, LPARAM lParam);
	LRESULT onCreate(LPCREATESTRUCT lParam);

	HWND _handle = nullptr;
	HWND _toolbar = nullptr;
	HWND _table = nullptr;
	HWND _statusBar = nullptr;
	HIMAGELIST _timageList = nullptr;
};

