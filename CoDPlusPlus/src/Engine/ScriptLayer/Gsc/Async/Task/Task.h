#pragma once
#include <mutex>
#include <memory>
#include <unordered_map>

namespace CoDUO::Gsc::Async
{
	enum TaskStatus
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

	inline std::unordered_map<int, std::shared_ptr<struct Task>> PendingTasks;
	inline std::mutex TaskResultsMutex;

	struct Task : public std::enable_shared_from_this<Task>
	{
		std::atomic<TaskType> Type = TaskType::Unknown;
		std::atomic<TaskStatus> Status = TaskStatus::New;
		std::atomic<int> Handle;

		void Initialize();
		void Finish();
		void Dispose();

		virtual void PushGscData() = 0;
	};
}

namespace CoDUO::Gsc::Async
{
	int NewAwaiterHandle();
}