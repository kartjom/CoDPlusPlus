#pragma once
#include <string>

namespace String
{
    std::string TrimLeft(const std::string& str);
    std::string TrimRight(const std::string& str);
    std::string Trim(const std::string& str);

    std::string ToLower(std::string str);
    std::string ToUpper(std::string str);

    void TransformToLower(std::string& str);
    void TransformToUpper(std::string& str);
}