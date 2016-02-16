
#include "CPMCore/CPMAppLayer.h"
#include "CPMCore/CPMUtils.h"
#include "ace/INET_Addr.h"

namespace CPMAppLayer_ns
{
    CPMAppLayerBase * CPMAppLayer::app_layer_ = 0;

    //***** CPMAppLayerBase
    CPMAppLayerBase::CPMAppLayerBase()
    : obj_impl_(this)
    , console_(0)
    , cpm_frame_(0)
    , as_gui_(false)
    , as_server_(false)
    {
        //CPMTRACER->trace("%s", _SRC);
    }

    CPMAppLayerBase::~CPMAppLayerBase()
    {
        //CPMTRACER->trace("%s", _SRC);
        if (console_)
        {
            delete console_;
        }
    }

    void CPMAppLayerBase::cpmo_id(const std::string & cpmo_id)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
        cpmo_id_ = cpmo_id;
    }

    std::string CPMAppLayerBase::cpmo_id()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
        return cpmo_id_;
    }

    void CPMAppLayerBase::tao_layer(CPMTAOLayer * layer)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
        tao_layer_ = layer;
    }

    CPMTAOLayer * CPMAppLayerBase::tao_layer()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
        return tao_layer_;
    }

    void CPMAppLayerBase::ctrl_end()
    {
        terminating();

        //CPMTRACER->trace("%s - terminate_()...", _SRC);
        terminate_();
        //CPMTRACER->trace("%s - shutdown()...", _SRC);
        tao_layer()->shutdown();
        //CPMTRACER->trace("%s - after shutdown().", _SRC);

        if (cpm_frame_)
        {
            cpm_frame_->close();
        }
    }

    void CPMAppLayerBase::ctrl_ping()
    {
    }

    void CPMAppLayerBase::show_console(::CORBA::Boolean show)
    {
        if (!console_)
        {
            console_ = new DPOConsole_ns::DPOConsole(cpmo_id_, 120, 40);
        }

        console_->show(show);
    }

    void CPMAppLayerBase::show_gui(::CORBA::Boolean show)
    {
        if (cpm_frame_)
        {
            ACE_DEBUG((LM_DEBUG, "CPMAppLayerBase::show_gui(%s)\n", show ? "true" : "false"));
            as_gui(show);
            cpm_frame_->show(show);
        }
    }

    void CPMAppLayerBase::as_server(::CORBA::Boolean flag)
    {
        as_server_ = flag;
    }

    void CPMAppLayerBase::as_gui(::CORBA::Boolean flag)
    {
        as_gui_ = flag;
    }

    void CPMAppLayerBase::show_console_on_init()
    {
        ICPM_module::CPMMSettings cpmm_settings;
        if (CPMUtils_ns::CPMUtils::get_settings_from_outproc_cpmm(cpmm_settings))
        {
            for (int i = 0; i != cpmm_settings.nodes.length(); ++i)
            {
                char current_node_name[MAXNAMELEN];
                ACE_OS::hostname(current_node_name, MAXNAMELEN);

                ACE_INET_Addr local_addr((std::string(current_node_name) + ":").c_str());
                std::string current_node_ip = local_addr.get_host_addr();

                ICPM_module::NodeDef node_def = cpmm_settings.nodes[i];
                std::string node_def_id = node_def.node_id.operator const CORBA::Char *();

                if (node_def_id == current_node_ip
                    || node_def_id == current_node_name)
                {
                    for (int j = 0; j != cpmm_settings.nodes[i].processes.length(); ++j)
                    {
                        ICPM_module::ProcessDef process_def = cpmm_settings.nodes[i].processes[j];
                        std::string process_id = process_def.process_id.operator const CORBA::Char *();
                        if (process_id == cpmo_id())
                        {
                            if (std::string(process_def.console.operator const CORBA::Char *()) == "show")
                            {
                                show_console(true);
                            }
                            else
                            {
                                show_console(false);
                            }
                        }
                    }
                }
            }
        }
    }

    void CPMAppLayerBase::object_list(::ICPM_module::ObjectDescSeq_out list)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);

        size_t size = objects_ids_.size();

        list = new ::ICPM_module::ObjectDescSeq;

        list->length(CORBA::ULong(size));

        CORBA::ULong i(0);
        for (i = 0; i != size; i++)
        {
            (*list)[i].object_id = objects_ids_[i].c_str();
        }
    }

    //***** CPMAppLayer
    CPMAppLayer::CPMAppLayer()
    {
    }

    CPMAppLayer::~CPMAppLayer()
    {
        //ACE_DEBUG((LM_DEBUG, "%n - ~CPMAppLayer()\n")); // to test ACE_Singleton<CPMAppLayer_ns::CPMAppLayer, ACE_Recursive_Thread_Mutex>::close();
    }

    void CPMAppLayer::set(CPMAppLayerBase * app_layer)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
        app_layer_ = app_layer;
    }

    CPMAppLayerBase * CPMAppLayer::get()
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);
        return app_layer_;
    }

    //void CPMAppLayer::destroy()
    //{
    //    // Defining destroy this way doesn't work. So, for those who are tempted to, don't do that!
    //    //ACE_Singleton<CPMAppLayer_ns::CPMAppLayer, ACE_Recursive_Thread_Mutex>::close();
    //}

} // CPMAppLayer_ns namespace
