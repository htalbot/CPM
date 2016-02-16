#include "CPMTraceEraser/CPMTraceEraserObj.h"
#include "CPMTraceEraser/CPMTraceEraserObj.evc.h"
#include "CPMCore/CPMEventRaiser.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMSettingsCatch.h"
#include "ace/Log_Msg.h"
#include "ace/Dirent.h"
#include "ace/FILE.h"
#include "libConfig/libconfig.h++"
#include "FileShortcut/FileShortcut.h"

#include <time.h>

using namespace libconfig;

CPMTraceEraserObj::CPMTraceEraserObj()
: terminate_(false)
{
}

bool CPMTraceEraserObj::init()
{
    char * pCPM_PATH = getenv("CPM_PATH");
    if (!pCPM_PATH)
    {
        RAISE_EVENT(
            CPMTraceEraserObjEvents::ev_env_var_load_failure,
            LM_ERROR,
            "CPM_PATH");
        return false;
    }

    traces_directory_ = pCPM_PATH;
    CPMUtils_ns::CPMUtils::backslash_to_slash(traces_directory_);

    if (!load_settings())
    {
        return false;
    }

    activate();
    return true;
}

bool CPMTraceEraserObj::load_settings()
{
    char * pCPM_etc = getenv("CPM_ETC");
    if (!pCPM_etc)
    {
        RAISE_EVENT(
            CPMTraceEraserObjEvents::ev_env_var_load_failure,
            LM_ERROR,
            "CPM_ETC");
        return false;
    }

    std::string file = std::string(pCPM_etc) + "/cpm_trace_eraser.cfg";
    file = FileShortcut_ns::FileShortcut::resolve(file);

    Config cfg;

    // Load config
    try
    {
        cfg.readFile(file.c_str());

        if (!cfg.lookupValue("frequency_ms", frequency_ms_))
        {
            RAISE_EVENT(
                CPMTraceEraserObjEvents::ev_libconfig_setting_field_not_found,
                LM_ERROR,
                "frequency_ms");
            return false;
        }

        if (!cfg.lookupValue("time_to_keep_ms", time_to_keep_ms_))
        {
            RAISE_EVENT(
                CPMTraceEraserObjEvents::ev_libconfig_setting_field_not_found,
                LM_ERROR,
                "time_to_keep_ms");
            return false;
        }
    }
    CPM_SETTINGS_CATCH(file)

    return true;
}

int CPMTraceEraserObj::svc()
{
    while (!terminate_.value())
    {
        ACE_DEBUG((LM_DEBUG, "%T - CPMTraceEraserObj::svc() - ????\n"));

        ACE_Dirent dir;
        if (dir.open (ACE_TEXT (traces_directory_.c_str())) == -1)
        {
            RAISE_EVENT(
                CPMTraceEraserObjEvents::ev_getting_dir_content_failure,
                LM_ERROR,
                traces_directory_.c_str());

            ACE_OS::sleep(ACE_Time_Value(0, frequency_ms_ * 1000));

            continue;
        }

        std::vector<TraceFile> trace_files;

        int i(0);
        for (ACE_DIRENT *directory = 0;
                (directory = dir.read ()) != 0;
                i++)
        {
            if (ACE::isdotdir(directory->d_name))
            {
                continue;
            }

            std::string complete = traces_directory_ + "/" + directory->d_name;

            ACE_stat stat_buf;
            if (ACE_OS::lstat (complete.c_str(), &stat_buf) == -1)
            {
                RAISE_EVENT(
                    CPMTraceEraserObjEvents::ev_getting_file_status_failure,
                    LM_ERROR,
                    traces_directory_.c_str(),
                    directory->d_name);
                break;
            }

            switch (stat_buf.st_mode & S_IFMT)
            {
            case S_IFREG: // Either a regular file or an executable.
                {
                    std::string file = directory->d_name;
                    if (file.rfind(".trace") != std::string::npos)
                    {
                        ACE_DEBUG((LM_DEBUG, "File to delete: %s\n", file.c_str()));
                        TraceFile f;
                        f.file_name_ = complete;
                        f.file_time_ = stat_buf.st_mtime;
                        trace_files.push_back(f);
                    }
                }

            default: // Must be a file
                break;
            }
        }

        bool first(true);
        time_t most_recent;
        for (auto it = trace_files.begin(); it != trace_files.end(); it++)
        {
            if (first)
            {
                most_recent = it->file_time_;
                first = false;
            }
            else
            {
                if (difftime(most_recent, it->file_time_) < 0)
                {
                    most_recent = it->file_time_;
                }
            }
        }

        for (auto it = trace_files.begin(); it != trace_files.end(); it++)
        {
            double diff = difftime(most_recent, it->file_time_);

            ACE_DEBUG((LM_DEBUG, "Time to delete: %s (%f)\n", it->file_name_.c_str(), diff));

            if (difftime(most_recent, it->file_time_) * 1000 > time_to_keep_ms_)
            {
                ACE_DEBUG((LM_DEBUG, "remove '%s'\n", it->file_name_.c_str()));

#               ifdef ACE_WIN32
                    DeleteFile(it->file_name_.c_str());
#               else
                    ACE_FILE_Addr file_to_rm(complete.c_str());
                    // TO_DO
#               endif
            }
        }

        ACE_OS::sleep(ACE_Time_Value(0, frequency_ms_ * 1000));
    }

    return 0;
}

void CPMTraceEraserObj::terminate()
{
    terminate_ = true;
}
