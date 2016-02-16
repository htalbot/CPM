#ifndef __CPMD_EVC_H__
#define __CPMD_EVC_H__

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMDEvents)
    STATIC_EVENT_DECL(ev_cpmd_port_env_var_not_defined)
    STATIC_EVENT_DECL(ev_cpmd_app_layer_failure)
    STATIC_EVENT_DECL(ev_cpmd_tao_layer_init_failure)
    STATIC_EVENT_DECL(ev_object_registration_with_ior_table_failure)
    STATIC_EVENT_DECL(ev_server_started)
    STATIC_EVENT_DECL(ev_env_var_not_defined)
    STATIC_EVENT_DECL(ev_event_mgr_initialization_failure)
    STATIC_EVENT_DECL(ev_server_stopped)
    STATIC_EVENT_DECL(ev_main_exception)
    STATIC_EVENT_DECL(ev_object_init_failure)
    STATIC_EVENT_DECL(ev_process_not_responding_not_checked_anymore)
    STATIC_EVENT_DECL(ev_new_getting_reference_timeout)
    STATIC_EVENT_DECL(ev_field_not_found)
    STATIC_EVENT_DECL(ev_exception_with_settings_file)
    STATIC_EVENT_DECL(ev_signal_rejected)
    STATIC_EVENT_DECL(ev_new_ace_message_block_failure)
    STATIC_EVENT_DECL(ev_task_activation_failure)
    STATIC_EVENT_DECL(ev_getting_object_location_failure)
    STATIC_EVENT_DECL(ev_showing_exception)
END_EVENT_NAMESPACE()


#endif // __CPMD_EVC_H__
