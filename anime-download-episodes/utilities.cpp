#include "utilities.h"

std::wstring StringTrim(const std::wstring& s)
{
    auto start = s.begin();
    while (start != s.end() && isspace(*start))
    {
        start++;
    }

    auto end = s.end();
    do {
        end--;
    } while (std::distance(start, end) > 0 && isspace(*end));

    return std::wstring(start, end + 1);
}