
#include "CPMCore/CPMO_i.h"
#include "CPMCore/CPMUtils.h"

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Task.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

class CtrlEnd: public ACE_Task_Base
{
public:
    CtrlEnd()
    {
        //CPMTRACER->trace("%s", _SRC);
    }

    ~CtrlEnd()
    {
        //CPMTRACER->trace("%s", _SRC);
    }

    void set_cpmo(CPMO_i * cpmo)
    {
        cpmo_ = cpmo;
    }

    virtual int svc()
    {
        cpmo_->app_layer_->ctrl_end();
        return 0;
    }

protected:
    CPMO_i * cpmo_;
};


CtrlEnd g_ctrl_end;


CPMO_i::CPMO_i(::ICPM_module::ICPMProcess * app_layer)
: app_layer_(app_layer)
{
    //CPMTRACER->trace("%s", _SRC);
    g_ctrl_end.set_cpmo(this);
}

CPMO_i::~CPMO_i()
{
    //CPMTRACER->trace("%s", _SRC);
}

void CPMO_i::object_list(::ICPM_module::ObjectDescSeq_out list)
{
     try
     {
         app_layer_->object_list(list);
     }
     catch(...)
     {
         return;
     }
}

void CPMO_i::ctrl_end()
{
    g_ctrl_end.activate();
}

void CPMO_i::ctrl_ping()
{
}

void CPMO_i::show_console(::CORBA::Boolean show)
{
    //ACE_DEBUG((LM_DEBUG, "CPMO_i::show_console() - %s\n", show ? "SHOW" : "HIDE"));
    app_layer_->show_console(show);
}

void CPMO_i::show_gui(::CORBA::Boolean show)
{
    //ACE_DEBUG((LM_DEBUG, "CPMO_i::show_gui() - %s\n", show ? "SHOW" : "HIDE"));
    app_layer_->show_gui(show);
}

void CPMO_i::as_server(::CORBA::Boolean as_server)
{
    app_layer_->as_server(as_server);
}

void CPMO_i::as_gui(::CORBA::Boolean as_gui)
{
    app_layer_->as_gui(as_gui);
}

::CORBA::Long CPMO_i::getpid ()
{
    pid_t pid = ACE_OS::getpid();
    return pid;
}
