#include "Task.h"
#include <print>

namespace CoDUO::Gsc::Async
{
	void Task::Initialize()
	{
		Handle.store(NewAwaiterHandle());
		Status.store(TaskStatus::InProgress);

		{
			std::unique_lock<std::mutex> lock(TaskResultsMutex);
			PendingTasks.emplace(Handle, shared_from_this());
		}
	}

	void Task::Finish()
	{
		Status.store(TaskStatus::Finished);

		std::println("[Awaiter] - Task {} finished", Handle.load());
	}

	void Task::Dispose()
	{
		{
			std::unique_lock<std::mutex> lock(TaskResultsMutex);
			PendingTasks.erase(Handle);
		}

		std::println("[Awaiter] - Task {} disposed", Handle.load());
	}
}

namespace CoDUO::Gsc::Async
{
	int32_t NewAwaiterHandle()
	{
		static int32_t NextAwaiterHandle = 0;
		if (NextAwaiterHandle < 0) NextAwaiterHandle = 0;

		return NextAwaiterHandle++;
	}
}