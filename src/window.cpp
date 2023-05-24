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


#include "window.hpp"
#include "resource.h"
#include "utilities.hpp"
#include "addanimedlg.hpp"
#include "listanimedlg.hpp"
#include "deleteanimedlg.hpp"
#include "searchdlg.hpp"
#include "tablectl.hpp"
#include "sqlmngr.hpp"

#include <vector>
#include <string>

#define MAX_LOADSTRING 100

extern HINSTANCE hInst;

using namespace App::Dialogs;
using namespace App::Controls;

namespace
{
    const wchar_t* WINDOW_CLASS = L"Win32Application";
    const wchar_t* WINDOW_INSTANCE = L"WindowObject";
    wchar_t szTitle[MAX_LOADSTRING];
}


Window::Window() {}

bool Window::create() noexcept
{
    ::LoadStringW(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	return createHandle();
}

void Window::addRow(const std::vector<std::string>& items)
{
    _table->addRow(items);
}

void Window::notify()
{
    std::vector<EpisodeData> data = SQLite::getEpisodes();
    _table->deleteAllRows();
    for (const auto& episode : data)
    {
        _table->addRow({ episode.token, std::to_string(episode.number), "Mega", 
            episode.date, episode.link 
            }
        );
    }

    _statusBar->setParts({ 100, 200 });
    _statusBar->setTextPart(format(L"Epi. %I64u", data.size()), 0);
}

bool Window::createHandle() noexcept
{
    registerClass();
    _handle = ::CreateWindowW(WINDOW_CLASS, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 520, nullptr, nullptr, hInst, this);

    if (!_handle) return false;

    centerWindow();
    ::ShowWindow(_handle, SW_NORMAL);
    ::UpdateWindow(_handle);

	return true;
}

bool Window::createTable()
{
    _table = new Table(CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 
                            10, 10, 380, 280, _handle, (HMENU)0, hInst, NULL));
    _no_null(_table->_hWnd);
    if (::IsThemeActive())
        ::SetWindowTheme(_table->_hWnd, L"Explorer", nullptr);

    std::vector<TableColumn> columns;
    columns.push_back(TableColumn(L"Nombre del anime", 300));
    columns.push_back(TableColumn(L"Episodio", 80));
    columns.push_back(TableColumn(L"Servidor", 120));
    columns.push_back(TableColumn(L"Fecha", 80));
    columns.push_back(TableColumn(L"Link", 220));

    _table->addColumns(columns);

    return true;
}

bool Window::createToolBar()
{
    _toolbar = ::CreateToolbarEx(_handle, WS_VISIBLE | WS_CHILD | TBSTYLE_TOOLTIPS | CCS_NODIVIDER,
                                 1, 0, NULL, 0, NULL, 0, 0, 0, 0, 0, sizeof(TBBUTTON));
    _no_null(_toolbar);

    TBMETRICS metrics{};
    metrics.cbSize = sizeof TBMETRICS;
    metrics.dwMask = TBMF_PAD | TBMF_BARPAD | TBMF_BUTTONSPACING;

    metrics.cxButtonSpacing += 6;
    metrics.cyButtonSpacing += 10;
    metrics.cxPad += 8;
    metrics.cyPad += 8;
    metrics.cxBarPad += 4;
    metrics.cyBarPad += 4;

    ::SendMessage(_toolbar, TB_SETMETRICS, 0, (LPARAM)&metrics);

    TBBUTTON tbButtons[] =
    {
        {0, IDM_ADD_ANIME, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Agregar"},
        {1, IDM_DELETE_ANIME, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Eliminar"},
        {2, IDM_SEARCH_EPISODES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Buscar"},
        {3, IDM_DOWNLOAD, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Descargar"},
        {4, IDM_PREFERENCES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Opciones"}
    };

    ::SendMessage(_toolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)createToolBarImageList());
    ::SendMessage(_toolbar, TB_ADDBUTTONS, (WPARAM)ARRAYSIZE(tbButtons), (LPARAM)&tbButtons);

    return false;
}

bool Window::createStatusBar()
{
    HWND _hWnd = CreateWindow(STATUSCLASSNAME, nullptr, WS_VISIBLE | WS_CHILD, 
        0, 0, 0, 0, _handle, nullptr, hInst, nullptr);
    _no_null(_hWnd);
    _statusBar = new StatusBar(_hWnd);
    return true;
}

bool Window::registerClass() noexcept
{
    WNDCLASSEXW wcex;

    wcex.cbSize        = sizeof(WNDCLASSEX);

    wcex.style         = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc   = Window::wndProc;
    wcex.cbClsExtra    = 0;
    wcex.cbWndExtra    = 0;
    wcex.hInstance     = hInst;
    wcex.hIcon         = ::LoadIcon(nullptr, IDI_APPLICATION);
    wcex.hCursor       = ::LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_MENUBAR + 1);
    wcex.lpszMenuName  = MAKEINTRESOURCE(IDC_MENUBAR);
    wcex.lpszClassName = WINDOW_CLASS;
    wcex.hIconSm       = wcex.hIcon;
	return ::RegisterClassEx(&wcex) != 0;
}

bool Window::layoutComponents() noexcept
{
    RECT rc{}, rctb{}, rcstb{};

    ::MoveWindow(_toolbar, 0, 0, 0, 0, TRUE);
    ::MoveWindow(*_statusBar, 0, 0, 0, 0, TRUE);
    if (!::GetClientRect(_handle, &rc) || !::GetClientRect(_toolbar, &rctb)
        || !::GetClientRect(*_statusBar, &rcstb))
        return false;

    ::MoveWindow(_table->_hWnd, 0, rctb.bottom + rctb.top + 10, rc.right, 
        rc.bottom - (rctb.bottom + rctb.top + 10 + rcstb.bottom) ,TRUE);

    return true;
}

void Window::initComponents()
{
    createToolBar();
    createTable();
    createStatusBar();
    notify();
}

void Window::centerWindow()
{
    RECT rc{};
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    int width = GetSystemMetrics(SM_CXSCREEN);
    int height = GetSystemMetrics(SM_CYSCREEN);

    width -= (width - rc.right);
    height -= (height - rc.bottom);

    MoveWindow
    (
        _handle, ((width / 2) - (900 / 2)) + rc.left, 
        ((height / 2) - (520 / 2)) + rc.top, 
        900, 520, TRUE
    );
}

HIMAGELIST Window::createToolBarImageList()
{
    _timageList = ::ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 0);
    std::vector<const wchar_t*> images = 
    { 
        L"add_32x32.ico", L"trash_32x32.ico",
        L"bookmark_32x32.ico",  L"hard-disc-drive_32x32.ico", 
        L"settings_32x32.ico"
    };
    for (auto var : images)
    {
        HICON hIcon = (HICON)LoadImage(nullptr, 
            std::basic_string<wchar_t>(L"images/").append(var).c_str(), 
            IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        ::ImageList_AddIcon(_timageList, hIcon);
    }
    return _timageList;
}

LRESULT Window::wndProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        return reinterpret_cast<Window*>(::GetProp(_hWnd, WINDOW_INSTANCE))
            ->onCommand(LOWORD(wParam), wParam, lParam);
    case WM_CREATE:
    {
        LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        auto* window = static_cast<Window*>(cs->lpCreateParams);
        window->_handle = _hWnd;
        ::SetProp(_hWnd, WINDOW_INSTANCE, window);
        return window->onCreate(cs);
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    case WM_SIZE:
    {
        reinterpret_cast<Window*>(::GetProp(_hWnd, WINDOW_INSTANCE))
            ->layoutComponents();
        break;
    }
    default:
        return ::DefWindowProc(_hWnd, message, wParam, lParam);
    }
    return 0;
}

LRESULT Window::onCommand(int cmd, WPARAM wParam, LPARAM lParam)
{
    switch (cmd)
    {
    case IDM_ABOUT:
        break;
    case IDM_EXIT:
        ::SetProp(_handle, WINDOW_INSTANCE, NULL);
        ::DestroyWindow(_handle);
        break;
    case IDM_ADD_ANIME:
        App::Dialogs::showAddAnimeDialog(_handle);
        break;
    case IDM_DELETE_ANIME:
        show_delete_anime(_handle);
        break;
    case IDM_SEARCH_EPISODES:
        show_search_episodes(_handle);
        break;
    case IDM_LIST_ANIMES:
        show_anime_list(_handle);
        break;
    default:
        return ::DefWindowProc(_handle, WM_COMMAND, wParam, lParam);
    }
    return 0;
}

LRESULT Window::onCreate(LPCREATESTRUCT lParam)
{
    initComponents();
    return 0;
}
