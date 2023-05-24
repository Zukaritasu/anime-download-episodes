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

#include "curlconn.hpp"

#include <iostream>

namespace App {
	namespace Connection {

		CurlConnection::CurlConnection()
		{
			curl_global_init(CURL_GLOBAL_ALL);
			if ((curl = curl_easy_init()) == nullptr)
				throw std::exception("Error initializing curl session");
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeCallback);
		}

		CurlConnection::~CurlConnection()
		{
			if (curl != nullptr) curl_easy_cleanup(curl);
			curl_global_cleanup();
		}

		CurlConnection& CurlConnection::setURL(const std::string& url) noexcept
		{
			curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
			return *this;
		}

		CurlConnection& CurlConnection::followLocation(bool enable) noexcept
		{
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, enable);
			return *this;
		}

		bool CurlConnection::readData(std::basic_string<char>& byteArray)
		{
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, &byteArray);
			CURLcode res = curl_easy_perform(curl);
			if (res != CURLE_OK)
				std::cout << "HTTP request failed: " << curl_easy_strerror(res) << std::endl;
			return res == CURLE_OK;
		}

		int CurlConnection::getResponseCode() noexcept
		{
			int code = 0;
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
			return code;
		}

		size_t CurlConnection::writeCallback(char* ptr, size_t size, size_t nmemb,
			void* userdata)
		{
			static_cast<std::basic_string<char>*>(userdata)->append(ptr, size * nmemb);
			return size * nmemb;
		}
	}
}