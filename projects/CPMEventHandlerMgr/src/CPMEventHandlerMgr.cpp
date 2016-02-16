
#include "CPMEventHandlerMgr/CPMEventHandlerMgr.h"
#include "CPMEventHandlerMgr/CPMEventHandlerMgr.evc.h"
#include "CPMEventHandlerMgr/version.h"
#include "CPMCore/CPMUtils.h"

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/ARGV.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "BasicEventCorba/BasicEventCorba.h"

#include "CPMCore/CPMAppLayer.h"

namespace CPMEventHandlerMgr_ns
{
    PLUGIN_MEM_DEFINE(CPMEVENTHANDLERMGR_Export, CPMEventHandlerMgr, CPMEventHandlerAbstract_ns, CPMEventHandlerAbstract)

    void CPMEventHandlerMgr_get_version(int & major, int & minor, int & patch)
    {
        major = CPMEVENTHANDLERMGR_MAJOR;
        minor = CPMEVENTHANDLERMGR_MINOR;
        patch = CPMEVENTHANDLERMGR_PATCH;
    }

    CPMEventHandlerMgr::CPMEventHandlerMgr()
    : initialized_(false)
    {
    }

    CPMEventHandlerMgr::~CPMEventHandlerMgr()
    {
        //ACE_DEBUG((LM_DEBUG, "%n - ~CPMEventHandlerMgr()\n"));
    }

    bool CPMEventHandlerMgr::init()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(init_mutex_);

        if (initialized_)
        {
            return true;
        }

        // Get reference on CPMEventMgrObj
        std::string current_node = CPMUtils_ns::CPMUtils::get_local_addr();

        std::string location("corbaloc:iiop:");
        location += current_node + ":";

        std::string env_var("CPMD_PORT");
        char * pPort = getenv (env_var.c_str());
        if (!pPort)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMEventHandlerMgrEvents::ev_env_var_not_defined,
                LM_ERROR,
                env_var.c_str());
            return false;
        }

        location += std::string(pPort) + "/" + "CPMEventMgrObj";

        int prev_loop_timeout;
        int prev_attempts;
        CPMTAOLAYER->get_taolayer_retry_params(prev_loop_timeout, prev_attempts);

        CPMTAOLAYER->set_taolayer_retry_params(0, 1);

        CPMTAOLayer::GetRefStatus status;
        ICPM_module::ICPMEventMgr_var cpm_event_mgr = CPMTAOLAYER->get_ref_from_iortable<ICPM_module::ICPMEventMgr>(location.c_str(), status);

        CPMTAOLAYER->set_taolayer_retry_params(prev_loop_timeout, prev_attempts);

        if (!cpm_event_mgr)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMEventHandlerMgrEvents::ev_getting_reference_on_corba_object_failure,
                LM_ERROR,
                location.c_str());
            return false;
        }

        timed_cpm_event_mgr_ = CPMTAOLAYER->timed_object<ICPM_module::ICPMEventMgr>(cpm_event_mgr, 200);

        initialized_ = true;

        return true;
    }

    bool CPMEventHandlerMgr::handle_event(const char * szSource,
                                BasicEventHandling_ns::BasicEvent & ev,
                                ACE_Log_Priority nPriority,
                                ...)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(handle_event_mutex_);

        va_list args;
        va_start(args, nPriority);
        bool rc = vhandle_event(szSource, ev, nPriority, args);
        va_end(args);

        return rc;
    }

    bool CPMEventHandlerMgr::vhandle_event(const char * szSource,
                        BasicEventHandling_ns::BasicEvent & ev,
                        ACE_Log_Priority nPriority,
                        va_list args)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(vhandle_event_mutex_);

        IBASICEVENTCORBA_module::BasicEventCorba corba_event;

        if (!BasicEventCorba_ns::BasicEventCorba::vconvert(
            ev,
            corba_event,
            CPMUtils_ns::CPMUtils::get_local_addr().c_str(),
            szSource,
            nPriority,
            args))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMEventHandlerMgrEvents::ev_converting_basicevent_to_corbaevent_failure,
                LM_ERROR,
                ev.namespace_(),
                ev.id());
            return false;
        }

        // Send the event to CPMEventMgrObj
        try
        {
            timed_cpm_event_mgr_->push(corba_event);
        }
        catch(...)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMEventHandlerMgrEvents::ev_pushing_event_to_cpm_event_mgr_failure,
                LM_ERROR,
                ev.id());
            EVENT_HANDLER_SYSTEMLOG->vhandle_event(
                _SRC,
                ev,
                nPriority,
                args);
            return false;
        }

        return true;
    }

    void CPMEventHandlerMgr::get_version(int & major, int & minor, int & patch)
    {
        CPMEventHandlerMgr_get_version(major, minor, patch);
    }

} // CPMEventHandlerMgr_ns namespace
