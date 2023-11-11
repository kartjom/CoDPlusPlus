#pragma once
#include <Engine/ScriptLayer/Gsc/Async/Awaiter.h>

namespace CoDUO::Gsc::Async
{
	struct HttpResult : public Awaitable
	{
		int StatusCode;
		std::string Body;

		HttpResult();

		void PushGscData() override;
	};
}