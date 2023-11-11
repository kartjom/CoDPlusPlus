#include "HttpResult.h"
#include <Engine/CoDUO.h>

namespace CoDUO::Gsc::Async
{
	HttpResult::HttpResult()
	{
		Type.store(TaskType::Http);
	}

	void HttpResult::PushGscData()
	{
		Scr_AddInt(this->StatusCode);
		Scr_AddArrayStringIndexed(G_NewString("status"));

		Scr_AddString(this->Body.c_str());
		Scr_AddArrayStringIndexed(G_NewString("body"));
	}
}