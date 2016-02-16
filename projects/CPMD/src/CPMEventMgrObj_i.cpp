
#include "CPMD/CPMEventMgrObj_i.h"
#include "CPMD/CPMEventMgr.evc.h"
#include "CPMD/version.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMSettingsCatch.h"
#include "libconfig/libconfig.h++"
#include "EnvVarExpand/EnvVarExpand.h"
#include "IBasicEventCorba/IBasicEventCorbaC.h"
#include "FileShortcut/FileShortcut.h"

#include <assert.h>
#include <sstream>

using namespace libconfig;

CPMEventMgrObj_i::CPMEventMgrObj_i()
{
}

CPMEventMgrObj_i::~CPMEventMgrObj_i()
{
    ACE_Message_Block *mb = 0;
    ACE_NEW_NORETURN(mb, ACE_Message_Block(0, ACE_Message_Block::MB_EVENT));
    if (mb != 0)
    {
        mb->msg_type(TERMINATE);
        if (this->msg_queue()->message_bytes() + mb->total_size() >= this->msg_queue()->high_water_mark())
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC, 
                CPMEventMgrEvents::ev_signal_rejected, 
                LM_INFO,
                "TERMINATE");
            mb->release();
        }
        else
        {
            putq(mb);
            wait();
        }
    }
    else
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC, 
            CPMEventMgrEvents::ev_new_ace_message_block_failure, 
            LM_CRITICAL);
    }
}

void CPMEventMgrObj_i::get_version (
    ::CORBA::Long_out major,
    ::CORBA::Long_out minor,
    ::CORBA::Long_out patch)
{
    major = CPMD_MAJOR;
    minor = CPMD_MINOR;
    patch = CPMD_PATCH;
}

void CPMEventMgrObj_i::ctrl_ping (
    void)
{
}

void CPMEventMgrObj_i::push (
      const ::IBASICEVENTCORBA_module::BasicEventCorba & basic_event_corba)
{
    TAO_OutputCDR cdr_out;
    cdr_out << basic_event_corba;

    ACE_Message_Block * mb = cdr_out.begin()->clone();
    mb->msg_type(NEW_EVENT);
    if (this->msg_queue()->message_bytes() + mb->total_size() >= this->msg_queue()->high_water_mark())
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC, 
            CPMEventMgrEvents::ev_signal_rejected, 
            LM_INFO,
            "NEW_EVENT");
        mb->release();
    }
    else
    {
        putq(mb);
    }
}

bool CPMEventMgrObj_i::init_i ()
{
    if (!load_settings())
    {
        return false;
    }

    if (!open_log_file())
    {
        return false;
    }

    activate();

    return true;
}

bool CPMEventMgrObj_i::load_settings()
{
    std::string cfg_path;
    if (!CPMUtils_ns::CPMUtils::get_cpmm_settings_path(cfg_path))
    {
        return false;
    }

    std::string file = cfg_path + "/cpm_event_mgr.cfg";
    file = FileShortcut_ns::FileShortcut::resolve(file);

    CPMUtils_ns::CPMUtils::backslash_to_slash(file);

    try
    {
        Config cfg;

        // Load libconfig file
        cfg.readFile(file.c_str());

        // root
        Setting & root = cfg.getRoot();

        // file
        if (!cfg.lookupValue("file", file_))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC, 
                CPMUtils_ns::CPMUtils::settings_field_not_found(), 
                LM_ERROR,
                "file");
            return false;
        }
        std::string new_port_number;
        if (!EnvVarExpand_ns::EnvVarExpand::expand_env_variables(file_, file_))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC,
                CPMEventMgrEvents::ev_expand_env_var_failure,
                LM_ERROR,
                file_.c_str());
            return false;
        }

        // line size
        if (!cfg.lookupValue("line_size", line_size_))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC, 
                CPMUtils_ns::CPMUtils::settings_field_not_found(), 
                LM_ERROR,
                "line_size");
            return false;
        }

        // file size
        if (!cfg.lookupValue("file_size", file_size_))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC, 
                CPMUtils_ns::CPMUtils::settings_field_not_found(), 
                LM_ERROR,
                "file_size");
            return false;
        }

        // flush
        if (!cfg.lookupValue("flush", flush_))
        {
            EVENT_HANDLER_SYSTEMLOG->handle_event(
                _SRC, 
                CPMUtils_ns::CPMUtils::settings_field_not_found(), 
                LM_ERROR,
                "flush");
            return false;
        }
    }
    CPM_SETTINGS_CATCH(file)

    return true;
}

bool CPMEventMgrObj_i::open_log_file()
{
    CPMUtils_ns::CPMUtils::backslash_to_slash(file_);

    if (!log_.Init(file_, flush_, line_size_, file_size_))
    {
        EVENT_HANDLER_SYSTEMLOG->handle_event(
            _SRC,
            CPMEventMgrEvents::ev_log_file_initialization_failure,
            LM_ERROR,
            file_.c_str());
        return false;
    }

    return true;
}

int CPMEventMgrObj_i::svc()
{
    bool stop(false);
    while (!stop)
    {
        ACE_Message_Block *mb = 0;
        if (this->getq(mb) != -1)
        {
            int type = mb->msg_type();
            switch(mb->msg_type())
            {
                case NEW_EVENT:
                    {
                        ::IBASICEVENTCORBA_module::BasicEventCorba ev;

                        TAO_InputCDR cdr_in(mb);
                        cdr_in >> ev;

                        std::string text;
                        vsprintf(text, ev);

                        if (!log_.Add(ev.source_id_.operator const CORBA::Char *(),
                                    ev.severity_,
                                    ev.event_id_,
                                    text.c_str()))
                        {
                            EVENT_HANDLER_SYSTEMLOG->handle_event(
                                _SRC, 
                                CPMEventMgrEvents::ev_logging_failure, 
                                LM_ERROR,
                                text.c_str());
                        }

                        // TO_DO...
                        //if (current_node != cpmm_node)
                        //{
                            // send ev to CPMEventMgrObj of cpmm_node
                            //if (!send_event_to_master())
                            //{
                            //    log...
                            //}
                        //}
                    }
                    break;

                case TERMINATE:
                    stop = true;
                    break;
            }

            mb->release();
        }
    }

    return 0;
}
