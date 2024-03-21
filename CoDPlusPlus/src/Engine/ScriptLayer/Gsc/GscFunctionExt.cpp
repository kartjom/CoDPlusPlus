#include <Engine/ScriptLayer/Gsc/Async/HttpResult/HttpResult.h>
#include <Utils/Network/HttpClient.h>
#include <Engine/CoDUO.h>
#include "GscExtensions.h"

using namespace CoDUO;
using namespace CoDUO::Gsc::Async;
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

	void Scr_AddCommand()
	{
		const char* name = Scr_GetString(0);
		int fn = Scr_GetFunction(1);

		if (!name || !fn) return;
		if (Cmd_HasCommand(name)) return;

		gsc_commands[name] = fn;
	}

	void Scr_GetSystemTime()
	{
		std::time_t currentTime = time(0);
		int argc = Scr_GetNumParam();

		if (argc > 0)
		{
			const char* format = Scr_GetString(0);

			if (format)
			{
				std::tm tm = *std::localtime(&currentTime);

				std::ostringstream ss;
				ss << std::put_time(&tm, format);

				std::string s1 = ss.str();
				const char* s2 = s1.c_str();
				Scr_AddString(s2);
			}
		}
		else
		{
			Scr_AddInt(currentTime);
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

	void Scr_Await()
	{
		int handle = Scr_GetInt(0);

		Scr_MakeArray();

		if (handle < 0) // Invalid handle
		{
			Scr_AddBool(false);
			Scr_AddArrayStringIndexed(G_NewString("pending"));
			return;
		}

		std::shared_ptr<Awaitable> result;
		{
			std::unique_lock<std::mutex> lock(TaskResultsMutex);

			auto it = PendingTasks.find(handle);
			if (it == PendingTasks.end()) // Resource doesn't exist
			{
				Scr_AddBool(false);
				Scr_AddArrayStringIndexed(G_NewString("pending"));
				return;
			}

			result = (*it).second;
		}

		if (result->AwaitStatus.load() == AwaiterStatus::InProgress)
		{
			Scr_AddBool(true);
			Scr_AddArrayStringIndexed(G_NewString("pending"));
			return;
		}

		if (result->AwaitStatus.load() == AwaiterStatus::Finished)
		{
			Scr_AddBool(false);
			Scr_AddArrayStringIndexed(G_NewString("pending"));

			result->PushGscData();
			return;
		}
	}

	void Scr_HttpGet()
	{
		const char* host = Scr_GetString(0);
		const char* endpoint = Scr_GetString(1);

		if (host && endpoint)
		{
			std::shared_ptr<HttpResult> httpResult = std::make_shared<HttpResult>();

			try
			{
				httpResult->Initialize();

				std::thread http_task([=]()
				{
					try
					{
						auto res = HttpClient::Get(std::string(host), std::string(endpoint));

						if ((bool)res.error())
						{
							httpResult->StatusCode = -1;
							httpResult->Body = httplib::to_string(res.error());
						}
						else
						{
							httpResult->StatusCode = res->status;
							httpResult->Body = res->body;
						}

						httpResult->Finish();

						std::this_thread::sleep_for(std::chrono::seconds(3)); // Wait to dispose
					}
					catch (std::exception& ex)
					{
						printf("[ERROR] - Scr_HttpGet.http_task: %s\n", ex.what());
					}

					httpResult->Dispose();
				});

				http_task.detach();

				Scr_AddInt(httpResult->Handle); // awaiter handle
			}
			catch (std::exception& ex)
			{
				httpResult->Dispose();
				Scr_AddInt(-1);

				printf("[ERROR] - Scr_HttpGet: %s\n", ex.what());
			}
		}
		else
		{
			Scr_AddInt(-1);
		}
	}
}