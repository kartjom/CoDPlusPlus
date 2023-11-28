#pragma once
#include <string>

namespace Utils
{
	class FileLogger
	{
	public:
		static void BeginLog(std::string_view name);
		static void EnqueueLog(std::string& msg);

		static bool EnsureFileIsOpen();
		static void Exit(const char* reason = nullptr);

		static void ThreadWorker();
	};
}