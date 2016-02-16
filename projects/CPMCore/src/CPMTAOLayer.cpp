#include "CPMCore/CPMTAOLayer.h"
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/INET_Addr.h"
#include "ICPM/ICPMC.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"
#include "CPMCore/CPMUtils.h"
#include "ExceptionHandling/ExceptionHandling.h"

CPMTAOLayer::CPMTAOLayer()
: ctrl_end_(0)
, init_(false)
, connection_timeout_orb_ms_(1000) // TAO Default
, request_timeout_orb_ms_(1000)
, request_timeout_thread_ms_(1000)
, taolayer_retry_loop_timeout_ms_(0)
, taolayer_retry_attempts_(2)
{
}


CPMTAOLayer::~CPMTAOLayer()
{
    if (ctrl_end_)
    {
        ctrl_end_->wait();
        delete ctrl_end_;
    }
}


bool CPMTAOLayer::init(
    int argc,
    ACE_TCHAR *argv[],
    const char *orb_name)
{
    if (init_)
    {
        return true;
    }

    ctrl_end_ = new CtrlEnd(this);

#if AS_TAO_SERVER == 1
    BasicEventHandling_ns::BasicEvent ev_init_child_poa_failure(
        _SRC,
        ev_init_child_poa_failure_id,
        ev_init_child_poa_failure_text);

    try
    {
        if (orb_manager_.init_child_poa (argc, argv, "child_poa", orb_name) == -1)
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                ev_init_child_poa_failure,
                LM_ERROR);
            return false;
        }
    }
    catch(...)
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_init_child_poa_failure,
            LM_ERROR);
        return false;
    }

    // activate poa_manager
    orb_manager_.activate_poa_manager();
#else
    #if AS_TAO_CLIENT == 1
        if (orb_manager_.init(argc, argv, orb_name) == -1)
        {
            BasicEventHandling_ns::BasicEvent ev_init_orb_manager_failure(
                _SRC,
                ev_init_orb_manager_failure_id,
                ev_init_orb_manager_failure_text);
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                ev_init_orb_manager_failure,
                LM_ERROR);
            return false;
        }
    #endif
#endif

    if (CORBA::is_nil(orb()))
    {
        BasicEventHandling_ns::BasicEvent ev_init_orb_manager_orb_nul(
            _SRC,
            ev_init_orb_manager_orb_nul_id,
            ev_init_orb_manager_orb_nul_text);
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_init_orb_manager_orb_nul,
            LM_ERROR);
        return false;
    }

    init_QOS();

    init_ = true;

    return true;
}

void CPMTAOLayer::init_QOS()
{
    CORBA::Object_var object = orb()->resolve_initial_references("ORBPolicyManager");
    policy_manager_ = CORBA::PolicyManager::_narrow(object.in());

    object = orb()->resolve_initial_references("PolicyCurrent");
    policy_current_ = CORBA::PolicyCurrent::_narrow(object.in());

    // Disable all default policies.
    CORBA::PolicyList policies;
    policies.length(0);
    policy_manager_->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
    policy_current_->set_policy_overrides(policies, CORBA::SET_OVERRIDE);
}

CORBA::ORB_var CPMTAOLayer::orb()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_);
    return orb_manager_.orb();
}


void CPMTAOLayer::shutdown()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_);

    // Here, If we shutdown synchronously, the caller may lose
    // the connection with the object and not get
    // the response. That's why we shutdown asynchronously.
    if (ctrl_end_)
    {
        ctrl_end_->activate();
    }
}


// CPMTAOLayer::set_connection_timeout_orb() is used to reduce
// the time required for the connection before returning a
// failure. It seems to be not possible to set the timeout to
// a value greater than 1 second.
// Returns the previous timeout value or -1 if guard fails.
int CPMTAOLayer::set_connection_timeout_orb(
    int timeout_ms)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
    if (!guard.locked())
    {
        BasicEventHandling_ns::BasicEvent ev_acquire_failed(
            _SRC,
            ev_acquire_failed_id,
            ev_acquire_failed_text);
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_acquire_failed,
            LM_ERROR,
            "shutting_down_mutex_");
        return connection_timeout_orb_ms_;
    }

    int prev = connection_timeout_orb_ms_;

    // This method has effect on remote connection (server host different from client host)
    connection_timeout_orb_ms_ = timeout_ms;

    // TimeT has 100 nanosecond resolution.
    int usec(10); // ==> usec
    int msec(1000); // ==> msec
    int timeout_ns = usec * msec * timeout_ms;

    TimeBase::TimeT relative_rt_timeout = timeout_ns;
    CORBA::Any relative_rt_timeout_as_any;
    relative_rt_timeout_as_any <<= relative_rt_timeout;

    CORBA::PolicyList policies;
    policies.length(1);
    policies[0] = orb()->create_policy (TAO::CONNECTION_TIMEOUT_POLICY_TYPE, relative_rt_timeout_as_any);

    policy_manager_->set_policy_overrides (policies, CORBA::SET_OVERRIDE);

    // Cleanup.
    policies[0]->destroy ();

    return prev;
}

void CPMTAOLayer::get_connection_timeout_orb(
    int & timeout_ms)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_);
    timeout_ms = connection_timeout_orb_ms_;
}

// CPMTAOLayer::set_request_timeout_orb() is used to change
// the time required for the request before returning a
// failure (at orb level).
// Returns the previous timeout value or -1 if guard fails.
int CPMTAOLayer::set_request_timeout_orb(
    int timeout_ms)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
    if (!guard.locked())
    {
        BasicEventHandling_ns::BasicEvent ev_acquire_failed(
            _SRC,
            ev_acquire_failed_id,
            ev_acquire_failed_text);
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_acquire_failed,
            LM_ERROR,
            "shutting_down_mutex_");
        return request_timeout_orb_ms_;
    }

    int prev = request_timeout_orb_ms_;

    request_timeout_orb_ms_ = timeout_ms;

    // TimeT has 100 nanosecond resolution.
    int usec(10); // ==> usec
    int msec(1000); // ==> msec
    int timeout_ns = usec * msec * timeout_ms;

    TimeBase::TimeT relative_rt_timeout = timeout_ns;
    CORBA::Any relative_rt_timeout_as_any;
    relative_rt_timeout_as_any <<= relative_rt_timeout;

    // Create the policy and put it in a policy list.
    CORBA::PolicyList policies;
    policies.length(1);
    policies[0] = orb()->create_policy(Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, relative_rt_timeout_as_any);

    // Apply the policy at the ORB level using the ORBPolicyManager.
    policy_manager_->set_policy_overrides(policies, CORBA::SET_OVERRIDE);

    // Cleanup.
    policies[0]->destroy ();

    return prev;
}

void CPMTAOLayer::get_request_timeout_orb(
    int & timeout_ms)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_);
    timeout_ms = request_timeout_orb_ms_;
}

// CPMTAOLayer::set_request_timeout_thread() is used to change
// the time required for the request before returning a
// failure (at the thread level).
// Returns the previous timeout value or -1 if guard fails.
int CPMTAOLayer::set_request_timeout_thread(
    int timeout_ms)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
    if (!guard.locked())
    {
        BasicEventHandling_ns::BasicEvent ev_acquire_failed(
            _SRC,
            ev_acquire_failed_id,
            ev_acquire_failed_text);
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_acquire_failed,
            LM_ERROR,
            "shutting_down_mutex_");
        return request_timeout_thread_ms_;
    }

    int prev = request_timeout_thread_ms_;

    request_timeout_thread_ms_ = timeout_ms;

    // TimeT has 100 nanosecond resolution.
    int usec(10); // ==> usec
    int msec(1000); // ==> msec
    int timeout_ns = usec * msec * timeout_ms;

    TimeBase::TimeT relative_rt_timeout = timeout_ns;
    CORBA::Any relative_rt_timeout_as_any;
    relative_rt_timeout_as_any <<= relative_rt_timeout;

    // Create the policy and put it in a policy list.
    CORBA::PolicyList policies;
    policies.length(1);
    policies[0] = orb()->create_policy(Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, relative_rt_timeout_as_any);

    // Apply the policy at the ORB level using the ORBPolicyManager.
    policy_current_->set_policy_overrides(policies, CORBA::SET_OVERRIDE);

    // Cleanup.
    policies[0]->destroy ();

    return prev;
}

bool CPMTAOLayer::set_taolayer_retry_params(
    int loop_timeout_ms,
    int attempts,
    bool raise_event)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(set_tao_layer_params_mutex_);

    // Advise CPMD that these params have changed (to let it log the needs to change unreachable_object_timeout_at_start_ms parameter if necessary)

    std::string this_node = CPMUtils_ns::CPMUtils::get_local_addr();

    std::string cpmd_port;
    if (!CPMUtils_ns::CPMUtils::get_cpmd_port(cpmd_port))
    {
        BasicEventHandling_ns::BasicEvent ev_env_var_not_defined(
            _SRC,
            ev_env_var_not_defined_id,
            ev_env_var_not_defined_nul_text);
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_env_var_not_defined,
            LM_ERROR,
            "CPMD_PORT");
        return false;
    }
    std::string location = std::string("corbaloc:iiop:") + this_node + ":" + cpmd_port + "/" + "CPMDObj";

    int loop_timeout_ms_bak = taolayer_retry_loop_timeout_ms_;
    int attempts_bak = taolayer_retry_attempts_;
    taolayer_retry_loop_timeout_ms_ = 0;
    taolayer_retry_attempts_ = 1;
    CPMTAOLayer::GetRefStatus status;
    ICPM_module::ICPMD_var cpmd = get_ref_from_iortable<ICPM_module::ICPMD>(location.c_str(), status);
    taolayer_retry_attempts_ = attempts_bak;
    taolayer_retry_loop_timeout_ms_ = loop_timeout_ms_bak;
    if (!cpmd)
    {
        if (raise_event)
        {
            BasicEventHandling_ns::BasicEvent ev_getting_corba_reference_failure(
                _SRC,
                ev_getting_corba_reference_failure_id,
                ev_getting_corba_reference_failure_nul_text);
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                ev_getting_corba_reference_failure,
                LM_ERROR,
                location.c_str());
        }

        return false;
    }

    taolayer_retry_loop_timeout_ms_ = loop_timeout_ms;
    taolayer_retry_attempts_ = attempts;

    try
    {
        cpmd->on_new_getting_reference_timeout(taolayer_retry_loop_timeout_ms_, taolayer_retry_attempts_);
    }
    catch(...)
    {
        BasicEventHandling_ns::BasicEvent ev_advising_cpmd_with_new_getting_settings_timeout_failure(
            _SRC,
            ev_advising_cpmd_with_new_getting_settings_timeout_failure_id,
            ev_advising_cpmd_with_new_getting_settings_timeout_failure_nul_text);
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            ev_advising_cpmd_with_new_getting_settings_timeout_failure,
            LM_ERROR,
            taolayer_retry_loop_timeout_ms_,
            taolayer_retry_attempts_);
        return false;
    }

    return true;
}

void CPMTAOLayer::get_taolayer_retry_params(
    int & loop_timeout_ms,
    int & attempts)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard(set_tao_layer_params_mutex_);

    loop_timeout_ms = taolayer_retry_loop_timeout_ms_;
    attempts = taolayer_retry_attempts_;
}

void CPMTAOLayer::unregister_objects()
{
    if (init_)
    {
        // unbind from naming service
        CosNaming::NamingContext_var nc;

        if (vect_ref_name_ns_.size() != 0)
        {
            nc = resolve_init<CosNaming::NamingContext>(orb().in(), "NameService");
        }

        std::vector<std::string>::iterator it_ns;
        for (it_ns = vect_ref_name_ns_.begin(); it_ns != vect_ref_name_ns_.end(); it_ns++)
        {
            CosNaming::Name object_ref_name(1);
            object_ref_name.length (1);
            object_ref_name[0].id = CORBA::string_dup(it_ns->c_str());
            nc->unbind(object_ref_name);
        }

        vect_ref_name_ns_.clear();


        // unbind from iortable
        IORTable::Table_var ior_table;
        if (vect_ref_name_iortable_.size() != 0)
        {
            ior_table = resolve_init<IORTable::Table>(orb().in(), "IORTable");
        }

        std::vector<std::string>::iterator it_ior;
        for (it_ior = vect_ref_name_iortable_.begin(); it_ior != vect_ref_name_iortable_.end(); it_ior++)
        {
            ior_table->unbind(it_ior->c_str());
        }

        vect_ref_name_iortable_.clear();
    }
}

void CPMTAOLayer::shutdown_i()
{
   if (init_)
   {
       ExceptionHandling_ns::ExceptionHandling * ex = EXCEPTIONHANDLING;

       unregister_objects();

       orb()->shutdown();

       init_ = false;
   }
}
