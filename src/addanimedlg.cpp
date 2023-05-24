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

#include "addanimedlg.hpp"
#include "resource.h"
#include "utilities.hpp"
#include "sqlmngr.hpp"
#include "dialog.hpp"

#define INVALID_TOKEN    L"Aun no se ha ingresado un token correcto. Presione aceptar para continuar."
#define TOKEN_EXISTS     L"El token '%s' ya se encuentra registrado en la base de datos."
#define REGISTER_SUCCESS L"El anime se ha agregado correctamente en la base de datos"

extern HINSTANCE hInst;

static void SetLabelFontStyle(HWND _hWnd);

class AnimeDialog : public App::Dialogs::Dialog
{
public:
	AnimeDialog(unsigned int id);

	virtual void onInit() override;
	virtual __int64 onColorStatic(HDC hDC, HWND hWnd) override;
	virtual bool onCommand(int cmd, unsigned __int64 lParam) override;

	bool addAnime();
private:

};

AnimeDialog::AnimeDialog(unsigned int id) : Dialog(id) {}

void AnimeDialog::onInit()
{
	::SetLabelFontStyle(::GetDlgItem(_hWnd, IDC_DLG_SAMPLE));
}

__int64 AnimeDialog::onColorStatic(HDC hDC, HWND hWnd) 
{
	if (::GetDlgCtrlID(hWnd) == IDC_DLG_SAMPLE) {
		::SetTextColor(hDC, RGB(80, 80, 80));
		::SetBkColor(hDC, ::GetSysColor(COLOR_MENUBAR));
		return (INT_PTR)::GetStockObject(NULL_BRUSH);
	}
	return 0;
}

bool AnimeDialog::onCommand(int cmd, unsigned __int64 lParam) 
{
	switch (cmd)
	{
	case IDCANCEL: return true;
	case IDOK:     return addAnime();
	default:
		break;
	}
	return false;
}

bool AnimeDialog::addAnime()
{
	auto token = ::get_hwnd_string(::GetDlgItem(_hWnd, IDC_DLG_TOKEN));
	if (token.empty())
		::_MSG_WARNING(_hWnd, INVALID_TOKEN);
	else if (SQLite::containsToken(token.c_str()))
		::_MSG_INFO(_hWnd, ::format(TOKEN_EXISTS, token.c_str()).c_str());
	else {
		auto str_ep = ::get_hwnd_string(::GetDlgItem(_hWnd, IDC_DLG_EPISODE));
		int episode = str_ep.empty() ? 1 : std::stoi(str_ep);
		if (SQLite::addAnime(token.c_str(), episode == 0 ? 1 : episode))
			::_MSG_INFO(_hWnd, REGISTER_SUCCESS);
		return true;
	}
	return false;
}

void SetLabelFontStyle(HWND _hWnd)
{
	HFONT hFont = (HFONT)SendMessage(_hWnd, WM_GETFONT, 0, 0);
	if (hFont != nullptr) {
		LOGFONT lFont = { 0 };
		GetObject(hFont, sizeof lFont, &lFont);
		lFont.lfItalic = TRUE;
		SendMessage(_hWnd, WM_SETFONT, (WPARAM)CreateFontIndirect(&lFont), MAKELPARAM(TRUE, 0));
	}
}

void App::Dialogs::showAddAnimeDialog(HWND _hWnd) {
	(new AnimeDialog(IDD_ADD_ANIME))->show(_hWnd);
}
