#ifndef __CPMBAR_EVC_H__
#define __CPMBAR_EVC_H__

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMBarEvents)
    STATIC_EVENT_DECL(ev_missing_arg)
    STATIC_EVENT_DECL(ev_arg_parsing_failure)
    STATIC_EVENT_DECL(ev_app_layer_failure)
    STATIC_EVENT_DECL(ev_tao_layer_init_failure)
    STATIC_EVENT_DECL(ev_object_registration_with_ior_table_failure)
    STATIC_EVENT_DECL(ev_server_started)
    STATIC_EVENT_DECL(ev_object_registration_with_ior_table_failure2)
    STATIC_EVENT_DECL(ev_object_init_failure)
    STATIC_EVENT_DECL(ev_server_stopped)
    STATIC_EVENT_DECL(ev_main_exception)
    STATIC_EVENT_DECL(ev_singleton_close_exception)
    STATIC_EVENT_DECL(ev_server_already_running)
    STATIC_EVENT_DECL(ev_bad_parameters)
    STATIC_EVENT_DECL(ev_cant_add_option)
    STATIC_EVENT_DECL(ev_args_parsing_failure)
    STATIC_EVENT_DECL(ev_corba_objects_init_failure)
END_EVENT_NAMESPACE()


#endif // __CPMBAR_EVC_H__
