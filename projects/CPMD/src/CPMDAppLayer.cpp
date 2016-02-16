
#include "CPMDAppLayer.h"
#include "CPMD/version.h"
#include "CPMD/CPMD.evc.h"

CPMDAppLayer::CPMDAppLayer()
{
    CPMAPPLAYER->set(this);
}

CPMDAppLayer::~CPMDAppLayer()
{
}

bool CPMDAppLayer::run()
{
    if (!objects_init())
    {
        return false;
    }

    //ACE_DEBUG((LM_DEBUG, "CPMDAppLayer::run() - cpmo_id_ = %s\n", cpmo_id_.c_str()));
    try
    {
        tao_layer_->register_with_ior_table<
                                CPMO_i,
                                ::ICPM_module::ICPMProcess>
                            (
                                &obj_impl_,
                                "CPMD_in_AOT",
                                cpmo_id_.c_str()
                            );
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_object_registration_with_ior_table_failure,
            LM_ERROR,
            cpmo_id_.c_str());

        return false;
    }

    // See comment in objects_register().
    //if (!objects_register())
    //{
    //    see comment in objects_register().
    //    return false;
    //}

    std::string cpmd_object_id = "CPMDObj";
    //ACE_DEBUG((LM_DEBUG, "CPMDAppLayer::run() - cpmd_object_id = %s\n", cpmd_object_id.c_str()));
    try
    {
        tao_layer_->register_with_ior_table<
                                CPMDObj_i,
                                ::ICPM_module::ICPMD>
                            (
                                &cpmd_obj_impl_,
                                "CPMDObj_in_AOT",
                                cpmd_object_id.c_str()
                            );
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_object_registration_with_ior_table_failure,
            LM_ERROR,
            cpmo_id_.c_str());

        return false;
    }

    std::string cpm_event_mgr_object_id = "CPMEventMgrObj";
    //ACE_DEBUG((LM_DEBUG, "CPMDAppLayer::run() - cpm_event_mgr_object_id = %s\n", cpm_event_mgr_object_id.c_str()));
    try
    {
        tao_layer_->register_with_ior_table<
                                CPMEventMgrObj_i,
                                ICPM_module::ICPMEventMgr>
                            (
                                &cpm_event_mgr_obj_,
                                "CPMEventMgrObj_in_AOT",
                                cpm_event_mgr_object_id.c_str()
                            );
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_object_registration_with_ior_table_failure,
            LM_ERROR,
            cpm_event_mgr_object_id.c_str());

        return false;
    }

    // Since ev_server_stopped event is generated at the main() level and that it is
    // as a system log event, we generate ev_server_started as a system log too.
    EVENT_HANDLER_SYSTEMLOG->handle_event(
        _SRC,
        CPMDEvents::ev_server_started,
        LM_INFO,
        "CPMD");

    if (int rc = cpmd_obj_impl_.activate() != 0)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_task_activation_failure,
            LM_ERROR,
            rc);
        return false;
    }

    ACE_DEBUG((LM_INFO, "%s server started, waiting for requests...\n\n", cpmo_id_.c_str()));
    try
    {
        tao_layer_->orb()->run();
    }
    catch(...)
    {
        return false;
    }

    return true;
}

bool CPMDAppLayer::objects_register()
{
    // Since CPMDObj_i doesn't inherit directly from POA_ICPM_module::ICPMObject, 
    // the template ObjToRegister<CPMDObj_i> has problem. So, we register 
    // tao_layer_->register_with_ior_table<>

    return true;
}

bool CPMDAppLayer::objects_init()
{
    //ACE_DEBUG((LM_DEBUG, "CPMDAppLayer::objects_init()\n"));

    try
    {
        if (!cpm_event_mgr_obj_.init_i())
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMDEvents::ev_event_mgr_initialization_failure,
                LM_ERROR);

            return false;
        }
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMDEvents::ev_object_init_failure,
            LM_ERROR);
        return false;
    }

    return true;
}

void CPMDAppLayer::terminate_()
{
    cpmd_obj_impl_.terminate();
}
