#pragma once
#include <print>
#include <mutex>
#include <memory>
#include <unordered_map>

namespace CoDUO::Gsc::Async
{
	enum AwaiterStatus
	{
		New = 0,
		InProgress,
		Finished,
	};

	enum TaskType
	{
		Unknown = 0,
		Http,
	};

	inline int32_t NewAwaiterHandle()
	{
		static int32_t NextAwaiterHandle = 0;
		if (NextAwaiterHandle < 0) NextAwaiterHandle = 0;

		return NextAwaiterHandle++;
	}

	inline std::unordered_map<int, std::shared_ptr<struct Awaitable>> PendingTasks;
	inline std::mutex TaskResultsMutex;

	struct Awaitable : public std::enable_shared_from_this<Awaitable>
	{
		std::atomic<TaskType> Type = TaskType::Unknown;
		std::atomic<AwaiterStatus> AwaitStatus = AwaiterStatus::New;
		std::atomic<int32_t> Handle;

		void Initialize()
		{
			Handle.store( NewAwaiterHandle() );
			AwaitStatus.store(AwaiterStatus::InProgress);

			{
				std::unique_lock<std::mutex> lock(TaskResultsMutex);
				PendingTasks.emplace(Handle, shared_from_this());
			}
		}

		void Finish()
		{
			AwaitStatus.store(AwaiterStatus::Finished);

			std::println("[Awaiter] - Task {} finished", Handle.load());
		}

		void Dispose()
		{
			{
				std::unique_lock<std::mutex> lock(TaskResultsMutex);
				PendingTasks.erase(Handle);
			}

			std::println("[Awaiter] - Task {} disposed", Handle.load());
		}

		virtual void PushGscData() = 0;
	};
}