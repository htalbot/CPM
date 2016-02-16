#include "CPMTraceEraserAppLayer.h"
#include "CPMTraceEraser/version.h"
#include "CPMTraceEraser/CPMTraceEraser.evc.h"
#include "CPMCore/CPMEventRaiser.h"

//***************************************
// Register objets types                                                        // <--
//REGISTER_TYPEOF(1, CPMObjDummy_i)
//REGISTER_TYPEOF(2, <*>_i)
//***************************************

CPMTraceEraserAppLayer::CPMTraceEraserAppLayer()
{
    CPMAPPLAYER->set(this);
}

CPMTraceEraserAppLayer::~CPMTraceEraserAppLayer()
{
}

bool CPMTraceEraserAppLayer::init_corba_objects()
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
        RAISE_EVENT(CPMTraceEraserEvents::ev_object_registration_with_ior_table_failure,
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

bool CPMTraceEraserAppLayer::run()
{
    show_console_on_init();

    // Since ev_server_stopped event is generated at the main() level and that it is
    // as a system log event, we generate ev_server_started as a system log too.
    EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
        CPMTraceEraserEvents::ev_server_started,
        LM_INFO,
        cpmo_id_.c_str());

    ACE_DEBUG((LM_INFO, "%s server started, waiting for requests...\n\n", cpmo_id_.c_str()));
    tao_layer_->orb()->run();

    return true;
}


bool CPMTraceEraserAppLayer::objects_register()
{
    try
    {
        //***************************************
        // Register objects with ORB                                            // <--
        //ObjToRegister<CPMObjDummy_i> obj(cpm_obj_dummy_,
        //                                    "CPMObjDummy",
        //                                    "CPMObjDummy_in_AOT");
        //obj.register_with_ior_table(objects_ids_);

        //ObjToRegister<CPMObjDummy_i> obj2(cpm_obj_dummy2_,
        //                                    "CPMObjDummy2",
        //                                    "CPMObjDummy2_in_AOT");
        //obj2.register_with_ior_table(objects_ids_);
        //***************************************
    }
    catch(...)
    {
        RAISE_EVENT(CPMTraceEraserEvents::ev_object_registration_with_ior_table_failure2,
                    LM_ERROR);
        return false;
    }

    return true;
}


bool CPMTraceEraserAppLayer::objects_init()
{
    try
    {
        if (!cpm_trace_eraser_obj_.init())
        {
            RAISE_EVENT(CPMTraceEraserEvents::ev_object_init_failure,
                        LM_ERROR);
            return false;
        }
    }
    catch(...)
    {
        RAISE_EVENT(CPMTraceEraserEvents::ev_object_init_failure,
                    LM_ERROR);
        return false;
    }

    return true;
}

void CPMTraceEraserAppLayer::terminate_()
{
    cpm_trace_eraser_obj_.terminate();
}

