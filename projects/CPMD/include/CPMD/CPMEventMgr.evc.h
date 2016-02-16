#ifndef __CPMEVENTMGR_EVC_H__
#define __CPMEVENTMGR_EVC_H__

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMEventMgrEvents)
    STATIC_EVENT_DECL(ev_cpm_log_dir_env_var_not_defined)
    STATIC_EVENT_DECL(ev_modulename_reading_failure)
    STATIC_EVENT_DECL(ev_log_file_initialization_failure)
    STATIC_EVENT_DECL(ev_logging_failure)
    STATIC_EVENT_DECL(ev_expand_env_var_failure)
    STATIC_EVENT_DECL(ev_new_ace_message_block_failure)
    STATIC_EVENT_DECL(ev_signal_rejected)
    STATIC_EVENT_DECL(ev_new_ace_message_block_failure)
END_EVENT_NAMESPACE()


#endif // __CPMEVENTMGR_EVC_H__
