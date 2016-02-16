
#include "CPMCore/CPMEventRaiser.h"
#include "CPMEventHandlerAbstract/CPMEventHandlerAbstract.h"
#include "CPMCore/get_plugin.h"
#include "CPMCore/CPMEventRaiser.evc.h"
#include "ace/DLL_Manager.h"

using namespace CPMEventHandlerAbstract_ns;

namespace CPMEventRaiser_ns
{
    bool CPMEventRaiser::initialized_(false);

    CPMEventRaiser::CPMEventRaiser()
    : event_handler_dll_(false)
    , event_handler_plugin_(0)
    {
        if (!init_plugins())
        {
            throw 0;
        }
    }

    CPMEventRaiser::~CPMEventRaiser()
    {
        //ACE_DEBUG((LM_DEBUG, "%n - CPMEventRaiser::~CPMEventRaiser()\n")); // to test ACE_Singleton<CPMEventRaiser_ns::CPMEventRaiser, ACE_Recursive_Thread_Mutex>::close();

        delete event_handler_plugin_;
    }

    //void CPMEventRaiser::destroy()
    //{
    //    // Defining destroy this way doesn't work. So, for those who are tempted to, don't do that!
    //    //ACE_Singleton<CPMEventRaiser_ns::CPMEventRaiser, ACE_Recursive_Thread_Mutex>::close();
    //}

    bool CPMEventRaiser::init_plugins()
    {
        char * pPluginId = getenv ("CPM_EVENT_HANDLER_PLUGIN_ID");
        if (!pPluginId)
        {
            raise_event(_SRC,
                        CPMEventRaiserEvents::ev_cpm_event_handler_plugin_id_env_var_not_defined,
                        LM_ERROR);
            return false;
        }
        std::string cpm_event_handler_plugin_id(pPluginId);

        std::string dll_prefix(ACE_DLL_PREFIX);

        std::string plugin_id = dll_prefix + cpm_event_handler_plugin_id;

#       if defined(ACE_WIN32) && defined(_DEBUG)
            plugin_id += "d";
#       endif

        // Event handler
        int rc = event_handler_dll_.open (plugin_id.c_str());
        if (rc != 0)
        {
            raise_event(_SRC,
                        CPMEventRaiserEvents::ev_event_handler_plugin_library_loading_failure,
                        LM_ERROR,
                        plugin_id.c_str());
            return false;
        }

        typedef CPMEventHandlerAbstract * (*CPMEventHandlerAbstract_creator) (void);
        if (!get_plugin<CPMEventHandlerAbstract_creator, CPMEventHandlerAbstract>(event_handler_dll_, "create_CPMEventHandlerAbstract", &event_handler_plugin_))
        {
            raise_event(_SRC,
                        CPMEventRaiserEvents::ev_event_handler_object_creation_failure,
                        LM_ERROR,
                        plugin_id.c_str());
            return false;
        }

        if (event_handler_plugin_ == 0)
        {
            return false;
        }

        if (!event_handler_plugin_->init())
        {
            raise_event(_SRC,
                        CPMEventRaiserEvents::ev_init_plugin_failure,
                        LM_ERROR,
                        plugin_id.c_str());
            return false;
        }

        this->set_event_handler(event_handler_plugin_);

        initialized_ = true;

        return true;
    }

    bool CPMEventRaiser::initialized()
    {
        return initialized_;
    }

} // CPMEventRaiser_ns namespace
