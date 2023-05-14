#include "window.h"
#include "resource.h"
#include "utilities.h"
#include "addanimedlg.h"

#include <vector>
#include <string>

#define MAX_LOADSTRING 100

struct TableColumn
{
    const wchar_t* name;
    int width;
};

extern HINSTANCE hInst;

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

bool Window::createHandle() noexcept
{
    registerClass();
    _handle = ::CreateWindowW(WINDOW_CLASS, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 900, 520, nullptr, nullptr, hInst, this);

    if (!_handle) return false;

    if (::IsThemeActive())
    {
        ::SetWindowTheme(_handle, L"DarkMode_Explorer", nullptr);
    }

    ::ShowWindow(_handle, SW_NORMAL);
    ::UpdateWindow(_handle);

	return true;
}

bool Window::createTable() noexcept
{
    _table = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, L"", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_EDITLABELS, 
                            10, 10, 380, 280, _handle, (HMENU)0, hInst, NULL);

    std::vector<TableColumn> columns = 
    {
        { L"Nombre del anime", 300 },
        { L"Episodio",          80 },
        { L"Servidor",         120 },
        { L"Fecha",             80 },
        { L"Link",             220 }
    };

    LVCOLUMN lvColumn{};
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    for (size_t i = 0; i < columns.size(); i++)
    {
        lvColumn.cx = columns[i].width;
        lvColumn.pszText = (LPWSTR)columns[i].name;
        SendMessage(_table, LVM_INSERTCOLUMN, i, (LPARAM)&lvColumn);
    }
   
    return true;
}

bool Window::createToolBar() noexcept
{
    _toolbar = ::CreateToolbarEx(_handle, WS_VISIBLE | WS_CHILD | TBSTYLE_TOOLTIPS | CCS_NODIVIDER,
                                 1, 0, NULL, 0, NULL, 0, 0, 0, 0, 0, sizeof(TBBUTTON));

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
        {3, IDM_PREFERENCES, TBSTATE_ENABLED, TBSTYLE_BUTTON, {0}, 0, (INT_PTR)L"Opciones"}
    };

    ::SendMessage(_toolbar, TB_SETIMAGELIST, (WPARAM)0, (LPARAM)createToolBarImageList());
    ::SendMessage(_toolbar, TB_ADDBUTTONS, (WPARAM)ARRAYSIZE(tbButtons), (LPARAM)&tbButtons);

    return false;
}

bool Window::createStatusBar() noexcept
{
    _statusBar = CreateWindow(STATUSCLASSNAME, nullptr, WS_VISIBLE | WS_CHILD, 
        0, 0, 0, 0, _handle, nullptr, hInst, nullptr);

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
    ::MoveWindow(_statusBar, 0, 0, 0, 0, TRUE);
    if (!::GetClientRect(_handle, &rc) || !::GetClientRect(_toolbar, &rctb)
        || !::GetClientRect(_statusBar, &rcstb))
        return false;
 
    ::MoveWindow(_table, 0, rctb.bottom + rctb.top + 10, rc.right, 
        rc.bottom - (rctb.bottom + rctb.top + 10 + rcstb.bottom) ,TRUE);

    return true;
}

void Window::initComponents()
{
    createToolBar();
    createTable();
    createStatusBar();
}

HIMAGELIST Window::createToolBarImageList()
{
    _timageList = ::ImageList_Create(32, 32, ILC_COLOR32 | ILC_MASK, 2, 0);
    std::vector<const wchar_t*> images = 
    { 
        L"add_32x32.ico", L"trash_32x32.ico",
        L"bookmark_32x32.ico",  L"settings_32x32.ico"
    };
    for (auto var : images)
    {
        HICON hIcon = (HICON)LoadImage(nullptr, 
#ifdef _DEBUG
            std::basic_string<wchar_t>(L"../anime-download-episodes/images/")
#else
            std::basic_string<wchar_t>(L"images/")
#endif 
                .append(var).c_str(), 
            IMAGE_ICON, 32, 32, LR_LOADFROMFILE | LR_DEFAULTSIZE);
        ::ImageList_AddIcon(_timageList, hIcon);
    }
    return _timageList;
}

LRESULT Window::wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        return reinterpret_cast<Window*>(::GetProp(hWnd, WINDOW_INSTANCE))
            ->onCommand(LOWORD(wParam), wParam, lParam);
    case WM_CREATE:
    {
        LPCREATESTRUCT cs = reinterpret_cast<LPCREATESTRUCT>(lParam);
        auto* window = static_cast<Window*>(cs->lpCreateParams);
        window->_handle = hWnd;
        ::SetProp(hWnd, WINDOW_INSTANCE, window);
        return window->onCreate(cs);
    }
    case WM_DESTROY:
        ::PostQuitMessage(0);
        break;
    case WM_SIZE:
    {
        reinterpret_cast<Window*>(::GetProp(hWnd, WINDOW_INSTANCE))
            ->layoutComponents();
        break;
    }
    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
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
        AnimeManager::showAddAnimeDialog(_handle);
        break;
    case IDM_DELETE_ANIME:
        break;
    case IDM_SEARCH_EPISODES:
        break;
    default:
        return ::DefWindowProc(_handle, WM_COMMAND, wParam, lParam);
    }
    return 0;
}

LRESULT Window::onCreate(LPCREATESTRUCT lParam)
{
    //MessageBox(nullptr, L"Ok", nullptr, 0);
    initComponents();
    return 0;
}
