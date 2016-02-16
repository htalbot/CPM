#ifndef __CPMEVENTHANDLERMGR_EVC_H__
#define __CPMEVENTHANDLERMGR_EVC_H__

#include "BasicEventHandling/BasicEventHandling.h"

BEGIN_EVENT_DECL_NAMESPACE(CPMEventHandlerMgrEvents)
    STATIC_EVENT_DECL(ev_orb_init_failure)
    STATIC_EVENT_DECL(ev_env_var_not_defined)
    STATIC_EVENT_DECL(ev_getting_reference_on_corba_object_failure)
    STATIC_EVENT_DECL(ev_pushing_event_to_cpm_event_mgr_failure)
    STATIC_EVENT_DECL(ev_converting_basicevent_to_corbaevent_failure)
END_EVENT_NAMESPACE()


#endif // __CPMEVENTHANDLERMGR_EVC_H__
