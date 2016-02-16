
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/INET_Addr.h"
#include "ace/Dirent.h"
#include "ace/Ping_Socket.h"
#include "ace/Process.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "libConfig/libconfig.h++"

#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMUtils.evc.h"
#include "CPMCore/CPMSettingsCatch.h"

#include "CPMCore/CPMAppLayer.h"
#include "CPMCore/CPMEventRaiser.h"
#include "EnvVarExpand/EnvVarExpand.h"
#include "FileShortcut/FileShortcut.h"

using namespace libconfig;

#include <locale>
#include <fstream>
#include <sstream>

#ifdef ACE_WIN32
#   include <Psapi.h>
#   include <tlhelp32.h>
#endif

// To debug CPMD/CPMM (CPMDd/CPMMd) in the context of boot_cpm, we change these two.
const std::string cpmd_module_name = "CPMD";
const std::string cpmm_module_name = "CPMM";


ACE_Recursive_Thread_Mutex g_ping_mutex;

namespace CPMUtils_ns
{
    std::string to_upper(
        const std::string & s)
    {
        std::string result;
        std::locale loc;
        for (unsigned int i = 0; i < s.length(); i++)
        {
            result += std::toupper(s.at(i), loc);
        }

        return result;
    }

    std::string to_lower(
        const std::string & s)
    {
        std::string result;
        std::locale loc;
        for (unsigned int i = 0; i < s.length(); i++)
        {
            result += std::tolower(s.at(i), loc);
        }

        return result;
    }

    bool CPMUtils::file_exists(
        const std::string & file)
    {
        std::fstream f(file);
        if (f)
        {
            return true;
        }

        return false;
    }

    std::string CPMUtils::get_local_addr()
    {
        char node[MAXNAMELEN];
        ACE_OS::hostname(node, MAXNAMELEN);
        ACE_INET_Addr local_addr((std::string(node) + ":").c_str());
        return local_addr.get_host_addr();
    }

    bool CPMUtils::get_cpmm_settings_path(
        std::string & cfg_path)
    {
        // Get the working directory for CPMM
        char * pPath = getenv ("CPM_ETC");
        if (!pPath)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                "CPM_ETC");
            return false;
        }

        std::string path(pPath);
        backslash_to_slash(path);

        cfg_path = path;

        return true;
    }

    bool CPMUtils::get_cpmd_port(
        std::string & port)
    {
        char * pPort = getenv ("CPMD_PORT");
        if (!pPort)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                "CPMD_PORT");
            return false;
        }

        port = pPort;

        return true;
    }

    bool CPMUtils::get_cpmm_port(
        std::string & port)
    {
        char * pPort = getenv ("CPMM_PORT");
        if (!pPort)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                "CPMM_PORT");
            return false;
        }

        port = pPort;

        return true;
    }

    bool CPMUtils::get_default_cpmm_settings_file(
        std::string & default_cfg)
    {
        std::string cfg_path;
        if (!get_cpmm_settings_path(cfg_path))
        {
            return false;
        }

        std::string file = cfg_path + "/cpmm_cfg_default";
        file = FileShortcut_ns::FileShortcut::resolve(file);

        Config cfg;

        // Load config
        try
        {
            cfg.readFile(file.c_str());
            default_cfg = cfg_path + "/" + cfg.lookup("Default").c_str();
        }
        CPM_SETTINGS_CATCH(file)

        return true;
    }

    bool CPMUtils::set_as_default_settings(
        const std::string & new_default)
    {
        std::string cfg_path;
        if (!get_cpmm_settings_path(cfg_path))
        {
            return false;
        }

        std::string file = cfg_path + "/cpmm_cfg_default";
        file = FileShortcut_ns::FileShortcut::resolve(file);

        Config cfg;

        libconfig::Config cfg_default;

        try
        {
            cfg_default.readFile(file.c_str());

            libconfig::Setting & default_value = cfg_default.lookup("Default");
            default_value = new_default.c_str();

            cfg_default.writeFile(file.c_str());
        }
        CPM_SETTINGS_CATCH(file)

        return true;
    }

    bool CPMUtils::get_cpmm_settings_from_file(
        const std::string & file_in,
        ::ICPM_module::CPMMSettings & cpmm_settings)
    {
        std::string file = FileShortcut_ns::FileShortcut::resolve(file_in);

        std::stringstream cfg_field_id;

        Config cfg;

        // Load config
        try
        {
            cfg.readFile(file.c_str());

            // root
            Setting & root = cfg.getRoot();

            // settings_id
            std::string settings_id;
            if (!root.lookupValue("settings_id", settings_id))
            {
                RAISE_EVENT(
                    CPMUtils::settings_field_not_found(),
                    LM_ERROR,
                    "settings_id");
                return false;
            }
            cpmm_settings.settings_id = settings_id.c_str();

            // startup
            Setting & startup = root["startup"];

            // auto_launch_processes
            if (!startup.lookupValue("auto_launch_processes", cpmm_settings.startup.auto_launch_processes))
            {
                RAISE_EVENT(
                    CPMUtils::settings_field_not_found(),
                    LM_ERROR,
                    "startup/auto_launch_processes");
                return false;
            }

            // required_time_before_stop_since_start_ms
            if (!startup.lookupValue("required_time_before_stop_since_start_ms", cpmm_settings.startup.required_time_before_stop_since_start_ms))
            {
                RAISE_EVENT(
                    CPMUtils::settings_field_not_found(),
                    LM_ERROR,
                    "startup/required_time_before_stop_since_start_ms");
                return false;
            }

            // nodes
            cfg_field_id << "nodes";
            Setting & nodes = root["nodes"];
            cpmm_settings.nodes.length(nodes.getLength());
            for (int i = 0; i != nodes.getLength(); i++)
            {
                std::stringstream node_field_id;
                node_field_id << cfg_field_id.str();

                node_field_id << "(" << i << ")";

                Setting & node = nodes[i];

                // node_id
                std::string node_id;
                if (!node.lookupValue("node_id", node_id))
                {
                    node_field_id << "/node_id";
                    RAISE_EVENT(
                        CPMUtils::settings_field_not_found(),
                        LM_ERROR,
                        node_field_id.str().c_str());
                    return false;
                }
                std::string node_id_origin(node_id);
                if (!EnvVarExpand_ns::EnvVarExpand::expand_env_variables(node_id, node_id))
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_expand_env_var_failure,
                        LM_ERROR,
                        node_id.c_str());
                    return false;
                }
                cpmm_settings.nodes[i].node_id = node_id.c_str();
                if (node_id_origin != node_id)
                {
                    cpmm_settings.nodes[i].env_var = node_id_origin.c_str();
                }

                // enabled
                bool enabled;
                if (!node.lookupValue("enabled", enabled))
                {
                    node_field_id << "/enabled";
                    RAISE_EVENT(
                        CPMUtils::settings_field_not_found(),
                        LM_ERROR,
                        node_field_id.str().c_str());
                    return false;
                }
                cpmm_settings.nodes[i].enabled = enabled;

                // processes
                Setting & processes = node["processes"];
                cpmm_settings.nodes[i].processes.length(processes.getLength());
                for (int j = 0; j != processes.getLength(); j++)
                {
                    std::stringstream procs_field_id;
                    procs_field_id << node_field_id.str();

                    procs_field_id << "/processes(" << i << ")";

                    Setting & process = processes[j];

                    cpmm_settings.nodes[i].processes[j].node_id = node_id.c_str();

                    // process_id
                    std::string process_id;
                    if (!process.lookupValue("process_id", process_id))
                    {
                        procs_field_id << "/process_id";
                        RAISE_EVENT(
                            CPMUtils::settings_field_not_found(),
                            LM_ERROR,
                            procs_field_id.str().c_str());
                        return false;
                    }
                    cpmm_settings.nodes[i].processes[j].process_id = process_id.c_str();

                    // env_var_id
                    std::string env_var_id;
                    if (!process.lookupValue("env_var_id", env_var_id))
                    {
                        procs_field_id << "/env_var_id";
                        RAISE_EVENT(
                            CPMUtils::settings_field_not_found(),
                            LM_ERROR,
                            procs_field_id.str().c_str());
                        return false;
                    }
                    cpmm_settings.nodes[i].processes[j].env_var_id = env_var_id.c_str();

                    // module_name
                    std::string module_name;
                    if (!process.lookupValue("module_name", module_name))
                    {
                        procs_field_id << "/module_name";
                        RAISE_EVENT(
                            CPMUtils::settings_field_not_found(),
                            LM_ERROR,
                            procs_field_id.str().c_str());
                        return false;
                    }
                    cpmm_settings.nodes[i].processes[j].module_name = module_name.c_str();

                    // port_number
                    std::string port_number;
                    if (!process.lookupValue("port_number", port_number))
                    {
                        procs_field_id << "/port_number";
                        RAISE_EVENT(
                            CPMUtils::settings_field_not_found(),
                            LM_ERROR,
                            procs_field_id.str().c_str());
                        return false;
                    }
                    cpmm_settings.nodes[i].processes[j].port_number = port_number.c_str();

                    // enabled (process)
                    bool enabled;
                    if (!process.lookupValue("enabled", enabled))
                    {
                        procs_field_id << "/enabled";
                        RAISE_EVENT(
                            CPMUtils::settings_field_not_found(),
                            LM_ERROR,
                            procs_field_id.str().c_str());
                        return false;
                    }
                    cpmm_settings.nodes[i].processes[j].enabled = enabled;

                    // console
                    std::string console;
                    if (!process.lookupValue("console", console))
                    {
                        procs_field_id << "/console";
                        RAISE_EVENT(
                            CPMUtils::settings_field_not_found(),
                            LM_ERROR,
                            procs_field_id.str().c_str());
                        return false;
                    }
                    if (console != "hide"
                        && console != "show"
                        && console != "cpmd")
                    {
                        std::string s = procs_field_id.str();
                        RAISE_EVENT(
                            CPMUtilsEvents::ev_bad_console_value,
                            LM_ERROR,
                            console.c_str(),
                            s.c_str());
                        return false;
                    }
                    cpmm_settings.nodes[i].processes[j].console = console.c_str();
                }
            }
        }
        CPM_SETTINGS_CATCH(file)

        return true;
    }

    bool CPMUtils::save_cpmm_settings(
        const ::ICPM_module::CPMMSettings & cpmm_settings,
        const std::string & file_in)
    {
        std::string file = FileShortcut_ns::FileShortcut::resolve(file_in);

        //ACE_DEBUG((LM_DEBUG, "CPMUtils::save_cpmm_settings to %s\n", file.c_str()));
        try
        {
            Config cfg;

            Setting &root = cfg.getRoot();

            root.add("settings_id", Setting::TypeString) = cpmm_settings.settings_id.operator const CORBA::Char *();

            Setting &startup = root.add("startup", Setting::TypeGroup);
            startup.add("auto_launch_processes", Setting::TypeBoolean) = cpmm_settings.startup.auto_launch_processes;
            startup.add("required_time_before_stop_since_start_ms", Setting::TypeInt) = cpmm_settings.startup.required_time_before_stop_since_start_ms;

            Setting &nodes_cfg = root.add("nodes", Setting::TypeList);

            for (::CORBA::ULong i = 0; i != cpmm_settings.nodes.length(); i++)
            {
                ICPM_module::NodeDef node = cpmm_settings.nodes[i];

                Setting &node_cfg = nodes_cfg.add(Setting::TypeGroup);
                if (std::string(node.env_var.operator const char *()).length() != 0)
                {
                    node_cfg.add("node_id", Setting::TypeString) = node.env_var.operator const CORBA::Char *();
                }
                else
                {
                    node_cfg.add("node_id", Setting::TypeString) = node.node_id.operator const CORBA::Char *();
                }

                Setting &processes = node_cfg.add("processes", Setting::TypeList);
                for (::CORBA::ULong j = 0; j != node.processes.length(); j++)
                {
                    ICPM_module::ProcessDef proc = node.processes[j];

                    Setting &proc_cfg = processes.add(Setting::TypeGroup);
                    proc_cfg.add("process_id", Setting::TypeString) = proc.process_id.operator const CORBA::Char *();
                    proc_cfg.add("env_var_id", Setting::TypeString) = proc.env_var_id.operator const CORBA::Char *();
                    proc_cfg.add("module_name", Setting::TypeString) = proc.module_name.operator const CORBA::Char *();
                    proc_cfg.add("port_number", Setting::TypeString) = proc.port_number.operator const CORBA::Char *();
                    proc_cfg.add("enabled", Setting::TypeBoolean) = proc.enabled;
                    proc_cfg.add("console", Setting::TypeString) = proc.console;
                }
                node_cfg.add("enabled", Setting::TypeBoolean) = node.enabled;
            }

            cfg.writeFile(file.c_str());
        }
        CPM_SETTINGS_CATCH(file)

        return true;
    }

    bool CPMUtils::get_cpmm_node(
        std::string & node)
    {
        char * pNode = getenv ("CPMM_NODE");
        if (!pNode)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                "CPMM_NODE");
            return false;
        }

        node = pNode;

        return true;
    }

    bool CPMUtils::get_ref_on_cpmm(
        ICPM_module::ICPMM_out cpmm_obj)
    {
        // Check for CPMM_PATH env. var.
        const std::string cmp_path("CPM_PATH");
        char * szPath = getenv(cmp_path.c_str());
        if (!szPath)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                cmp_path.c_str());
            return false;
        }

        std::string cpmm_port;
        if (!get_cpmm_port(cpmm_port))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_cpmm_port_failure,
                LM_ERROR);
            return false;
        }

        std::string cpmm_node;
        if (!get_cpmm_node(cpmm_node))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_cpmm_node_failure,
                LM_ERROR);
            return false;
        }

        std::string location = std::string("corbaloc:iiop:") + cpmm_node + ":" + cpmm_port + "/" + "CPMMObj";

        if (!EnvVarExpand_ns::EnvVarExpand::expand_env_variables(location, location))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                location.c_str());
            return false;
        }

        CPMTAOLayer::GetRefStatus status;
        cpmm_obj = CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMM>(location.c_str(), status);
        if(!cpmm_obj)
        {
            return false;
        }

        return true;
    }

    void CPMUtils::backslash_to_slash(
        std::string & s)
    {
        std::string::iterator it;
        for (it = s.begin(); it != s.end(); it++)
        {
            if (*it == '\\')
            {
                *it = '/';
            }
        }
    }

    bool CPMUtils::compare_names(
        const std::string & name1,
        const std::string & name2)
    {
        std::string name2b = name2;

        size_t found = name2.rfind(".exe");
        if (found != std::string::npos)
        {
            // On enlève l'extension
            std::string non_const_names2 = name2;
            name2b = non_const_names2.erase(found, 4);
        }

        if (to_upper(name1) == to_upper(name2b))
        {
            return true;
        }

        return false;
    }

    bool CPMUtils::find_process(
        const std::string & process_name,
        const std::string & path,
        std::string & launch_path)
    {
        char cwd[_MAX_PATH];
        if (!ACE_OS::getcwd(cwd, _MAX_PATH))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_cwd_failure,
                LM_ERROR);
            return false;
        }

        if (ACE_OS::chdir (path.c_str()) == -1)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_chdir_failure,
                LM_ERROR,
                path.c_str());

            if (ACE_OS::chdir (cwd) == -1)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_chdir_back_failure,
                    LM_ERROR,
                    cwd);
            }
            return false;
        }

        ACE_Dirent dir;
        if (dir.open (ACE_TEXT (".")) == -1)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_dir_content_failure,
                LM_ERROR,
                cwd);

            if (ACE_OS::chdir (cwd) == -1)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_chdir_back_failure,
                    LM_ERROR,
                    cwd);
            }
            return false;
        }

        int i(0);
        for (ACE_DIRENT *directory = 0;
                (directory = dir.read ()) != 0;
                i++)
        {
            if (ACE::isdotdir(directory->d_name))
            {
                continue;
            }

            ACE_stat stat_buf;
            if (ACE_OS::lstat (directory->d_name, &stat_buf) == -1)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_file_status_failure,
                    LM_ERROR,
                    path.c_str(),
                    directory->d_name);

                if (ACE_OS::chdir (cwd) == -1)
                {
                    RAISE_EVENT(
                        CPMUtilsEvents::ev_chdir_back_failure,
                        LM_ERROR,
                        cwd);
                }
                return false;
            }

            switch (stat_buf.st_mode & S_IFMT)
            {
            case S_IFREG: // Either a regular file or an executable.
                if (compare_names(process_name, directory->d_name))
                {
                    char the_path[_MAX_PATH];
                    if (!ACE_OS::getcwd(the_path, _MAX_PATH))
                    {
                        RAISE_EVENT(
                            CPMUtilsEvents::ev_cwd_failure,
                            LM_ERROR);

                        if (ACE_OS::chdir (cwd) == -1)
                        {
                            RAISE_EVENT(
                                CPMUtilsEvents::ev_chdir_back_failure,
                                LM_ERROR,
                                cwd);
                        }
                        return false;
                    }
                    launch_path = the_path;
                    backslash_to_slash(launch_path);
                    if (ACE_OS::chdir (cwd) == -1)
                    {
                        RAISE_EVENT(
                            CPMUtilsEvents::ev_chdir_back_failure,
                            LM_ERROR,
                            cwd);
                    }
                    return true;
                }
                break;

            case S_IFLNK: // Either a file or directory link, so let's find out.
                break;

            case S_IFDIR:
                if (find_process(process_name, directory->d_name, launch_path))
                {
                    if (ACE_OS::chdir (cwd) == -1)
                    {
                        RAISE_EVENT(
                            CPMUtilsEvents::ev_chdir_back_failure,
                            LM_ERROR,
                            cwd);
                    }
                    backslash_to_slash(launch_path);
                    return true;
                }
                break;

            default: // Must be some other type of file (PIPE/FIFO/device)
                break;
            }

        }

        if (ACE_OS::chdir (cwd) == -1)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_chdir_back_failure,
                LM_ERROR,
                cwd);
        }

        return false;
    }

    bool CPMUtils::ping(
        const std::string & node)
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard (g_ping_mutex);

        u_long prev_flags = ACE_LOG_MSG->flags();
        ACE_LOG_MSG->clr_flags(1023);

        ACE_Ping_Socket ping;

        bool rc(true);

        if (ping.open() != -1)
        {
            //printf("AAAA: %s\n", node.c_str());
            ACE_INET_Addr addr1((u_short)0, node.c_str());
            ACE_Time_Value tv(1);
            if (ping.make_echo_check(addr1, false, &tv) == -1)
            {
                rc = false;
            }
            else
            {
                rc = true;
            }
            //printf("BBBB: %s\n", node.c_str());

            //int rc2 =
                ping.close(); // close takes time (2-3 secs) when the remote is unreachable. Is it possible to avoid that?
            //printf("CCCC: %d\n", rc2);
        }
        else
        {
            rc = false;
        }

        ACE_LOG_MSG->set_flags(prev_flags);

        return rc;
    }

    bool CPMUtils::get_object_location(
        const ICPM_module::ProcessDef & def,
        std::string & location)
    {
        std::string node = def.node_id.operator const CORBA::Char *();
        std::string process_id = def.process_id.operator const CORBA::Char *();
        std::string port = def.port_number.operator const CORBA::Char *();

        std::string new_port_number;
        if (!EnvVarExpand_ns::EnvVarExpand::expand_env_variables(port, new_port_number))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_expand_env_var_failure,
                LM_ERROR,
                port.c_str());
            return false;
        }
        port = new_port_number;

        std::string corbaloc = std::string("corbaloc:iiop:") + node + ":" + port;
        std::string obj_ref_name = process_id.c_str();
        location = corbaloc + "/" + obj_ref_name;

        return true;
    }

    bool CPMUtils::get_node_from_settings(
        const ::ICPM_module::CPMMSettings & settings,
        const std::string & node_id,
        ICPM_module::NodeDef & node)
    {
        for (::CORBA::ULong i = 0; i != settings.nodes.length(); i++)
        {
            std::string id = settings.nodes[i].node_id.operator const CORBA::Char *();
            if (id == node_id)
            {
                node = settings.nodes[i];
                return true;
            }
        }

        return false;
    }

    bool CPMUtils::get_process_from_settings(
        const ::ICPM_module::CPMMSettings & settings,
        const std::string & node_id,
        const std::string & process_id,
        ICPM_module::ProcessDef & proc)
    {
        for (::CORBA::ULong i = 0; i != settings.nodes.length(); i++)
        {
            std::string id = settings.nodes[i].node_id.operator const CORBA::Char *();
            if (id == node_id)
            {
                ICPM_module::NodeDef node = settings.nodes[i];

                for (::CORBA::ULong j = 0; j != node.processes.length(); j++)
                {
                    std::string proc_id = node.processes[j].process_id.operator const CORBA::Char *();
                    if (proc_id == process_id)
                    {
                        proc = node.processes[j];
                        return true;
                    }
                }
            }
        }

        return false;
    }


    bool CPMUtils::get_settings_from_outproc_cpmm(
        ::ICPM_module::CPMMSettings & settings)
    {
        // Get reference on cpmm.
        std::string cpmm_node;
        if (!get_cpmm_node(cpmm_node))
        {
            return false;
        }

        std::string cpmm_port;
        if (!get_cpmm_port(cpmm_port))
        {
            return false;
        }

        std::string location = std::string("corbaloc:iiop:") + cpmm_node + ":" + cpmm_port + "/" + "CPMMObj";

        ICPM_module::ICPMM_var cpmm;

        CPMTAOLayer::GetRefStatus status;
        cpmm = CPMTAOLAYER->get_ref_from_iortable<ICPM_module::ICPMM>(location.c_str(), status);
        if (!cpmm)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_corba_reference_failure,
                LM_ERROR,
                location.c_str());
            return false;
        }

        // Get settings from it.
        ICPM_module::CPMMSettings_var current_settings;
        try
        {
            ICPM_module::CPMMSettings_var initial_settings;
            ::CORBA::String_var settings_file;
            cpmm->get_settings(initial_settings, current_settings, settings_file);
        }
        catch(...)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_settings_from_cpmm_failure,
                LM_ERROR);
            return false;
        }

        settings = current_settings;

        return true;
    }

    bool CPMUtils::get_system_processes_list(
        std::list<std::string> & list)
    {
#       ifdef ACE_WIN32
            HANDLE hProcessSnap;
            PROCESSENTRY32 pe32;

            hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);  // Takes a snapshot of all the processes

            if (hProcessSnap == INVALID_HANDLE_VALUE)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_processes_snapshot_failure,
                    LM_ERROR,
                    GetLastError());
                return false;
            }

            pe32.dwSize = sizeof(PROCESSENTRY32);

            if(!Process32First(hProcessSnap, &pe32))
            {
                CloseHandle(hProcessSnap);
                RAISE_EVENT(
                    CPMUtilsEvents::ev_retreiving_first_processes_failure,
                    LM_ERROR,
                    GetLastError());
                return false;
            }

            do
            {
                list.push_back(pe32.szExeFile);
            } while(Process32Next(hProcessSnap,&pe32));  // gets next member of snapshot

            CloseHandle(hProcessSnap);  // closes the snapshot handle
#       else

            **** TO_TEST *****

            DIR* dir;
            struct dirent* ent;
            char* endptr;
            char buf[512];

            if (!(dir = opendir("/proc")))
            {
                // TO_DO
                //perror("can't open /proc");
                return false;
            }

            while((ent = readdir(dir)) != NULL)
            {
                /* if endptr is not a null character, the directory is not
                * entirely numeric, so ignore it */
                long lpid = strtol(ent->d_name, &endptr, 10);
                if (*endptr != '\0') {
                    continue;
                }

                /* try to open the stat file */
                snprintf(buf, sizeof(buf), "/proc/%ld/stat", lpid);
                FILE* fp = fopen(buf, "r");

                if (fp)
                {
                    const int proc_name_size = 64;
                    int pid;
                    char proc_name[proc_name_size];
                    memset(proc_name, 0, proc_name_size * sizeof(char));
                    fscanf(fp, "%d %s", &pid, proc_name);
                    char proc_name2[proc_name_size];
                    memset(proc_name2, 0, proc_name_size * sizeof(char));
                    // remove parentheses
                    strncpy(proc_name2, proc_name + 1, strlen(proc_name) - 2);
                    list.push_back(proc_name2);
                    fclose(fp);
                }
                else
                {
                    // TO_DO
                }
            }
            closedir(dir);

#       endif

        return true;
    }

    bool CPMUtils::is_process_running(
        const std::string & proc_name)
    {
        std::list<std::string> list;
        if (!get_system_processes_list(list))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_system_processes_list_failure,
                LM_ERROR);
            return false;
        }

        if (std::find(list.begin(), list.end(), proc_name + proc_extention()) != list.end())
        {
            return true;
        }
        else
        {
            return false;
        }
    }

#   ifdef ACE_WIN32
        bool CPMUtils::get_process_name(
            DWORD processID,
            std::string & process_name)
        {
            process_name = "";

            TCHAR szProcessName[MAX_PATH];

            // Get a handle to the process.
            HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                FALSE,
                processID );

            if (NULL != hProcess )
            {
                if (GetModuleFileNameEx(hProcess, 0, szProcessName, MAX_PATH))
                {
                    process_name = szProcessName;
                    backslash_to_slash(process_name);
                    size_t pos = process_name.rfind("/");
                    process_name = process_name.substr(pos + 1);
                }
                else
                {
                    process_name = "";
                }

                CloseHandle( hProcess );
            }

            return true;
        }
    #endif

    bool CPMUtils::load_cpm_params(
        bool & show_cpmm_console,
        bool & show_cpmd_console,
        bool & overwrite_autostart_to_false)
    {
        try
        {
            // Get boot parameters
            char * pPath = getenv ("CPM_ETC");
            if (!pPath)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_env_var_not_defined,
                    LM_ERROR,
                    "CPM_ETC");

                return false;
            }

            std::string path(pPath);
            backslash_to_slash(path);

            std::string file = path + "/cpm.cfg";
            file = FileShortcut_ns::FileShortcut::resolve(file);

            Config cfg;

            try
            {
                cfg.readFile(file.c_str());

                // root
                Setting & root = cfg.getRoot();

                // boot
                Setting & boot = root["boot"];

                // show_cpmd_console
                if (!boot.lookupValue("show_cpmd_console", show_cpmd_console))
                {
                    RAISE_EVENT(
                        CPMUtils::settings_field_not_found(),
                        LM_ERROR,
                        "boot/show_cpmd_console");
                    return false;
                }

                // show_cpmm_console
                if (!boot.lookupValue("show_cpmm_console", show_cpmm_console))
                {
                    RAISE_EVENT(
                        CPMUtils::settings_field_not_found(),
                        LM_ERROR,
                        "boot/show_cpmm_console");
                    return false;
                }

                // Overwrite_autostart_to_false
                if (!boot.lookupValue("overwrite_autostart_to_false", overwrite_autostart_to_false))
                {
                    RAISE_EVENT(
                        CPMUtils::settings_field_not_found(),
                        LM_ERROR,
                        "boot/overwrite_autostart_to_false");
                    return false;
                }
            }
            CPM_SETTINGS_CATCH(file)
        }
        catch(...)
        {
            return false;
        }

        return true;
    }

    bool CPMUtils::save_cpm_params(
        bool show_cpmm_console,
        bool show_cpmd_console,
        bool overwrite_autostart_to_false)
    {
        try
        {
            // Get boot parameters
            char * pPath = getenv ("CPM_ETC");
            if (!pPath)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_env_var_not_defined,
                    LM_ERROR,
                    "CPM_ETC");
                return false;
            }

            std::string path(pPath);
            backslash_to_slash(path);

            std::string file = path + "/cpm.cfg";
            file = FileShortcut_ns::FileShortcut::resolve(file);

            libconfig::Config cfg;
            
            try
            {
                cfg.readFile(file.c_str());

                libconfig::Setting & cpmd = cfg.lookup("boot/show_cpmd_console");
                cpmd = show_cpmd_console;

                libconfig::Setting & cpmm = cfg.lookup("boot/show_cpmm_console");
                cpmm = show_cpmm_console;

                libconfig::Setting & overwrite_autostart = cfg.lookup("boot/overwrite_autostart_to_false");
                overwrite_autostart = overwrite_autostart_to_false;

                cfg.writeFile(file.c_str());
            }
            CPM_SETTINGS_CATCH(file)
        }
        catch(...)
        {
            return false;
        }

        return true;
    }

    bool CPMUtils::boot_cpmd()
    {
        bool show_cpmd_console;
        bool show_cpmm_console;
        bool overwrite_autostart_to_false;

        if (!load_cpm_params(show_cpmm_console, show_cpmd_console, overwrite_autostart_to_false))
        {
            return false;
        }

        return boot_cpmd(show_cpmd_console);
    }

    bool CPMUtils::boot_cpmd(
        bool show_cpmd_console)
    {
        const std::string cmp_path("CPM_PATH");
        char * szPath = getenv(cmp_path.c_str());
        if (!szPath)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                cmp_path.c_str());
            return false;
        }

        std::string launch_path;
        if (!find_process(cpmd_module_name, szPath, launch_path))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_process_not_found,
                LM_ERROR,
                cpmd_module_name.c_str(),
                szPath);
            return false;
        }

        std::string cmd = launch_path + "/" +
                            cpmd_module_name;

        // Start CPMD only if it is not yet.
        std::list<std::string> list;
        get_system_processes_list(list);

        if (std::find(list.begin(), list.end(), cpmd_module_name + proc_extention()) == list.end())
        {
            ACE_Process_Options options;
            options.command_line(cmd.c_str());
    #       ifdef ACE_WIN32
                options.creation_flags(CREATE_NEW_CONSOLE);
                STARTUPINFO * si = options.startup_info();
                memset(si, 0, sizeof(si));
                si->cb = sizeof(STARTUPINFO);
                si->dwFlags = STARTF_USESHOWWINDOW;
                si->wShowWindow = SW_HIDE;
                if (show_cpmd_console)
                {
                    si->wShowWindow = SW_SHOW;
                }
    #       else
                // TO_DO: how does linux behave?
    #       endif

            ACE_Process process;
            pid_t pid = process.spawn(options);
            if (pid == -1)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_spawning_process_failure,
                    LM_ERROR,
                    cmd.c_str());
                return false;
            }

            bool found(false);
            ACE_Time_Value begin = ACE_OS::gettimeofday();
            while ((ACE_OS::gettimeofday() - begin).get_msec() < 3000)
            {
                std::list<std::string> list;
                get_system_processes_list(list);

                if (std::find(list.begin(), list.end(), cpmd_module_name + proc_extention()) != list.end())
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_spawning_process_failure,
                    LM_ERROR,
                    cmd.c_str());
                return false;
            }
        }

        return true;
    }

    bool CPMUtils::boot_cpm(
        bool use_these_params,
        bool show_cpmm_console_over,
        bool show_cpmd_console_over,
        bool overwrite_autostart_to_false_over)
    {
        bool show_cpmd_console = show_cpmd_console_over;
        bool show_cpmm_console = show_cpmm_console_over;
        bool overwrite_autostart_to_false = overwrite_autostart_to_false_over;

        if (!use_these_params)
        {
            if (!load_cpm_params(show_cpmm_console, show_cpmd_console, overwrite_autostart_to_false))
            {
                return false;
            }
        }

        const std::string cmp_path("CPM_PATH");
        char * szPath = getenv(cmp_path.c_str());
        if (!szPath)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_env_var_not_defined,
                LM_ERROR,
                cmp_path.c_str());
            return false;
        }

        std::string launch_path;
        if (!find_process(cpmd_module_name, szPath, launch_path))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_process_not_found,
                LM_ERROR,
                cpmd_module_name.c_str(),
                szPath);
            return false;
        }

        std::string cmd = launch_path + "/" +
                            cpmd_module_name;

        if (!boot_cpmd(show_cpmd_console))
        {
            return false;
        }

        // Start CPMM if host is the master node
        std::string cpmm_node;
        if (!get_cpmm_node(cpmm_node))
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_cpmm_node_failure,
                LM_ERROR);
            //shutdown_cpmd(); // To be called by some kind of service only
            return false;
        }

        std::string this_node = get_local_addr();

        if (this_node == cpmm_node)
        {
            std::string launch_path;
            if (!find_process(cpmm_module_name, szPath, launch_path))
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_process_not_found,
                    LM_ERROR,
                    cpmm_module_name.c_str(),
                    szPath);
                //shutdown_cpmd(); // To be called by some kind of service only
                return false;
            }

            std::string cmd = launch_path + "/" +
                                cpmm_module_name;

            if (overwrite_autostart_to_false)
            {
                cmd += " overwrite_autostart_to_false";
            }

            ACE_Process_Options options;
            options.command_line(cmd.c_str());

#       ifdef ACE_WIN32
            options.creation_flags(CREATE_NEW_CONSOLE);
            STARTUPINFO * si = options.startup_info();
            memset(si, 0, sizeof(si));
            si->cb = sizeof(STARTUPINFO);
            si->dwFlags = STARTF_USESHOWWINDOW;
            if (show_cpmm_console)
            {
                si->wShowWindow = SW_SHOW;
            }
#       else
            // TO_DO: how does linux behave?
#       endif

            ACE_Process process;
            pid_t pid = process.spawn(options);
            if (pid == -1)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_spawning_process_failure,
                    LM_ERROR,
                    cmd.c_str());
                //shutdown_cpmd(); // To be called by some kind of service only
                return false;
            }

            bool found(false);
            ACE_Time_Value begin = ACE_OS::gettimeofday();
            while ((ACE_OS::gettimeofday() - begin).get_msec() < 3000)
            {
                std::list<std::string> list;
                get_system_processes_list(list);

                if (std::find(list.begin(), list.end(), cpmm_module_name + proc_extention()) != list.end())
                {
                    found = true;
                    break;
                }
            }

            if (!found)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_spawning_process_failure,
                    LM_ERROR,
                    cmd.c_str());
                return false;
            }
        }
        else
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_cpmm_not_started_because_not_master,
                LM_INFO,
                this_node.c_str(),
                cpmm_node.c_str());
        }

        return true;
    }

    bool CPMUtils::shutdown_cpmd()
    {
        // Because CPMD is may be not launched yet, when catch exception
        // from CPMEventRaiser (RAISE_EVENT).
        try
        {
            std::list<std::string> list;
            get_system_processes_list(list);

            if (std::find(list.begin(), list.end(), cpmd_module_name + proc_extention()) == list.end())
            {
                return true;
            }

            // Get reference on cpmd.
            std::string this_node = get_local_addr();

            std::string cpmd_port;
            if (!get_cpmd_port(cpmd_port))
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_cpmd_port_failure,
                    LM_ERROR);
                return false;
            }

            std::string location = std::string("corbaloc:iiop:") + this_node + ":" + cpmd_port + "/" + "CPMD";

            ICPM_module::ICPMProcess_var cpmo;

            CPMTAOLayer::GetRefStatus status;
            cpmo = CPMTAOLAYER->get_ref_from_iortable<ICPM_module::ICPMProcess>(location.c_str(), status);
            if (!cpmo)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_corba_reference_failure,
                    LM_ERROR,
                    location.c_str());
                return false;
            }

            try
            {
                cpmo->ctrl_end();
            }
            catch(...)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_ending_failure,
                    LM_ERROR,
                    "CPMD");
                return false;
            }
        }
        catch(...)
        {
            return false;
        }

        return true;
    }

    bool CPMUtils::shutdown_cpmm()
    {
        // Because CPMD may not be launched yet (when catch exception from CPMEventRaiser (RAISE_EVENT)).
        try
        {
            std::list<std::string> list;
            get_system_processes_list(list);

            bool found(false);
            if (std::find(list.begin(), list.end(), cpmm_module_name + proc_extention()) == list.end())
            {
                return true;
            }

            // Get reference on cpmm.
            std::string cpmm_node;
            if (!get_cpmm_node(cpmm_node))
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_cpmm_node_failure,
                    LM_ERROR);
                return false;
            }

            std::string cpmm_port;
            if (!get_cpmm_port(cpmm_port))
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_cpmm_port_failure,
                    LM_ERROR);
                return false;
            }

            std::string location = std::string("corbaloc:iiop:") + cpmm_node + ":" + cpmm_port + "/" + cpmm_module_name;

            ICPM_module::ICPMProcess_var cpmo;

            CPMTAOLayer::GetRefStatus status;
            cpmo = CPMTAOLAYER->get_ref_from_iortable<ICPM_module::ICPMProcess>(location.c_str(), status);
            if (!cpmo)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_getting_corba_reference_failure,
                    LM_ERROR,
                    location.c_str());
                return false;
            }

            try
            {
                cpmo->ctrl_end();

                // Wait for cpmm termination
                while(true)
                {
                    std::list<std::string> list;
                    if (!get_system_processes_list(list))
                    {
                        continue;
                    }

                    if (std::find(list.begin(), list.end(), CPMUtils_ns::CPMUtils::cpmm_name() + CPMUtils_ns::CPMUtils::proc_extention()) == list.end())
                    {
                        break;
                    }
                }
            }
            catch(...)
            {
                RAISE_EVENT(
                    CPMUtilsEvents::ev_ending_failure,
                    LM_ERROR,
                    cpmm_module_name);
                return false;
            }
        }
        catch(...)
        {
            return false;
        }

        return true;
    }

    bool CPMUtils::shutdown_cpm()
    {
        shutdown_cpmm();

        //shutdown_cpmd(); // To be called by some kind of service only

        return true;
    }

    std::string CPMUtils::cpmm_name()
    {
        return cpmm_module_name;
    }

    std::string CPMUtils::cpmd_name()
    {
        return cpmd_module_name;
    }

    BasicEventHandling_ns::BasicEvent CPMUtils::settings_field_not_found()
    {
        return CPMSettingsCatchEvents::ev_libconfig_setting_field_not_found;
    }

    void CPMUtils::report_settings_failure(const std::string & msg)
    {
        RAISE_EVENT(CPMSettingsCatchEvents::ev_settings_failure,\
                    LM_ERROR,\
                    msg.c_str());\
    }


    bool NodeProcess_map_NodePort::prepare(
        const ICPM_module::CPMMSettings & settings)
    {
        // This method builds a map of
        //   node_id/process_id -> node_id:port_number

        map_.clear();

        for (CORBA::ULong i = 0; i != settings.nodes.length(); i++)
        {
            ICPM_module::NodeDef node_def = settings.nodes[i];

            if (node_def.enabled)
            {
                std::string node_id = node_def.node_id.operator const CORBA::Char *();

                for (CORBA::ULong j = 0; j != node_def.processes.length(); j++)
                {
                    ICPM_module::ProcessDef proc_def = node_def.processes[j];
                    if (proc_def.enabled)
                    {
                        std::string key = node_id + "/" + proc_def.process_id.operator const CORBA::Char *();
                        map_[key] = node_id + ":" + proc_def.port_number.operator const CORBA::Char *();
                    }
                }
            }
        }

        return true;
    }

    bool NodeProcess_map_NodePort::get_node(
        const std::string & node_process,
        std::string & node)
    {
        if (map_.find(node_process) == map_.end())
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_node_from_map_failure,
                LM_ERROR,
                node_process.c_str());
            return false;
        }

        size_t pos = node_process.find("/");
        if (pos == std::string::npos)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_bad_format,
                LM_ERROR,
                node_process.c_str());
            return false;
        }

        node = node_process.substr(0, pos);

        return true;
    }

    bool NodeProcess_map_NodePort::get_port(
        const std::string & node_process,
        std::string & port)
    {
        if (map_.find(node_process) == map_.end())
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_port_failure,
                LM_ERROR,
                node_process.c_str());
            return false;
        }

        size_t pos = map_[node_process].find(":");
        if (pos == std::string::npos)
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_bad_format,
                LM_ERROR,
                node_process.c_str());
            return false;
        }

        port = map_[node_process].substr(pos + 1);

        return true;
    }

    bool NodeProcess_map_NodePort::get_corbaloc_node_port(
        const std::string & node_process,
        std::string & corbaloc_node_port)
    {
        if (map_.find(node_process) == map_.end())
        {
            RAISE_EVENT(
                CPMUtilsEvents::ev_getting_corbaloc_node_port_from_map_failure,
                LM_ERROR,
                node_process.c_str());
            return false;
        }

        corbaloc_node_port = map_[node_process];

        return true;
    }

} // CPMUtils_ns namespace
