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
		Scr_AddArrayStringIndexed(SL_GetString("status", 1));

		Scr_AddString(this->Body.c_str());
		Scr_AddArrayStringIndexed(SL_GetString("body", 1));
	}
}