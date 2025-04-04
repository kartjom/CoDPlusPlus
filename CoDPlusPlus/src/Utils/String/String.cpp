#include "String.h"
#include <algorithm>

namespace String
{
    std::string String::TrimLeft(const std::string& str)
    {
        auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
        return std::string(start, str.end());
    }
    std::string String::TrimRight(const std::string& str)
    {
        auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
        return std::string(str.begin(), end);
    }
    std::string String::Trim(const std::string& str)
    {
        auto start = std::find_if_not(str.begin(), str.end(), ::isspace);
        auto end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
        return (start < end ? std::string(start, end) : std::string());
    }

    std::string ToLower(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
        return str;
    }
    std::string ToUpper(std::string str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
        return str;
    }

    void TransformToLower(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::tolower);
    }
    void TransformToUpper(std::string& str)
    {
        std::transform(str.begin(), str.end(), str.begin(), ::toupper);
    }
}
