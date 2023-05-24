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
    namespace Connection {
        class CurlConnection
        {
        public:
            CurlConnection();
            ~CurlConnection();

            CurlConnection& setURL(const std::string& url) noexcept;
            CurlConnection& followLocation(bool enable) noexcept;
            bool readData(std::basic_string<char>& byteArray);
            int getResponseCode() noexcept;

        private:
            CURL* curl = nullptr;

            static size_t writeCallback(char* ptr, size_t size, size_t nmemb, void* userdata);
        };
    }
}
