/**
 * Copyright (C) 2023 Zukaritasu
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "dialog.hpp"

extern HINSTANCE hInst;

#define DIALOG_INSTANCE TEXT("INSTANCE")

App::Dialogs::Dialog::Dialog(unsigned id) : _id(id) {}

void App::Dialogs::Dialog::show(HWND parent) {
	DialogBoxParam(hInst, MAKEINTRESOURCE(_id), parent, wndProc, (LPARAM)this);
}

HWND App::Dialogs::Dialog::getControl(int id)
{
	return GetDlgItem(_hWnd, id);
}

void App::Dialogs::Dialog::onInit() {}

void App::Dialogs::Dialog::onInitComponents()
{

}

void App::Dialogs::Dialog::onSize() {}

bool App::Dialogs::Dialog::onCommand(int cmd, unsigned __int64 lParam) {
	return cmd == IDOK || cmd == IDCANCEL || cmd == IDCLOSE;
}

__int64 App::Dialogs::Dialog::onColorStatic(HDC hDC, HWND hWnd) {
	return 0;
}

INT_PTR App::Dialogs::Dialog::wndProc(HWND hWnd, UINT cmd, 
	WPARAM wParam, LPARAM lParam) {
	switch (cmd) {
		case WM_SIZE:
			static_cast<Dialog*>(GetProp(hWnd, DIALOG_INSTANCE))->onSize();
			break;
		case WM_INITDIALOG: {
			Dialog* dialog = reinterpret_cast<Dialog*>(lParam);
			SetProp(hWnd, DIALOG_INSTANCE, (HANDLE)dialog);
			dialog->_hWnd = hWnd;
			dialog->onInit();
			dialog->onInitComponents();
			return TRUE;
		}
		case WM_CTLCOLORSTATIC:
			return static_cast<Dialog*>(GetProp(hWnd, DIALOG_INSTANCE))
				->onColorStatic(HDC(wParam), HWND(lParam));
		case WM_COMMAND: {
			Dialog* dialog = static_cast<Dialog*>(GetProp(hWnd, DIALOG_INSTANCE));
			if (dialog->onCommand(LOWORD(wParam), lParam)) {
				::EndDialog(hWnd, TRUE);
				delete dialog;
			}
			return TRUE;
		}
	}
	return FALSE;
}
