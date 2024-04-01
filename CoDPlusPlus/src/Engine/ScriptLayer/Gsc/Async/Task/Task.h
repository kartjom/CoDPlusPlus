#pragma once
#include <mutex>
#include <memory>
#include <unordered_map>

namespace CoDUO::Gsc::Async
{
	using TaskList = std::unordered_map<int, std::shared_ptr<struct Task>>;

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

	inline TaskList AllocatedTasks;
	inline std::mutex TaskListMutex;

	struct Task : public std::enable_shared_from_this<Task>
	{
		std::atomic<TaskType> Type = TaskType::Unknown;
		std::atomic<TaskStatus> Status = TaskStatus::New;

		std::atomic<int> Handle;
		std::atomic<int> FinishedAt;

		void Initialize();
		void Finish();
		void Dispose();

		virtual void PushGscData() = 0;
	};
}