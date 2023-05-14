#pragma once

#include "framework.h"

class AnimeManager
{
public:
	static void showAddAnimeDialog(HWND hWnd = nullptr);

private:
	AnimeManager() {}

	static INT_PTR CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
};
