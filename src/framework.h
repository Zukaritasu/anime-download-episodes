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

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN 

#include <windows.h>

#include <stdlib.h>
#include <string>
#include <stdarg.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <Shlwapi.h>
#include <ShlObj.h>
#include <gdiplus.h>
#include <CommCtrl.h>
#include <Uxtheme.h>
#include <curl/curl.h>
#include <sqlite3.h>
#include <stringapiset.h>
#include <assert.h>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")
#pragma comment(lib, "libcurl.lib")
#pragma comment(lib, "sqlite3.lib")
#pragma comment(lib, "shlwapi.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
