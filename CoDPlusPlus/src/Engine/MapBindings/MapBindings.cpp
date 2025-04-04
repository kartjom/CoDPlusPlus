#include "MapBindings.h"
#include <Utils/String/String.h>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <print>

namespace MapBindings
{
    MapBindingEntry& GetBindings()
    {
        static MapBindingEntry MapBindingInstance{};
        return MapBindingInstance;
    }

	void ReloadMapBindings()
	{
        constexpr const char* map_bindings = "codplusplus/map_bindings.txt";

        MapBindingEntry& Instance = GetBindings();
        Instance = {};

        if (!std::filesystem::exists(map_bindings)) return; // no file, no work

        std::ifstream file(map_bindings, std::ifstream::in);
        if (!file.is_open()) return;

		std::string line;
        line.reserve(64);

        int line_number = 0;
		while (std::getline(file, line))
		{
            line_number++;

            if (line.empty()) continue;
            if (line.length() >= 2 && line[0] == '/' && line[1] == '/') continue; // comment

            String::TransformToLower(line);

            if (size_t pos = line.find('='); pos != std::string::npos)
            {
                if (std::count(line.begin(), line.end(), '=') > 1)
                {
                    std::println("[CoDPlusPlus] - Map Bindings: Entry '{}' contains more than one '='. Line {}", line, line_number);
                    continue;
                }

                std::string key = String::Trim( line.substr(0, pos) );
                std::string value = String::Trim( line.substr(pos + 1) );

                if (key.empty() || value.empty())
                {
                    std::println("[CoDPlusPlus] - Map Bindings: Entry '{}' contains empty key or value. Line {}", line, line_number);
                    continue;
                }

                if (key == "default")
                {
                    Instance.Default = value;
                }
                else
                {
                    Instance.Bindings[key] = value;
                }
            }
            else
            {
                std::println("[CoDPlusPlus] - Map Bindings: Invalid entry '{}'. Line {}", line, line_number);
                continue;
            }
		}
	}

	std::string GetBindingForMap(std::string mapName)
	{
        MapBindingEntry& Instance = GetBindings();

		String::TransformToLower(mapName);

        if (auto it = Instance.Bindings.find(mapName); it != Instance.Bindings.end())
        {
            return it->second; // map found
        }

        if (!Instance.Default.empty())
        {
            return Instance.Default; // fallback
        }

        return ""; // not found
	}
}