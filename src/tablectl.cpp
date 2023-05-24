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

#include "tablectl.hpp"
#include "utilities.hpp"

namespace App {
	namespace Controls {

		Table::Table(HWND _hWnd) : _hWnd(_hWnd) {}

		void Table::addColumns(const std::vector<TableColumn>& columns) noexcept
		{
			if (::IsWindow(_hWnd))
			{
				for (auto& column : columns)
				{
					::SendMessage(_hWnd, LVM_INSERTCOLUMN, getColumnCount(),
						(LPARAM)(const ListViewColumn*)&column);
				}
			}
		}

		void Table::addRow(const std::vector<std::string> items) noexcept
		{
			if (::IsWindow(_hWnd))
			{
				TableItem _titem;
				for (size_t i = 0; i < items.size(); i++)
				{
					::SendMessage(_hWnd, i == 0 ? LVM_INSERTITEM : LVM_SETITEM, 0,
						(LPARAM)(const ListViewItem*)&_titem
						.setText(::string_to_wstring(items[i]), static_cast<int>(i)));
				}
			}
		}

		void Table::deleteAllRows() noexcept
		{
			if (::IsWindow(_hWnd))
			{
				::SendMessage(_hWnd, LVM_DELETEALLITEMS, 0, 0);
			}
		}

		int Table::getColumnCount() const noexcept
		{
			return ::IsWindow(_hWnd) ? Header_GetItemCount((HWND)
				::SendMessage(_hWnd, LVM_GETHEADER, 0, 0)) : 0;
		}

		TableColumn::TableColumn(const TableColumn& column)
		{
			::memcpy(&_column, &column, sizeof(TableColumn));
			if ((_column.pszText = ::_wcsdup(column._column.pszText)) == nullptr)
				throw std::bad_alloc();
		}

		TableColumn::TableColumn(const wchar_t* name, int width)
		{
			_no_null(name);
			::memset(&_column, 0, sizeof(ListViewColumn));
			_column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
			_column.cx = width;
			if ((_column.pszText = ::_wcsdup(name)) == nullptr)
				throw std::bad_alloc();
		}

		TableColumn::~TableColumn()
		{
			if (_column.pszText != nullptr) ::free(_column.pszText);
		}

		TableItem::TableItem(const TableItem& item)
		{
			::memcpy(&_item, &item, sizeof(ListViewItem));
			if ((_item.pszText = ::_wcsdup(item._item.pszText)) == nullptr)
				throw std::bad_alloc();
		}

		TableItem::TableItem()
		{
			::memset(&_item, 0, sizeof(ListViewItem));
			_item.mask = LVIF_TEXT;
		}

		TableItem::~TableItem()
		{
			if (_item.pszText != nullptr) ::free(_item.pszText);
		}

		TableItem& TableItem::setText(const std::wstring& text, int subitem)
		{
			_item.iSubItem = subitem;
			if (_item.pszText != nullptr) ::free(_item.pszText);
			if ((_item.pszText = ::_wcsdup(text.c_str())) == nullptr)
				throw std::bad_alloc();
			return *this;
		}
	}
}