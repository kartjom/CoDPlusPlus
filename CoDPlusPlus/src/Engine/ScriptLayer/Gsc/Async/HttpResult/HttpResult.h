#pragma once
#include <Engine/ScriptLayer/Gsc/Async/Task/Task.h>

namespace CoDUO::Gsc::Async
{
	struct HttpResult : public Task
	{
		int StatusCode;
		std::string Body;

		HttpResult();

		void PushGscData() override;
	};
}