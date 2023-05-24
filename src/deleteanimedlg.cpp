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

#include "deleteanimedlg.hpp"
#include "resource.h"
#include "sqlmngr.hpp"
#include "utilities.hpp"

extern HINSTANCE hInst;

static INT_PTR wndProc(HWND _hWnd, UINT cmd, WPARAM wParam, LPARAM lParam);
static void    combobox_add_items(HWND _hWnd);
static bool    delete_anime(HWND _hWnd);

namespace App {
	namespace Dialogs {
		void show_delete_anime(HWND _hWnd) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_DELETE_ANIME), _hWnd, wndProc);
		}
	}
}

static INT_PTR wndProc(HWND _hWnd, UINT cmd, WPARAM wParam, LPARAM lParam)
{
	switch (cmd) {
		case WM_INITDIALOG: {
			combobox_add_items(GetDlgItem(_hWnd, IDC_DLG_CBOX_ANIMES));
			SetFocus(GetDlgItem(_hWnd, IDC_DLG_TOKEN));
			break;
		}
		case WM_COMMAND:
			switch (LOWORD(wParam)) {
				case IDOK: 
					if (delete_anime(GetDlgItem(_hWnd, IDC_DLG_TOKEN)))
						EndDialog(_hWnd, TRUE);
					break;
				case IDCANCEL:
					EndDialog(_hWnd, TRUE);
					break;
			}
			break;
	}
	return FALSE;
}

void combobox_add_items(HWND _hWnd)
{
	std::vector<AnimeData> data = SQLite::getAnimeList();
	if (data.empty())
		EnableWindow(_hWnd, FALSE);
	else {
		for (const auto& item : data) {
			SendMessage(_hWnd, CB_ADDSTRING, 0, (LPARAM)string_to_wstring(item.name).c_str());
		}
	}
}

bool delete_anime(HWND _hWnd)
{
	std::wstring text = get_hwnd_string(_hWnd);
	if (text.empty())
		_MSG_WARNING(_hWnd, L"Aun no se ha ingresado un token valido.");
	else if (!SQLite::containsToken(text.c_str()))
		_MSG_WARNING(_hWnd, L"El token ingresado o seleccionado no existe.");
	else if (SQLite::deleteToken(text.c_str())) {
		_MSG_INFO(_hWnd, L"El anime fue eliminado satisfactoriamente.");
		return true;
	}
	return false;
}
