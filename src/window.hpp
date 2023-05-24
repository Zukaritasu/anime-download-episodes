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
#include "tablectl.hpp"
#include "statusctl.hpp"

#include <vector>

namespace App {
	namespace Controls {
		class Window
		{
		public:
			Window(const Window&) = delete;
			Window();

			bool create() noexcept;
			void addRow(const std::vector<std::string>& items);
			void notify();

		private:
			bool createHandle() noexcept;
			bool createTable();
			bool createToolBar();
			bool createStatusBar();
			bool registerClass() noexcept;
			bool layoutComponents() noexcept;
			void initComponents();
			void centerWindow();
			HIMAGELIST createToolBarImageList();

			static LRESULT CALLBACK wndProc(HWND _hWnd, UINT message, WPARAM wParam,
				LPARAM lParam);
			LRESULT onCommand(int cmd, WPARAM wParam, LPARAM lParam);
			LRESULT onCreate(LPCREATESTRUCT lParam);

			Table* _table = nullptr;

			HWND _handle = nullptr;
			HWND _toolbar = nullptr;
			StatusBar* _statusBar = nullptr;
			HIMAGELIST _timageList = nullptr;
		};
	}
}


