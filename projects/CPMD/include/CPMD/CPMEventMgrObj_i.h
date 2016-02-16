#ifndef CPMEVENTMGR_CPMEVENTMGROBJ_I_H__
#define CPMEVENTMGR_CPMEVENTMGROBJ_I_H__

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ICPM/ICPMS.h"
#include "ICPM/ICPMC.h"
#include "ace/Task.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"
#include "CyclicLog/CyclicLog.h"
#include "BasicEventHandling/PrintfBase.h"
#include "IBasicEventCorba/IBasicEventCorbaC.h"

class CPMEventMgrObj_i : 
    public POA_ICPM_module::ICPMEventMgr,
    public ACE_Task<ACE_MT_SYNCH>,
    protected BasicEventHandling_ns::PrintfBase
{
public:
    CPMEventMgrObj_i();
    ~CPMEventMgrObj_i();

    virtual void get_version (
      ::CORBA::Long_out major,
      ::CORBA::Long_out minor,
      ::CORBA::Long_out patch);

    virtual void ctrl_ping (
      void);

    virtual void push (
      const ::IBASICEVENTCORBA_module::BasicEventCorba & basic_event_corba);

public:
    bool init_i();

    enum {
        NEW_EVENT,
        TERMINATE
    } MsgType;
    
protected:
    bool load_settings();
    bool open_log_file();

    void vsprintf(std::string & text, const ::IBASICEVENTCORBA_module::BasicEventCorba & basic_event_corba)
    {
        args_ = const_cast<::IBASICEVENTCORBA_module::EventArgSeq *>(&basic_event_corba.args_);
        loop(text, basic_event_corba.format_.operator const CORBA::Char *());
    }
    
    virtual long get_long_value(int param_no) // %ld, %li, %lo, %lx, %lX
    {
        long n = (*args_)[param_no - 1].arg_.lValue();
        return n;
    }

    virtual unsigned long get_unsigned_long_value(int param_no) // %lu
    {
        unsigned long n = (*args_)[param_no - 1].arg_.ulValue();
        return n;
    }

    virtual double get_double_value(int param_no) // %lf, %lF, %le, %lE, %lg, %lG
    {
        double n = (*args_)[param_no - 1].arg_.dValue();
        return n;
    }

    virtual long long get_long_long_value(int param_no) // lld
    {
        long long n = (*args_)[param_no - 1].arg_.llValue();
        return n;
    }

    virtual unsigned long long get_unsigned_long_long_value(int param_no) // llu
    {
        unsigned long long n = (*args_)[param_no - 1].arg_.ullValue();
        return n;
    }

    virtual std::string get_string_value(int param_no) // %s
    {
        std::string s = (*args_)[param_no - 1].arg_.strValue();

        return s;
    }

protected:
    int svc();

protected:
    CyclicLog_ns::CyclicLog log_;
    std::string file_;
    int line_size_;
    int file_size_;
    bool flush_;

    ::IBASICEVENTCORBA_module::EventArgSeq * args_;
};

#endif /* CPMEVENTMGR_CPMEVENTMGROBJ_I_H__ */


