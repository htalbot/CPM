
#include "CPMMAppLayer.h"
#include "CPMM/version.h"
#include "CPMM/CPMM.evc.h"
#include "CPMCore/CPMEventRaiser.h"
#include "CPMCore/CPMUtils.h"

CPMMAppLayer::CPMMAppLayer()
{
    CPMAPPLAYER->set(this);
}

CPMMAppLayer::~CPMMAppLayer()
{
}

void CPMMAppLayer::set_overwrite_autostart_to_false(bool overwrite_autostart_to_false)
{
    overwrite_autostart_to_false_ = overwrite_autostart_to_false;
}

bool CPMMAppLayer::run()
{
    INIT_RAISER_RETURN(false);

    //CPMTRACER->trace("%s - cpmm_obj_impl_.init_i...", _SRC);
    if (!cpmm_obj_impl_.init_i(overwrite_autostart_to_false_))
    {
        RAISE_EVENT(CPMMEvents::ev_cpmm_object_init_failure,
                    LM_ERROR);
        return false;
    }

    //CPMTRACER->trace("%s - tao_layer_->register_with_ior_table<%s>", _SRC, cpmo_id_.c_str());
    //ACE_DEBUG((LM_DEBUG, "CPMMAppLayer::run() - cpmo_id_ = %s\n", cpmo_id_.c_str()));
    try
    {
        tao_layer_->register_with_ior_table<
                                CPMO_i,
                                ::ICPM_module::ICPMProcess>
                            (
                                &obj_impl_,
                                (cpmo_id_ + "_in_AOT").c_str(),
                                cpmo_id_.c_str()
                            );
    }
    catch(...)
    {
        RAISE_EVENT(CPMMEvents::ev_object_registration_with_ior_table_failure,
                    LM_ERROR,
                    cpmo_id_.c_str());
        return false;
    }

    std::string cpmm_object_id("CPMMObj");
    //CPMTRACER->trace("%s - tao_layer_->register_with_ior_table<%s>", _SRC, cpmm_object_id.c_str());
    //ACE_DEBUG((LM_DEBUG, "CPMMAppLayer::run() - object_id = %s\n", object_id.c_str()));
    try
    {
        tao_layer_->register_with_ior_table<
                                CPMMObj_i,
                                ::ICPM_module::ICPMM>
                            (
                                &cpmm_obj_impl_,
                                "CPMMObj_in_AOT",
                                cpmm_object_id.c_str()
                            );
    }
    catch(...)
    {
        RAISE_EVENT(CPMMEvents::ev_object_registration_with_ior_table_failure,
                    LM_ERROR,
                    cpmm_object_id.c_str());
        return false;
    }

    // Since ev_server_stopped event is generated at the main() level and that it is
    // as a system log event, we generate ev_server_started as a system log too.
    EVENT_HANDLER_SYSTEMLOG->handle_event(
        _SRC,
        CPMMEvents::ev_server_started,
        LM_INFO,
        cpmo_id_.c_str());

    if (int rc = cpmm_obj_impl_.activate() != 0)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMMEvents::ev_task_activation_failure,
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

    //CPMTRACER->trace("%s - return true.", _SRC);
    return true;
}

bool CPMMAppLayer::objects_register()
{
    // Since CPMMObj_i doesn't inherit directly from POA_ICPM_module::ICPMObject, 
    // the template ObjToRegister<CPMMObj_i> has problem. So, we register 
    // tao_layer_->register_with_ior_table<>

    return true;
}

bool CPMMAppLayer::objects_init()
{
    // Done outside this method because it needs auto_launch_processes parameter.
    return true;
}

void CPMMAppLayer::terminate_()
{
    cpmm_obj_impl_.terminate();
}
