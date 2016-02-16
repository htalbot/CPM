#include "CPMFooAppLayer.h"
#include "CPMFoo/version.h"
#include "CPMFoo/CPMFoo.evc.h"
#include "CPMCore/CPMEventRaiser.h"

REGISTER_TYPEOF(1, CPMObjDummy_i)

CPMFooAppLayer::CPMFooAppLayer()
{
    CPMAPPLAYER->set(this);
}

CPMFooAppLayer::~CPMFooAppLayer()
{
}

bool CPMFooAppLayer::init_corba_objects()
{
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
        RAISE_EVENT(CPMFooEvents::ev_object_registration_with_ior_table_failure,
                    LM_ERROR,
                    cpmo_id_.c_str());
        return false;
    }

    if (!objects_init())
    {
        return false;
    }

    if (!objects_register())
    {
        return false;
    }

    return true;
}

bool CPMFooAppLayer::run()
{
    show_console_on_init();

    // Since ev_server_stopped event is generated at the main() level and that it is
    // as a system log event, we generate ev_server_started as a system log too.
    EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
        CPMFooEvents::ev_server_started,
        LM_INFO,
        cpmo_id_.c_str());

    ACE_DEBUG((LM_INFO, "%s server started, waiting for requests...\n\n", cpmo_id_.c_str()));
    tao_layer_->orb()->run();

    return true;
}


bool CPMFooAppLayer::objects_register()
{
    std::string obj_id = "CPMObjFoo1";
    std::string obj2_id = "CPMObjFoo2";

    try
    {
        ObjToRegister<CPMObjDummy_i> obj(cpm_obj_dummy_,
                                            obj_id,
                                            "CPMObjFoo1_in_AOT");
        obj.register_with_ior_table(objects_ids_);

        ObjToRegister<CPMObjDummy_i> obj2(cpm_obj_dummy2_,
                                            obj2_id,
                                            "CPMObjFoo2_in_AOT");
        obj2.register_with_ior_table(objects_ids_);
    }
    catch(...)
    {
        RAISE_EVENT(CPMFooEvents::ev_object_registration_with_ior_table_failure2,
                    LM_ERROR);
        return false;
    }

    return true;
}


bool CPMFooAppLayer::objects_init()
{
    try
    {
        if (!cpm_obj_dummy_.init_i())
        {
            RAISE_EVENT(CPMFooEvents::ev_object_init_failure,
                        LM_ERROR);
            return false;
        }

        if (!cpm_obj_dummy2_.init_i())
        {
            RAISE_EVENT(CPMFooEvents::ev_object_init_failure,
                        LM_ERROR);
            return false;
        }
    }
    catch(...)
    {
        RAISE_EVENT(CPMFooEvents::ev_object_init_failure,
                    LM_ERROR);
        return false;
    }

    return true;
}

void CPMFooAppLayer::terminate_()
{
}

