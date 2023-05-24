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

namespace App {
	namespace Dialogs {
		class Dialog
		{
		public:
			Dialog(unsigned id);

			void show(HWND parent = nullptr);
		private:
			static INT_PTR wndProc(HWND hWnd, UINT cmd, WPARAM wParam, LPARAM lParam);

		protected:
			HWND _hWnd = nullptr;
			unsigned _id = 0;

			HWND getControl(int id);
			virtual void onInit();
			virtual void onInitComponents();
			virtual void onSize();
			virtual bool onCommand(int cmd, unsigned __int64 lParam);
			virtual __int64 onColorStatic(HDC hDC, HWND hWnd);
		};
	}
}
