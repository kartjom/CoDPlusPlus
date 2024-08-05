#include "Task.h"
#include <chrono>
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
			std::unique_lock<std::mutex> lock(TaskListMutex);
			AllocatedTasks.emplace(Handle, shared_from_this());
		}
	}

	void Task::Finish()
	{
		Status.store(TaskStatus::Finished);
		FinishedAt.store(time(0));

		std::println("[Task] - Async task {} finished", Handle.load());
	}

	void Task::Dispose()
	{
		{
			std::unique_lock<std::mutex> lock(TaskListMutex);
			AllocatedTasks.erase(Handle);
		}

		std::println("[Task] - Async task {} disposed", Handle.load());
	}
}

namespace CoDUO::Gsc::Async::TaskManager
{
	void InitializeGarbageCollector()
	{
		std::thread gc([] {
			while (true)
			{
				using namespace std::chrono_literals;
				std::this_thread::sleep_for(10s);

				int currentTime = time(0);

				std::unique_lock<std::mutex> lock(TaskListMutex);

				const auto count = std::erase_if(AllocatedTasks, [=](const auto& item)
				{
					auto const& [key, value] = item;
					return value->Status == TaskStatus::Finished && currentTime > value->FinishedAt + 10;
				});

				if (count > 0)
				{
					std::println("[Task] - Garbage collected {} items", count);
				}
			}
		});

		gc.detach();

		std::println("[Task] - Initialized garbage collector");
	}
}