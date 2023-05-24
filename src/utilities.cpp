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


#include "utilities.hpp"

#include <iostream>

#pragma warning(disable: 4996)

std::wstring string_trim(const std::wstring& s)
{
    auto start = s.begin();
    while (start != s.end() && isspace(*start))
        start++;

    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && isspace(*end));

    return std::wstring(start, end + 1);
}

std::string string_trim(const std::string& s)
{
    auto start = s.begin();
    while (start != s.end() && isspace(*start))
        start++;

    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && isspace(*end));

    return std::string(start, end + 1);
}

std::string wstring_to_string(const std::wstring& wstr)
{
    int length = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    char* buffer = new char[length];
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, buffer, length, NULL, NULL);
    std::string str(buffer);
    delete[] buffer;
    return str;
}

std::wstring string_to_wstring(const std::string& str)
{
    int length = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    wchar_t* buffer = new wchar_t[length];
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, buffer, length);
    std::wstring wstr(buffer);
    delete[] buffer;
    return wstr;
}

std::wstring get_last_error()
{
    DWORD error_code = GetLastError();
    if (error_code != ERROR_SUCCESS)
    {
        LPWSTR error_message = NULL;
        FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            NULL, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR)&error_message,
            0,
            NULL);
        if (error_message == nullptr)
#if defined(_DEBUG) && (APP_ALLOC_CONSOLE)
                std::cerr << "get_last_error(): FormatMessageW() function fail message allocate buffer." << std::endl;
#else
            _MSG_ERROR(nullptr, L"get_last_error(): FormatMessageW() function fail message allocate buffer.");
#endif // defined(_DEBUG) && (APP_ALLOC_CONSOLE)
        else 
        {
            std::wstring str(error_message);
            LocalFree(error_message);
            return str;
        }
    }
    return std::wstring();
}

std::wstring format(const wchar_t* _format, ...)
{
    va_list args;
    va_start(args, _format);
    std::wstring str;
    int c_count = vswprintf(nullptr, 0, _format, args);
    if (c_count > 0)
    {
        wchar_t* _str = new wchar_t[c_count + 1];
        vswprintf(_str, _format, args);
        str = _str;
        delete[] _str;
    }
    return str;
}

std::string format(const char* _format, ...)
{
    va_list args;
    va_start(args, _format);
    std::string str;
    int c_count = vsnprintf(nullptr, 0, _format, args);
    if (c_count > 0)
    {
        char* _str = new char[c_count + 1];
        vsnprintf(_str, c_count + 1, _format, args);
        str = _str;
        delete[] _str;
    }
    return str;
}

std::wstring get_hwnd_string(HWND _hWnd)
{
    int length = GetWindowTextLength(_hWnd);
    if (length > 0)
    {
        length++;
        wchar_t* buffer = new wchar_t[length];
        GetWindowText(_hWnd, buffer, length);
        std::wstring str(buffer);
        delete[] buffer;
        return string_trim(str);
    }
    return std::wstring();
}

bool equals_ignore_case(const std::wstring& a, const std::wstring& b)
{
    if (a.size() == 0 || a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++)
    {
        if (tolower(a[i]) != tolower(b[i]))
        {
            return false;
        }
    }
    return true;
}

bool equals_ignore_case(const std::string& a, const std::string& b)
{
    if (a.size() == 0 || a.size() != b.size())
        return false;
    for (size_t i = 0; i < a.size(); i++)
    {
        if (tolower(a[i]) != tolower(b[i]))
        {
            return false;
        }
    }
    return true;
}
