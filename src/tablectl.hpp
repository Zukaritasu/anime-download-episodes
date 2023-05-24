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

#include <vector>

namespace App {
	namespace Controls {

		typedef LVITEM ListViewItem;
		typedef LVCOLUMN ListViewColumn;

		class TableColumn
		{
		private:
			ListViewColumn _column;

			friend class Table;

		public:
			TableColumn(const TableColumn& column);
			TableColumn(const wchar_t* name, int width);
			~TableColumn();

			const ListViewColumn* operator &() const { return &_column; }
			const ListViewColumn* operator ->() const { return &_column; }
		};

		class TableItem
		{
		private:
			ListViewItem _item;

			friend class Table;

		public:
			TableItem(const TableItem& item);
			TableItem();
			~TableItem();

			TableItem& setText(const std::wstring& text, int subitem = 0);

			const ListViewItem* operator &() const { return &_item; }
			const ListViewItem* operator ->() const { return &_item; }
		};

		class Table
		{
		public:
			HWND _hWnd = nullptr;

			Table(HWND _hWnd = nullptr);

			void addColumns(const std::vector<TableColumn>& columns) noexcept;
			void addRow(const std::vector<std::string> items) noexcept;
			void deleteAllRows() noexcept;
			int getColumnCount() const noexcept;

			operator HWND() { return _hWnd; }
		};
	}
}

