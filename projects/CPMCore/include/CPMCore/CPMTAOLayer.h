#ifndef CPM_TAOLAYER_H
#define CPM_TAOLAYER_H

#include "CPMCore/CPMUtils.h"

#include "CPMCORE_Export.h"
#include "BasicEventHandling/BasicEventHandling.h"

#include "ACEWin64Warnings/BeginSuppressWarnings.h"

#include "ace/Task.h"
#include <string>
#include <vector>

#define AS_TAO_SERVER 1
#define AS_TAO_CLIENT 1

#if !defined(AS_TAO_SERVER) && !defined(AS_TAO_CLIENT)
#error AS_TAO_SERVER or AS_TAO_CLIENT (or both) has to be defined
#endif

// Needed TAO headers
#include "CPMCore/CPMTAOLayerContrib.h"
#if AS_TAO_SERVER == 1
#include "orbsvcs/CosNamingS.h"
#endif
#if AS_TAO_CLIENT == 1
#include "orbsvcs/CosNamingC.h"
#endif

#include "TAO/tao/Messaging/Messaging.h"
#include "TAOLayerUtils.h"


// Since CPMTAOLayer contains template member functions,
// we must provide BasicEvent objects inside those member functions.

#define ev_resolving_failure_id 1
#define ev_resolving_failure_text "ORB can't resolve initial reference: '%1(%s)'."

#define ev_making_corba_object_failure_id 2
#define ev_making_corba_object_failure_text "Can't make corba object: '%1(%s)'."

#define ev_activating_under_child_poa_failure_id 3
#define ev_activating_under_child_poa_failure_text "Can't activate object under child POA: '%1(%s)'."

#define ev_init_child_poa_failure_id 4
#define ev_init_child_poa_failure_text "Can't init child POA."

#define ev_init_orb_manager_failure_id 5
#define ev_init_orb_manager_failure_text "Can't init ORB manager."

#define ev_init_orb_manager_orb_nul_id 6
#define ev_init_orb_manager_orb_nul_text "ORB manager initialization returned null orb."

#define ev_get_config_param_env_var_not_defined_id 7
#define ev_get_config_param_env_var_not_defined_text "Environment variable is not defined: '%1(%s)'."

#define ev_field_not_found_id 8
#define ev_field_not_found_text "Field not found in '%1(%s)': '%2(%s)'."

#define ev_exception_with_settings_file_id 9
#define ev_exception_with_settings_file_text "Exception while reading settings file: '%1(%s)'."

#define ev_env_var_not_defined_id 10
#define ev_env_var_not_defined_nul_text "Environmanet variable not defined: '%1(%s)'."

#define ev_getting_corba_reference_failure_id 11
#define ev_getting_corba_reference_failure_nul_text "Can't get reference on CPMD: '%1(%s)'."

#define ev_advising_cpmd_with_new_getting_settings_timeout_failure_id 12
#define ev_advising_cpmd_with_new_getting_settings_timeout_failure_nul_text "Can't advise CPMD with new getting reference attempts - loop timeout: %1(%d), attempts: %2(%d)."

#define ev_corba_system_exception_id 13
#define ev_corba_system_exception_nul_text "Corba::SystemException: '%1(%s)'."

#define ev_corba_exception_id 14
#define ev_corba_exception_nul_text "Corba::Exception: '%1(%s)'."

#define ev_orb_run_exception_id 15
#define ev_orb_run_exception_nul_text "Unkbown orb run exception."

#define ev_register_with_naming_service_exception_id 16
#define ev_register_with_naming_service_exception_text "'register_with_naming_service_exception(%1(%s))' exception."

#define register_with_ior_table_exception_id 17
#define register_with_ior_table_exception_text "'register_with_iortable_exception(%1(%s))' exception."

#define ev_acquire_failed_id 18
#define ev_acquire_failed_text "ACE_Recursive_Thread_Mutex::acquire failed for '%1(%s)'."


class CtrlEnd;

/**
 * @class CPMTAOLayer
 *
 * @brief Bridges the TAO and Application Layer functionality.
 *
 */
class CPMCORE_Export CPMTAOLayer /*:
    public ACE_Task_Base*/
{
    class CtrlEnd: public ACE_Task_Base
    {
    public:
        CtrlEnd(CPMTAOLayer * tao_layer)
        : tao_layer_(tao_layer)
        {
        }

        virtual int svc()
        {
            // Let the caller get the response.
            ACE_Time_Value tv(0, 800000);
            ACE_OS::sleep(tv);

            // Shutdown.
            tao_layer_->shutdown_i();

            return 0;
        }

    protected:
        CPMTAOLayer * tao_layer_;
    };

public:

    enum GetRefStatus {
        GET_REF_OK,
        GET_REF_EXCEPTION,
        GET_REF_CONCURRENCY
    };

    CPMTAOLayer();

    virtual ~CPMTAOLayer();

    virtual bool init(
        int argc,
        ACE_TCHAR *argv[],
        const char *orb_name = 0);

    virtual CORBA::ORB_var orb();
    virtual void shutdown();

    virtual int set_connection_timeout_orb(
        int timeout_ms);
    virtual int set_request_timeout_orb(
        int timeout_ms);
    virtual int set_request_timeout_thread(
        int timeout_ms);

    virtual void get_connection_timeout_orb(
        int & connection_timeout_orb_ms);
    virtual void get_request_timeout_orb(
        int & timeout_ms);

    virtual bool set_taolayer_retry_params(
        int loop_timeout_ms, 
        int attempts, 
        bool raise_event = true);
    virtual void get_taolayer_retry_params(
        int & loop_timeout_ms, 
        int & attempts);

    template <typename Interface>
    typename Interface::_ptr_type timed_object(
        typename Interface::_ptr_type obj, 
        long timeout_ms)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
        if (!guard.locked())
        {
            typename Interface::_var_type ref;
            return ref._retn();
        }

        int usec(10); // ==> usec
        int msec(1000); // ==> msec
        int timeout_ns = usec * msec * timeout_ms;

        TimeBase::TimeT relative_rt_timeout = timeout_ns;
        CORBA::Any relative_rt_timeout_as_any;
        relative_rt_timeout_as_any <<= relative_rt_timeout;

        // Create the policy and put it in a policy list.
        CORBA::PolicyList policies;
        policies.length(1);

        // Override the object policies.
        policies[0] = orb()->create_policy(Messaging::RELATIVE_RT_TIMEOUT_POLICY_TYPE, relative_rt_timeout_as_any);

        // Create a new object reference!
        CORBA::Object_var object = obj->_set_policy_overrides(policies, CORBA::SET_OVERRIDE);
        typename Interface::_var_type ref = Interface::_narrow(object.in());

        // Cleanup.
        policies[0]->destroy();

        return ref._retn();
    }

    template <typename Interface>
    typename Interface::_ptr_type get_ref_from_ns(
        const char * szName,
        GetRefStatus & status,
        int taolayer_retry_attempts = -1 /* >=1 */,
        int taolayer_retry_loop_timeout_ms = -1,
        bool use_min_timeout = false)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
        if (!guard.locked())
        {
            status = GET_REF_CONCURRENCY;
            return 0;
        }

        if (taolayer_retry_attempts == 0)
        {
            taolayer_retry_attempts = 1; // We must make at least one iteration.
        }

        if (taolayer_retry_attempts == -1)
        {
            taolayer_retry_attempts = taolayer_retry_attempts_;
        }

        if (taolayer_retry_loop_timeout_ms == -1)
        {
            taolayer_retry_loop_timeout_ms = taolayer_retry_loop_timeout_ms_;
        }

        CosNaming::Name object_name(1);
        object_name.length (1);
        object_name[0].id = CORBA::string_dup (szName);

        std::string service_id("NameService");
        for (int i = 0; i != taolayer_retry_attempts; i++)
        {
            // Here, either a delay of ~1 second or an exception happens when 
            // connection fails.
            // When an exception happens, we must sleep at least 1 second, else
            // the iterations (attempts) will go very fast. This causes an unexpected
            // behaviour.

            ACE_UINT64 diff(0);

            ACE_Time_Value begin = ACE_OS::gettimeofday();
            try
            {
                // get a ref on naming service
                CosNaming::NamingContext_var nc = resolve_init<CosNaming::NamingContext>(
                                                            orb_manager_.orb(),
                                                            service_id.c_str());

                // resolve name and get object ref
                CORBA::Object_var obj = resolve_name<Interface>(nc.in(), object_name);
                typename Interface::_var_type ref = Interface::_narrow(obj.in());
                if (CORBA::is_nil(ref.ptr()))
                {
                    //ACE_DEBUG((LM_DEBUG, "%T - %s - _narrow failed: %s\n", _SRC, szName));
                    diff = 1000 - (ACE_OS::gettimeofday() - begin).get_msec();
                }
                else
                {
                    status = GET_REF_OK;
                    return ref._retn();
                }
            }
            catch(...)
            {
                // If exception happens we must sleep at least one second.
                //ACE_DEBUG((LM_DEBUG, "%T - %s - exception: %s\n", _SRC, szName));
                diff = 1000 - (ACE_OS::gettimeofday() - begin).get_msec();
                status = GET_REF_EXCEPTION;
            }

            if (diff != 0)
            {
                if (use_min_timeout)
                {
                    ACE_OS::sleep(ACE_Time_Value(0, std::min((int)diff * 1000, taolayer_retry_loop_timeout_ms * 1000)));
                }
                else
                {
                    ACE_OS::sleep(ACE_Time_Value(0, std::max((int)diff * 1000, taolayer_retry_loop_timeout_ms * 1000)));
                }
            }
        }

        return 0;
    }


    template <typename Interface>
    typename Interface::_ptr_type get_ref_from_iortable(
        const char * szName,
        GetRefStatus & status,
        int taolayer_retry_attempts = -1 /* >=1 */,
        int taolayer_retry_loop_timeout_ms = -1,
        bool use_min_timeout = false)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
        if (!guard.locked())
        {
            status = GET_REF_CONCURRENCY;
            return 0;
        }

        if (taolayer_retry_attempts == 0)
        {
            taolayer_retry_attempts = 1; // We must make at least one iteration.
        }

        if (taolayer_retry_attempts == -1)
        {
            taolayer_retry_attempts = taolayer_retry_attempts_;
        }

        if (taolayer_retry_loop_timeout_ms == -1)
        {
            taolayer_retry_loop_timeout_ms = taolayer_retry_loop_timeout_ms_;
        }

        for (int i = 0; i != taolayer_retry_attempts; i++)
        {
            // Here, either a delay of ~1 second or an exception happens when 
            // connection fails.
            // When an exception happens, we must sleep at least 1 second, else
            // the iterations (attempts) will go very fast. This causes an unexpected
            // behaviour.

            ACE_UINT64 diff(0);

            ACE_Time_Value begin = ACE_OS::gettimeofday();
            try
            {
                CORBA::Object_var obj = orb()->string_to_object(szName);
                typename Interface::_var_type ref = Interface::_narrow(obj.in());
                if (CORBA::is_nil(ref.in()))
                {
                    //ACE_DEBUG((LM_DEBUG, "%T - %s - _narrow failed: %s\n", _SRC, szName));
                    diff = 1000 - (ACE_OS::gettimeofday() - begin).get_msec();
                }
                else
                {
                    status = GET_REF_OK;
                    return ref._retn();
                }
            }
            catch(...)
            {
                // If exception happens we must sleep at least one second.
                //ACE_DEBUG((LM_DEBUG, "%T - %s - exception: %s\n", _SRC, szName));
                diff = 1000 - (ACE_OS::gettimeofday() - begin).get_msec();
                status = GET_REF_EXCEPTION;
            }

            if (diff != 0)
            {
                if (use_min_timeout)
                {
                    int minimum = std::min((int)diff * 1000, taolayer_retry_loop_timeout_ms * 1000);
                    //ACE_DEBUG((LM_DEBUG, "%T - %s - minimum = %u\n", _SRC, minimum));
                    ACE_OS::sleep(ACE_Time_Value(0, minimum));
                }
                else
                {
                    int maximum = std::max((int)diff * 1000, taolayer_retry_loop_timeout_ms * 1000);
                    //ACE_DEBUG((LM_DEBUG, "%T - %s - maximum = %u\n", _SRC, maximum));
                    ACE_OS::sleep(ACE_Time_Value(0, maximum));
                }
            }
        }

        return 0;
    }


    template <typename Object, typename Interface>
    bool register_with_naming_service(
        Object * obj, 
        const char * szObjectID, 
        const char * szRefName)
    {
        try
        {
            ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
            if (!guard.locked())
            {
                return false;
            }

            CORBA::ORB_var orb = this->orb_manager_.orb();

            // Activate object
            if (!this->orb_manager_.activate_under_child_poa(szObjectID, obj))
            {
                BasicEventHandling_ns::BasicEvent ev_activating_under_child_poa_failure(
                    _SRC,
                    ev_activating_under_child_poa_failure_id,
                    ev_activating_under_child_poa_failure_text);
                EVENT_HANDLER_SYSTEMLOG->handle_event(
                    _SRC,
                    ev_activating_under_child_poa_failure,
                    LM_ERROR,
                    szObjectID);
                return false;
            }

            // Register with naming service
            typename Interface::_var_type temp_ref = obj->_this();

            CosNaming::Name object_ref_name(1);
            object_ref_name.length (1);
            object_ref_name[0].id = CORBA::string_dup(szRefName);

            std::string service_id("NameService");
            try
            {
                // get a ref on naming service
                CosNaming::NamingContext_var nc = resolve_init<CosNaming::NamingContext>(
                                                                 orb.in(), service_id.c_str());

                //Register the object with Naming Service
                nc->rebind(object_ref_name, temp_ref.in());
            }
            catch(...)
            {
                BasicEventHandling_ns::BasicEvent ev_resolving_failure(
                    _SRC,
                    ev_resolving_failure_id,
                    ev_resolving_failure_text);
                EVENT_HANDLER_SYSTEMLOG->handle_event(
                    _SRC,
                    ev_resolving_failure,
                    LM_ERROR,
                    service_id.c_str());
                return false;
            }

            vect_ref_name_ns_.push_back(szRefName);
        }
        catch(...)
        {
            BasicEventHandling_ns::BasicEvent ev_register_with_naming_service_exception(
                _SRC,
                ev_register_with_naming_service_exception_id,
                ev_register_with_naming_service_exception_text);
            EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
                ev_register_with_naming_service_exception,
                LM_ERROR,
                szRefName);
            return false;
        }

        return true;
    }


    template <typename Object, typename Interface>
    bool register_with_ior_table(
        Object * obj, 
        const char * szObjectID, 
        const char * szRefName)
    {
        try
        {
            ACE_Guard<ACE_Recursive_Thread_Mutex> guard(shutting_down_mutex_, 0);
            if (!guard.locked())
            {
                return false;
            }

            CORBA::ORB_var orb = this->orb_manager_.orb();

            // Activate object
            this->orb_manager_.activate_under_child_poa(szObjectID, obj);

            // Register with IORTable
            typename Interface::_var_type temp_ref = obj->_this();
            CORBA::String_var ior_str = orb->object_to_string(temp_ref.in());

            CORBA::Object_var tobj = orb->resolve_initial_references("IORTable");
            IORTable::Table_var table = IORTable::Table::_narrow(tobj.in());
            table->rebind(szRefName, ior_str.in());

            vect_ref_name_iortable_.push_back(szRefName);
        }
        catch(...)
        {
            BasicEventHandling_ns::BasicEvent register_with_ior_table_exception(
                _SRC,
                register_with_ior_table_exception_id,
                register_with_ior_table_exception_text);
            EVENT_HANDLER_SYSTEMLOG->handle_event(_SRC,
                register_with_ior_table_exception,
                LM_ERROR,
                szRefName);
            return false;
        }

        return true;
    }

protected:

    void init_QOS();
    void unregister_objects();
    void shutdown_i();

    CORBA::PolicyManager_var policy_manager_;
    CORBA::PolicyCurrent_var policy_current_;

    CtrlEnd * ctrl_end_;

    TAO_ORB_Manager orb_manager_;

    std::vector<std::string> vect_ref_name_ns_;
    std::vector<std::string> vect_ref_name_iortable_;

    bool init_;
    int connection_timeout_orb_ms_;
    int request_timeout_orb_ms_;
    int request_timeout_thread_ms_;
    int taolayer_retry_loop_timeout_ms_;
    int taolayer_retry_attempts_;
    ACE_Recursive_Thread_Mutex shutting_down_mutex_;
    ACE_Recursive_Thread_Mutex set_tao_layer_params_mutex_;
};

#include "ACEWin64Warnings/EndSuppressWarnings.h"

#endif // CPM_TAOLAYER_H
