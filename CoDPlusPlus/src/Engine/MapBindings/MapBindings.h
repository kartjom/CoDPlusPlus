#pragma once
#include <unordered_map>
#include <string>

namespace MapBindings
{
    struct MapBindingEntry
    {
        std::unordered_map<std::string, std::string> Bindings;
        std::string Default;
    };

    MapBindingEntry& GetBindings();

	void ReloadMapBindings();
	std::string GetBindingForMap(std::string mapName);
}

