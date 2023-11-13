#include "gsc.h"
#include <Engine/CoDUO.h>

using namespace CoDUO;

const char* ScrVar::String()
{
    return SL_ConvertToString(StringIndex);
}
