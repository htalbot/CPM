#ifndef __CPMM_EVC_H__
#define __CPMM_EVC_H__

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMMEvents)
    STATIC_EVENT_DECL(ev_cpmm_port_env_var_not_defined)
    STATIC_EVENT_DECL(ev_cpmm_app_layer_failure)
    STATIC_EVENT_DECL(ev_cpmm_app_layer_exception)
    STATIC_EVENT_DECL(ev_cpmm_tao_layer_init_failure)
    STATIC_EVENT_DECL(ev_object_registration_with_ior_table_failure)
    STATIC_EVENT_DECL(ev_cpmm_object_init_failure)
    STATIC_EVENT_DECL(ev_server_started)
    STATIC_EVENT_DECL(ev_getting_default_cpmm_settings_file_failure)
    STATIC_EVENT_DECL(ev_getting_default_cpmm_settings_failure)
    STATIC_EVENT_DECL(ev_cpmd_port_env_var_not_defined)
    STATIC_EVENT_DECL(ev_saving_cpmm_settings_failure)
    STATIC_EVENT_DECL(ev_bad_file_name_format)
    STATIC_EVENT_DECL(ev_ping_failure)
    STATIC_EVENT_DECL(ev_getting_cpmd_object_ref_failure)
    STATIC_EVENT_DECL(ev_cpmd_start_stop_failure)
    STATIC_EVENT_DECL(ev_cpmd_start_stop_exception)
    STATIC_EVENT_DECL(ev_getting_cpmd_object_ref_failure2)
    STATIC_EVENT_DECL(ev_getting_launched_processes_failure)
    STATIC_EVENT_DECL(ev_server_stopped)
    STATIC_EVENT_DECL(ev_main_exception)
    STATIC_EVENT_DECL(ev_singleton_close_exception)
    STATIC_EVENT_DECL(ev_bad_arg)
    STATIC_EVENT_DECL(ev_node_defined_multiple_times)
    STATIC_EVENT_DECL(ev_task_activation_failure)
END_EVENT_NAMESPACE()


#endif // __CPMM_EVC_H__
