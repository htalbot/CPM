    
#ifndef CPMO_I_H
#define CPMO_I_H

#include "CPMCORE_Export.h"

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ICPM/ICPMS.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

class CPMCORE_Export CPMO_i : public POA_ICPM_module::ICPMProcess
{
public:
    CPMO_i(::ICPM_module::ICPMProcess * app_layer);
    ~CPMO_i();

    virtual void object_list(::ICPM_module::ObjectDescSeq_out list);
    virtual void ctrl_end ();
    virtual void ctrl_ping ();
    virtual void show_console(::CORBA::Boolean show);
    virtual void show_gui(::CORBA::Boolean show);
    virtual void as_server(::CORBA::Boolean as_server);
    virtual void as_gui(::CORBA::Boolean as_gui);
    virtual ::CORBA::Long getpid ();

    ::ICPM_module::ICPMProcess * app_layer_;
};

#endif /* CPMO_I_H */


