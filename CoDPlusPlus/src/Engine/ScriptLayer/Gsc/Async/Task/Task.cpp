#include "Task.h"
#include <print>

namespace CoDUO::Gsc::Async
{
	void Task::Initialize()
	{
		static int nextAwaiterHandle = 0;
		if (nextAwaiterHandle < 0) nextAwaiterHandle = 0;

		Handle.store(nextAwaiterHandle++);
		Status.store(TaskStatus::InProgress);

		{
			std::unique_lock<std::mutex> lock(TaskResultsMutex);
			PendingTasks.emplace(Handle, shared_from_this());
		}
	}

	void Task::Finish()
	{
		Status.store(TaskStatus::Finished);

		std::println("[Task] - Async task {} finished", Handle.load());
	}

	void Task::Dispose()
	{
		{
			std::unique_lock<std::mutex> lock(TaskResultsMutex);
			PendingTasks.erase(Handle);
		}

		std::println("[Task] - Async task {} disposed", Handle.load());
	}
}