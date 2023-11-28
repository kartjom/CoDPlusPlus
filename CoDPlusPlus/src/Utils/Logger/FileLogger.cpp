#include "FileLogger.h"
#include <Utils/Classes/AtomicQueue.h>
#include <iostream>
#include <fstream>
#include <atomic>
#include <thread>

namespace Utils
{
	static std::ofstream _outfile;
	static std::string _fileName;

	static std::thread _loggerThread;
	static AtomicQueue<std::string> _queue;
	static std::atomic<bool> _threadExit = false;

	void FileLogger::BeginLog(std::string_view name)
	{
		_fileName = name;
		FileLogger::EnsureFileIsOpen();

		_loggerThread = std::thread(ThreadWorker);
		_loggerThread.detach();
	}

	void FileLogger::EnqueueLog(std::string& msg)
	{
		if (!_threadExit)
		{
			_queue.push(msg);
		}
	}

	bool FileLogger::EnsureFileIsOpen()
	{
		if (!_outfile.is_open())
		{
			_outfile.open(_fileName, std::ios::app);
		}

		return _outfile.is_open();
	}

	void FileLogger::Exit(const char* reason)
	{
		if (_outfile.is_open())
		{
			_outfile.close();
		}

		if (reason)
		{
			printf("[ERROR] - %s", reason);
		}
	}

	void FileLogger::ThreadWorker()
	{
		while (true)
		{
			_queue.wait_for_value();

			std::string log;
			if (_queue.pop(log))
			{
				retry:
				if (FileLogger::EnsureFileIsOpen())
				{
					_outfile << log << std::endl;
				}
				else
				{
					printf("[Logger] - Trying to reopen the file...\n");
					std::this_thread::sleep_for(std::chrono::seconds(5));
					goto retry;
				}
			}
		}

		_threadExit = true;
	}
}