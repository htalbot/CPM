
#ifndef __CPMUTILS_H__
#define __CPMUTILS_H__

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ICPM/ICPMC.h"
#include "ace/Date_Time.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMCore/CPMCORE_Export.h"
#include "BasicEventHandling/BasicEventHandling.h"

#include <string>
#include <list>
#include <vector>
#include <iomanip>

namespace CPMUtils_ns
{
    class CPMCORE_Export CPMUtils
    {
    public:
        static std::string get_local_addr();

        static bool file_exists(
            const std::string & file);

        static bool get_cpmm_settings_path(
            std::string & cfg_path);

        static bool get_cpmd_port(
            std::string & port);

        static bool get_cpmm_port(
            std::string & port);

        static bool get_default_cpmm_settings_file(
            std::string & default_settings_file);

        static bool set_as_default_settings(
            const std::string & file);

        static bool get_cpmm_settings_from_file(
            const std::string & file,
            ::ICPM_module::CPMMSettings & cpmm_settings);

        static bool save_cpmm_settings(
            const ::ICPM_module::CPMMSettings & cpmm_settings,
            const std::string & file);

        static bool get_cpmm_node(
            std::string & node);

        static bool get_ref_on_cpmm(
            ICPM_module::ICPMM_out cpmm_obj);

        static void backslash_to_slash(
            std::string &);

        static bool compare_names(
            const std::string & name1,
            const std::string & name2);

        static bool find_process(
            const std::string & process_name,
            const std::string & path,
            std::string & launch_path);

        static bool ping(
            const std::string & node);

        static bool get_object_location(
            const ICPM_module::ProcessDef & def,
            std::string & location);

        static bool get_node_from_settings(
            const ::ICPM_module::CPMMSettings & settings,
            const std::string & node_id,
            ICPM_module::NodeDef &);

        static bool get_process_from_settings(
            const ::ICPM_module::CPMMSettings & settings,
            const std::string & node_id,
            const std::string & process_id,
            ICPM_module::ProcessDef & proc);

        static bool get_settings_from_outproc_cpmm(
            ::ICPM_module::CPMMSettings & settings);

        static bool get_system_processes_list(
            std::list<std::string> &);

        static bool is_process_running(
            const std::string & proc_name);

        static std::string proc_extention()
        {
            std::string ext;
#ifdef ACE_WIN32
            ext = ".exe";
#endif
            return ext;
        }

        #ifdef ACE_WIN32
            static bool get_process_name(
                DWORD processID,
                std::string & process_name);
        #endif

        static bool load_cpm_params(
            bool & show_cpmm_console,
            bool & show_cpmd_console,
            bool & overwrite_autostart_to_false);

        static bool save_cpm_params(
            bool show_cpmm_console,
            bool show_cpmd_console,
            bool overwrite_autostart_to_false);

        static bool boot_cpmd();

        static bool boot_cpmd(
            bool show_cpmd_console);

        static bool boot_cpm(
            bool use_these_params = false,
            bool show_cpmm_console = false,
            bool show_cpmd_console = false,
            bool overwrite_autostart_to_false = false);

        static bool shutdown_cpmd(); // To be called by some kind of service only

        static bool shutdown_cpmm();

        static bool shutdown_cpm();

        static std::string cpmm_name();

        static std::string cpmd_name();

        static BasicEventHandling_ns::BasicEvent CPMUtils::settings_field_not_found();

        static void report_settings_failure(const std::string & msg);
    };


    class CPMCORE_Export CPMTracer
    {
        friend ACE_Singleton<CPMUtils_ns::CPMTracer, ACE_Recursive_Thread_Mutex>;

    public:
        inline void trace(
            const std::string & format, ...)
        {
            ACE_Guard<ACE_Recursive_Thread_Mutex> guard(mutex_);

            ACE_Date_Time d;
            std::stringstream dd;
            dd << std::setfill('0') << std::setw(4);
            dd << d.year();
            dd << "-";
            dd << std::setfill('0') << std::setw(2);
            dd << d.month();
            dd << "-";
            dd << std::setfill('0') << std::setw(2);
            dd << d.day();
            dd << "_";
            dd << std::setfill('0') << std::setw(2);
            dd << d.hour();
            dd << ".";
            dd << std::setfill('0') << std::setw(2);
            dd << d.minute();
            dd << ".";
            dd << std::setfill('0') << std::setw(2);
            dd << d.second();
            dd << ".";
            dd << std::setfill('0') << std::setw(3);
            dd << d.microsec() / 1000;

	        va_list args;
	        va_start(args, format);

            if (pf_ == 0)
            {
                char * pCPMVar = getenv ("CPM_PATH");

                char szModuleName[2048];

#               ifdef ACE_WIN32
                    GetModuleFileNameA(NULL, szModuleName, 2048);
#               else
                    // TO_DO
#               endif

                std::string module_name = szModuleName;

                CPMUtils_ns::CPMUtils::backslash_to_slash(module_name);

                size_t pos = module_name.rfind('/');
                module_name = module_name.substr(pos);

                std::stringstream ss;

                ss << pCPMVar;
                ss << "/";
                ss << module_name;
                ss << "-";
                ss << std::setfill('0') << std::setw(4);
                ss << dd.str();
                ss << ".trace";

                std::string s = ss.str();

                CPMUtils_ns::CPMUtils::backslash_to_slash(s);

                pf_ = fopen(ss.str().c_str(), "wt");
            }

            vfprintf(pf_, (dd.str() + " - " + format + "\n").c_str(), args);
            fflush(pf_);
        }

    protected:
        CPMTracer()
        {
            pf_ = 0;
        }
        ~CPMTracer()
        {
        }

        ACE_Recursive_Thread_Mutex mutex_;
        FILE * pf_;
    };

    class NodeProcess_map_NodePort
    {
    public:
        CPMCORE_Export bool prepare(
            const ICPM_module::CPMMSettings & settings);

        CPMCORE_Export bool get_node(
            const std::string & node_sensor, std::string & node);

        CPMCORE_Export bool get_port(
            const std::string & node_sensor, std::string & port);

        CPMCORE_Export bool get_corbaloc_node_port(
            const std::string & node_sensor, std::string & corbaloc_node_port);

    protected:
        std::map<std::string, std::string> map_;
    };


    enum enumEnvVarType {
        Custom,
        Env
    };

} // CPMUtils_ns namespace

    typedef ACE_Singleton<CPMUtils_ns::CPMTracer, ACE_Recursive_Thread_Mutex> CPMTracerSingleton;
#   ifdef ACE_WIN32
#       pragma warning(push)
#       pragma warning(disable: 4231) // will disapear when VC is conform to C++11
#   endif
    CPMCORE_SINGLETON_DECLARE(ACE_Singleton, CPMUtils_ns::CPMTracer, ACE_Recursive_Thread_Mutex);
#   ifdef ACE_WIN32
#       pragma warning(push)
#   endif
#   define CPMTRACER CPMTracerSingleton::instance()

// Because CPMTRACER writes in a file only, it's not considered
// necessary to declare a kind of INIT_CPMTRACER.

#endif // __CPMUTILS_H__
