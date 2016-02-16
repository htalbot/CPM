#ifndef __CPMTRACEERASEROBJ_EVC_H__
#define __CPMTRACEERASEROBJ_EVC_H__

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMTraceEraserObjEvents)
    STATIC_EVENT_DECL(ev_env_var_load_failure)
    STATIC_EVENT_DECL(ev_libconfig_setting_field_not_found)
    STATIC_EVENT_DECL(ev_getting_dir_content_failure)
    STATIC_EVENT_DECL(ev_getting_file_status_failure)
END_EVENT_NAMESPACE()


#endif // __CPMTRACEERASEROBJ_EVC_H__
