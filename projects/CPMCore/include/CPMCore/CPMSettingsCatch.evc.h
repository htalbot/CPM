#pragma once

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMSettingsCatchEvents)
    STATIC_EVENT_DECL(ev_settings_failure)
    STATIC_EVENT_DECL(ev_libconfig_setting_field_not_found)
END_EVENT_NAMESPACE()

