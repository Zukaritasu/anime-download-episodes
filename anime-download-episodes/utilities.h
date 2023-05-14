#pragma once

#include <string>

#ifdef _DEBUG
#define PRINT_RECT(rc) \
	printf("Rect (%d, %d, %d, %d)\n", rc.left, rc.top, rc.right, rc.bottom) 
#define PRINT_SIZE(sc) \
	printf("Size (%d, %d)\n", sc.cx, sc.cy) 
#else
#define PRINT_RECT(rc)
#define PRINT_SIZE(sc)
#endif // _DEBUG

#define _MSG_ERROR(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONERROR)
#define _MSG_INFO(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONINFORMATION)
#define _MSG_WARNING(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONWARNING)
#define _MSG_QUESTION(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONQUESTION)

std::wstring StringTrim(const std::wstring& s);
