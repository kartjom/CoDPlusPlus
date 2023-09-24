#include <Utils/Network/HttpClient.h>
#include "GscExtensions.h"
#include <Engine/CoDUO.h>

using namespace CoDUO;
namespace CoDUO::Gsc
{
	void Scr_StringToCmd()
	{
		const char* str = Scr_GetString(0);

		if (str)
		{
			size_t strSize = strlen(str) + 1;

			char* cmd = (char*)alloca(strSize + 1);
			memcpy(cmd, str, strSize);
			cmd[strSize] = '\n';

			Cbuf_AddText(cmd);
		}
	}

	void Scr_GetWeaponInfo()
	{
		int index = Scr_GetInt(0);
		weapondef_t* weapon = BG_GetWeaponDef(index);

		if (weapon)
		{
			Scr_MakeArray();

			Scr_AddInt(weapon->number);
			Scr_AddArrayStringIndexed(G_NewString("number"));

			Scr_AddString(weapon->name);
			Scr_AddArrayStringIndexed(G_NewString("name"));

			Scr_AddInt(weapon->clipSize);
			Scr_AddArrayStringIndexed(G_NewString("clipSize"));

			Scr_AddInt(weapon->maxAmmo);
			Scr_AddArrayStringIndexed(G_NewString("maxAmmo"));
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_GetWeaponIndexByName()
	{
		const char* weaponName = Scr_GetString(0);
		if (weaponName)
		{
			int32_t weaponIndex = BG_GetWeaponIndexForName(weaponName);
			Scr_AddInt(weaponIndex);
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_String_ToUpper()
	{
		char* c_str = (char*)Scr_GetString(0);

		if (c_str)
		{
			std::string str(c_str);

			for (int i = 0; str[i]; i++)
			{
				str[i] = toupper(str[i]);
			}
			Scr_AddString(str.c_str());
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_String_ToLower()
	{
		char* c_str = (char*)Scr_GetString(0);

		if (c_str)
		{
			std::string str(c_str);

			for (int i = 0; str[i]; i++)
			{
				str[i] = tolower(str[i]);
			}
			Scr_AddString(str.c_str());
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_String_StartsWith()
	{
		const char* text = Scr_GetString(0);
		const char* substr = Scr_GetString(1);

		if (text && substr)
		{
			std::string str(text);

			Scr_AddBool(str.starts_with(substr));
		}
		else
		{
			Scr_AddBool(false);
		}
	}

	void Scr_String_EndsWith()
	{
		const char* text = Scr_GetString(0);
		const char* substr = Scr_GetString(1);

		if (text && substr)
		{
			std::string str(text);

			Scr_AddBool(str.ends_with(substr));
		}
		else
		{
			Scr_AddBool(false);
		}
	}

	void Scr_String_Contains()
	{
		const char* text = Scr_GetString(0);
		const char* substr = Scr_GetString(1);

		if (text && substr)
		{
			std::string str(text);

			Scr_AddBool(str.contains(substr));
		}
		else
		{
			Scr_AddBool(false);
		}
	}

	void Scr_String_Replace()
	{
		const char* c_str = Scr_GetString(0);
		const char* from = Scr_GetString(1);
		const char* to = Scr_GetString(2);

		if (c_str && from && to)
		{
			std::string str(c_str);

			size_t startPos = 0;
			while ((startPos = str.find(from, startPos)) != std::string::npos)
			{
				str.replace(startPos, strlen(from), to);
				startPos += strlen(to);
			}

			Scr_AddString(str.c_str());
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void Scr_String_Split()
	{
		const char* c_str = Scr_GetString(0);
		const char* delimiter = Scr_GetString(1);

		if (c_str && delimiter)
		{
			Scr_MakeArray();
			std::string_view str = c_str;

			size_t pos = 0;
			while ((pos = str.find(delimiter)) != std::string_view::npos)
			{
				std::string token(str.substr(0, pos));
				str.remove_prefix(pos + strlen(delimiter));

				Scr_AddString(token.c_str());
				Scr_AddArray();
			}

			std::string token(str.substr(0, pos));
			Scr_AddString(token.c_str());
			Scr_AddArray();
		}
		else
		{
			Scr_AddUndefined();
		}
	}

	void BackgroundHttpRequest(int actionIndex, httplib::Result (*request_fn)(std::string, std::string), std::string host, std::string endpoint)
	{
		auto res = request_fn(host, endpoint);
		
		if (CodeCallback.OnHttpResponse)
		{
			HttpResult model;
			if ((bool)res.error())
			{
				model = {
					.ActionIndex = actionIndex,
					.StatusCode = -1,
					.Body = httplib::to_string(res.error())
				};
			}
			else
			{
				model = {
					.ActionIndex = actionIndex,
					.StatusCode = res->status,
					.Body = res->body
				};
			}

			std::unique_lock<std::mutex> lock(HttpMutex);
			BackgroundHttpResults.push(model);
		}
	}

	void Scr_HttpGet()
	{
		int actionIndex = Scr_GetInt(0);
		const char* host = Scr_GetString(1);
		const char* endpoint = Scr_GetString(2);

		if (host && endpoint)
		{
			std::thread http_thread(BackgroundHttpRequest, actionIndex, HttpClient::Get, std::string(host), std::string(endpoint));
			http_thread.detach();
		}
	}
}