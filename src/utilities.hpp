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

#pragma once

#include "framework.h"

#ifdef _DEBUG
	#define PRINT_RECT(rc) \
		printf("Rect (%d, %d, %d, %d)\n", rc.left, rc.top, rc.right, rc.bottom) 
	#define PRINT_SIZE(sc) \
		printf("Size (%d, %d)\n", sc.cx, sc.cy) 
	#define PRINT_POINT(pt) \
		printf("Point (%d, %d)\n", pt.x, pt.y) 
#else
	#define PRINT_RECT(rc)
	#define PRINT_SIZE(sc)
#endif // _DEBUG

#define _no_null(variable) \
	if (variable == nullptr) throw std::exception(__FUNCTION__ " variable '" #variable "' is null.")

#define _MSG_ERROR(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONERROR)
#define _MSG_INFO(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONINFORMATION)
#define _MSG_WARNING(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONWARNING)
#define _MSG_QUESTION(hwnd, msg) \
	MessageBox(hwnd, msg, nullptr, MB_OK | MB_ICONQUESTION)
#define _AMSG_ERROR(hwnd, msg) \
	MessageBoxA(hwnd, msg, nullptr, MB_OK | MB_ICONERROR)
#define _AMSG_INFO(hwnd, msg) \
	MessageBoxA(hwnd, msg, nullptr, MB_OK | MB_ICONINFORMATION)
#define _AMSG_WARNING(hwnd, msg) \
	MessageBoxA(hwnd, msg, nullptr, MB_OK | MB_ICONWARNING)
#define _AMSG_QUESTION(hwnd, msg) \
	MessageBoxA(hwnd, msg, nullptr, MB_OK | MB_ICONQUESTION)

#define API_URL "https://jimov-api.vercel.app/"
#define PROVIDER "anime/tioanime/"

std::wstring string_trim(const std::wstring& s);
std::string  string_trim(const std::string& s);
std::string  wstring_to_string(const std::wstring& wstr);
std::wstring string_to_wstring(const std::string& str);
std::wstring get_last_error();
std::wstring format(const wchar_t* _format, ...);
std::string  format(const char* _format, ...);
std::wstring get_hwnd_string(HWND _hWnd);
bool         equals_ignore_case(const std::wstring & a, const std::wstring & b);
bool         equals_ignore_case(const std::string & a, const std::string & b);

#define TO_C_WSTR(str) string_to_wstring(str).c_str()
#define TO_C_STR(str) wstring_to_string(str).c_str()
