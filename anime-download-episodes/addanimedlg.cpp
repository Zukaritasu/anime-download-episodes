#include "addanimedlg.h"
#include "resource.h"
#include "utilities.h"
#include "sqlmngr.h"

extern HINSTANCE hInst;

static void SetLabelFontStyle(HWND hWnd);
static std::wstring GetTextBoxString(HWND hWnd);

void AnimeManager::showAddAnimeDialog(HWND hWnd)
{
	::DialogBox(hInst, MAKEINTRESOURCE(IDD_ADD_ANIME), hWnd, AnimeManager::wndProc);
}

INT_PTR AnimeManager::wndProc(HWND hWnd, UINT cmd, WPARAM wParam, LPARAM lParam)
{
	switch (cmd)
	{
	case WM_INITDIALOG:
	{
		::SetLabelFontStyle(::GetDlgItem(hWnd, IDC_DLG_SAMPLE));
		return TRUE;
	}
	case WM_CTLCOLORSTATIC:
		if (GetDlgCtrlID((HWND)lParam) == IDC_DLG_SAMPLE)
		{
			SetTextColor((HDC)wParam, RGB(80, 80, 80));
			SetBkColor((HDC)wParam, GetSysColor(COLOR_MENUBAR));
			return (INT_PTR)GetStockObject(NULL_BRUSH);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDOK:
		{
			auto result = GetTextBoxString(GetDlgItem(hWnd, IDC_DLG_TOKEN));
			if (result.empty())
			{
				MessageBox(hWnd, L"Aun no se ha ingresado un token"
					L" correcto. Presione aceptar para contonuar", nullptr, MB_OK | MB_ICONWARNING);
				break;
			}
			if (SQLite::containsToken(result.c_str()))
			{
				std::wstring msg = std::wstring(L"El token '")
					.append(result.c_str())
					.append(std::wstring(L"' ya existe en la base de datos."));
				MessageBox(hWnd, msg.c_str(), nullptr, MB_OK | MB_ICONWARNING);
			}
			else
			{
				result = GetTextBoxString(GetDlgItem(hWnd, IDC_DLG_EPISODE));
				int episode = std::stoi(result);
				SQLite::addAnime(result.c_str(), episode == 0 ? 1 : episode);
			}
			::EndDialog(hWnd, EXIT_SUCCESS);
			return TRUE;
		}
		case IDCANCEL:
			::EndDialog(hWnd, EXIT_SUCCESS);
			return TRUE;
		default:
			break;
		}
		break;
	default:
		break;
	}
	return FALSE;
}

void SetLabelFontStyle(HWND hWnd)
{
	HFONT hFont = (HFONT)SendMessage(hWnd, WM_GETFONT, 0, 0);
	if (hFont != nullptr)
	{
		LOGFONT lFont = { 0 };
		GetObject(hFont, sizeof lFont, &lFont);
		lFont.lfItalic = TRUE;
		SendMessage(hWnd, WM_SETFONT, (WPARAM)CreateFontIndirect(&lFont), MAKELPARAM(TRUE, 0));
	}
}

std::wstring GetTextBoxString(HWND hWnd)
{
	int length = GetWindowTextLength(hWnd);
	if (length > 0)
	{
		std::wstring buffer(length, '\0');
		GetWindowText(hWnd, const_cast<LPWSTR>(buffer.c_str()), length + 1);
		return StringTrim(buffer);
	}
	return std::wstring();
}
