#include "globalsetting.h"

GlobalSetting::GlobalSetting() {}

GlobalSetting&GlobalSetting::instance()
{
    static GlobalSetting setting;
    return setting;
}
