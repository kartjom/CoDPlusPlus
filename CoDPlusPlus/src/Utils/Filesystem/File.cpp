#include "File.h"
#include <fstream>
#include <sstream>

std::string File::ReadText(std::string path)
{
    std::ifstream file(path);
    
    std::string str;
    if (file)
    {
        std::stringstream ss;
        ss << file.rdbuf();
        str = ss.str();
    }

    return str;
}
