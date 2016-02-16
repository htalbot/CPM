
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/INET_Addr.h"
#include "ACEWin64Warnings/BeginSuppressWarnings.h"

#include "CPMM/CPMMObj_i.h"
#include "CPMM/CPMM.evc.h"
#include "CPMCore/CPMUtils.h"
#include "CPMMAppLayer.h"
#include "CPMCore/CPMEventRaiser.h"
#include "Barriers.h"

#define NO_BARRIER  // ACE_Barrier has a bug

class NodePingThread: public ACE_Task_Base
{
public:
    NodePingThread(NodeTaskArgs * args)
    : node_task_args_(args)
    {
    }

    ~NodePingThread()
    {
        delete node_task_args_;
    }

    int svc()
    {
        if (node_task_args_->node_.enabled)
        {
            //ACE_DEBUG((LM_DEBUG, "%T - Ping begin %s\n", node_task_args_.node_.node_id.operator const CORBA::Char *()));
            // Avec ACE_DEBUG, avec 2 thread, le begin est affiché pour un thread et le end est affiché pour l'autre thread.
            // On n'a pas les deux begin et les deux end ????????
            //printf("Ping begin %s\n", node_task_args_->node_.node_id.operator const CORBA::Char *());
            if (CPMUtils_ns::CPMUtils::ping(node_task_args_->node_.node_id.operator const CORBA::Char *()))
            {
                node_task_args_->barrier_.add_success(node_task_args_->node_);
            }
            else
            {
                node_task_args_->barrier_.add_failure(node_task_args_->node_);
            }
            //ACE_DEBUG((LM_DEBUG, "%T - Ping end %s\n", node_task_args_.node_.node_id.operator const CORBA::Char *()));
            //printf("Ping end %s\n", node_task_args_->node_.node_id.operator const CORBA::Char *());
        }

        return 0;
    }

protected:
    NodeTaskArgs * node_task_args_;
};

CPMMObj_i::CPMMObj_i()
: want_to_stop_(false)
{
}

CPMMObj_i::~CPMMObj_i()
{
    //CPMTRACER->trace("%s", _SRC);
}

bool CPMMObj_i::init_i(bool overwrite_autostart_to_false)
{
    std::string default_cfg_file;
    if (!CPMUtils_ns::CPMUtils::get_default_cpmm_settings_file(default_cfg_file))
    {
        RAISE_EVENT(CPMMEvents::ev_getting_default_cpmm_settings_file_failure,
                    LM_ERROR);
        return false;
    }

    if (!CPMUtils_ns::CPMUtils::get_cpmm_settings_from_file(default_cfg_file, initial_settings_))
    {
        RAISE_EVENT(CPMMEvents::ev_getting_default_cpmm_settings_failure,
                    LM_ERROR);
        return false;
    }

    if (!validate_settings(initial_settings_))
    {
        return false;
    }

    current_settings_ = initial_settings_;
    current_settings_file_ = default_cfg_file;

    if (!CPMUtils_ns::CPMUtils::get_cpmd_port(cpmd_port_))
    {
        RAISE_EVENT(CPMMEvents::ev_cpmd_port_env_var_not_defined,
                    LM_ERROR);
        return false;
    }

    if (current_settings_.startup.auto_launch_processes
        && !overwrite_autostart_to_false)
    {
        launch(current_settings_);
    }

    return true;
}

void CPMMObj_i::get_settings(::ICPM_module::CPMMSettings_out initial_settings,
                            ::ICPM_module::CPMMSettings_out current_settings,
                            ::CORBA::String_out current_settings_file)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (settings_mutex_);

    initial_settings = new ::ICPM_module::CPMMSettings(initial_settings_);
    current_settings = new ::ICPM_module::CPMMSettings(current_settings_);
    CORBA::String_var tmp = CORBA::string_dup(current_settings_file_.c_str());
    current_settings_file = tmp._retn();
}

::CORBA::Boolean CPMMObj_i::start(const ::ICPM_module::ProcessSeq & procs)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    start_or_stop(procs, action_start);

    return 1;
}

::CORBA::Boolean CPMMObj_i::stop(const ::ICPM_module::ProcessSeq & procs)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    start_or_stop(procs, action_stop);

    return 1;
}

void CPMMObj_i::show_console (
    const ::ICPM_module::ProcessSeq & procs,
    ::CORBA::Boolean show)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    start_or_stop(procs, show ? action_show_console : action_hide_console);
}

void CPMMObj_i::show_gui (
    const ::ICPM_module::ProcessSeq & procs,
    ::CORBA::Boolean show)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    start_or_stop(procs, show ? action_show_gui : action_hide_gui);
}

::CORBA::Boolean CPMMObj_i::started()
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    int cond = (started_processes_.size() != 0);

    return cond ? 1 : 0;
}

::CORBA::Boolean CPMMObj_i::get_unavailable (
    ::ICPM_module::UnavailableElementIDSeq_out nodes,
    ::ICPM_module::UnavailableElementIDSeq_out processes)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (unavailable_mutex_);

    long i(0);
    std::list<std::string>::iterator it;

    nodes = new ::ICPM_module::UnavailableElementIDSeq;
    nodes->length(CORBA::ULong(unavailable_nodes_.size()));
    for (it = unavailable_nodes_.begin(); it != unavailable_nodes_.end(); it++)
    {
        ::ICPM_module::UnavailableElementID element;
        element.element_id = it->c_str();
        (*nodes)[i] = element;
        ++i;
    }

    i = 0;
    processes = new ::ICPM_module::UnavailableElementIDSeq;
    processes->length(CORBA::ULong(unavailable_processes_.size()));
    for (it = unavailable_processes_.begin(); it != unavailable_processes_.end(); it++)
    {
        ::ICPM_module::UnavailableElementID element;
        element.element_id = it->c_str();
        (*processes)[i] = element;
        ++i;
    }

    return 1;
}

void CPMMObj_i::update_settings(
            const ::ICPM_module::CPMMSettings & new_initial_settings,
            const ::ICPM_module::CPMMSettings & new_curent_settings,
            const char * file)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->settings_mutex_);
    initial_settings_ = new_initial_settings;
    current_settings_ = new_curent_settings;
    current_settings_file_ = file;
}

::CORBA::Boolean CPMMObj_i::save_settings_to(const char * target_file)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->settings_mutex_);

    if (!CPMUtils_ns::CPMUtils::save_cpmm_settings(current_settings_, target_file))
    {
        RAISE_EVENT(CPMMEvents::ev_saving_cpmm_settings_failure,
                    LM_ERROR,
                    target_file);
        return 0;
    }

    initial_settings_ = current_settings_;
    current_settings_file_ = target_file;

    return 1;
}

::CORBA::Boolean CPMMObj_i::set_as_default()
{
    std::string file;
    size_t pos = current_settings_file_.rfind("/");
    if (pos != std::string::npos)
    {
        file = current_settings_file_.substr(pos + 1);
    }
    else
    {
        RAISE_EVENT(CPMMEvents::ev_bad_file_name_format,
                    LM_ERROR,
                    current_settings_file_.c_str());
        return 0;
    }

    return CPMUtils_ns::CPMUtils::set_as_default_settings(file) ? 1 : 0;
}

bool CPMMObj_i::validate_settings(const ::ICPM_module::CPMMSettings & settings)
{
    std::list<std::string> node_ids;

    for (::CORBA::ULong i = 0; i != settings.nodes.length(); i++)
    {
        std::string id = settings.nodes[i].node_id.operator const CORBA::Char *();
        std::list<std::string>::iterator it;
        it = std::find(node_ids.begin(),
                        node_ids.end(),
                        id);
        if (it == node_ids.end())
        {
            node_ids.push_back(id);
        }
        else
        {
            RAISE_EVENT(CPMMEvents::ev_node_defined_multiple_times,
                        LM_ERROR);

            return false;
        }
    }

    return true;
}

void CPMMObj_i::launch(const ::ICPM_module::CPMMSettings & settings)
{
    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (start_stop_mutex_);

    // Get total number of processes
    long count(0);
    for (CORBA::ULong i = 0; i != settings.nodes.length(); i++)
    {
        count += settings.nodes[i].processes.length();
    }

    ::ICPM_module::ProcessSeq proc_seq;

    proc_seq.length(count);

    long index(0);
    for (CORBA::ULong i = 0; i != settings.nodes.length(); i++)
    {
        for (::CORBA::ULong j = 0; j != settings.nodes[i].processes.length(); j++)
        {
            proc_seq[index++] = settings.nodes[i].processes[j];
        }
    }

    start_or_stop(proc_seq, action_start);
}

void CPMMObj_i::start_or_stop(const ::ICPM_module::ProcessSeq & procs, enum_popup action)
{
    std::string action_text;
    if (action == action_start)
    {
        action_text = "start";
    }
    else
    {
        action_text = "stop";

        if (started_.value())
        {
            wait_for_required_time_before_stop_since_start();
        }
    }

    std::list<std::string> unavail_nodes;
    std::list<std::string> unavail_processes;
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard (unavailable_mutex_);

        std::list<std::string>::iterator it_unavail;
        for (it_unavail = unavailable_nodes_.begin(); it_unavail != unavailable_nodes_.end(); it_unavail++)
        {
            unavail_nodes.push_back(*it_unavail);
        }

        for (it_unavail = unavailable_processes_.begin(); it_unavail != unavailable_processes_.end(); it_unavail++)
        {
            unavail_processes.push_back(*it_unavail);
        }
    }

    ::ICPM_module::NodeSeq settings_nodes;
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->settings_mutex_);
        settings_nodes = current_settings_.nodes;
    }

    // Use a map to work on a node at a time.
    std::map<std::string, std::list<::ICPM_module::ProcessDef> > nodes;
    for (::CORBA::ULong i = 0; i != procs.length(); i++)
    {
        if (procs[i].enabled)
        {
            bool insert(true);

            std::string proc_node_id = procs[i].node_id.operator const CORBA::Char *();

            // Is proc's node enabled? If not, don't insert proc_node_id in the map.
            for (CORBA::ULong j = 0; j != settings_nodes.length(); j++)
            {
                std::string settings_node_id = settings_nodes[j].node_id.operator const CORBA::Char *();
                if (settings_node_id == proc_node_id
                    && !settings_nodes[j].enabled)
                {
                    // Disabled node: don't start/stop processes on it.
                    insert = false;
                    break;
                }
            }

            if (insert)
            {
                // Node is enabled.

                // Is node available? If not, don't insert proc_node_id in the map.

                std::list<std::string>::iterator it;
                for (it = unavail_nodes.begin();
                        it != unavail_nodes.end();
                        it++)
                {
                    if (*it == proc_node_id)
                    {
                        // Unavailable node: unnecessary to try starting/stopping processes on it.
                        insert = false;
                        break;
                    }
                }

                if (insert)
                {
                    // Node is available.

                    // Is process available? If not and if we want to stop, don't insert proc_node_id in the map.
                    for (it = unavail_processes.begin();
                            it != unavail_processes.end();
                            it++)
                    {
                        std::string proc_id = std::string(procs[i].node_id.operator const CORBA::Char *()) +
                                                ":" +
                                                std::string(procs[i].process_id.operator const CORBA::Char *());

                        if (*it == proc_id
                            && action == action_stop)
                        {
                            // Unavailable process: unnecessary to try stopping it.
                            insert = false;
                            break;
                        }
                    }
                }
            }

            if (insert)
            {
                nodes[proc_node_id].push_back(procs[i]);
            }
        }
    }

    // Iterate over nodes
    std::map<std::string, std::list<::ICPM_module::ProcessDef> >::iterator it;
    for (it = nodes.begin(); it != nodes.end(); it++)
    {
        // Where the cpmd is:
        std::string location("corbaloc:iiop:");
        std::string node_id = it->first;

        if (!CPMUtils_ns::CPMUtils::ping(node_id))
        {
            RAISE_EVENT(CPMMEvents::ev_ping_failure,
                        LM_ERROR,
                        action_text.c_str(),
                        node_id.c_str());
            continue;
        }

        location += node_id + ":" + cpmd_port_;
        location += std::string("/") + "CPMDObj";

        ::ICPM_module::ICPMD_var cpmd;

        // Get a reference on CPMD from node

        // ACE_DEBUG((LM_DEBUG, "%T - %s - get_ref on %s\n", _SRC, location.c_str()));
        int prev_connection_timeout = CPMTAOLAYER->set_connection_timeout_orb(100);
        CPMTAOLayer::GetRefStatus status;
        cpmd = CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMD>(location.c_str(), status, 1, 0);
        CPMTAOLAYER->set_connection_timeout_orb(prev_connection_timeout);
        // ACE_DEBUG((LM_DEBUG, "%T - %s - after get_ref on %s\n", _SRC, location.c_str()));

        if(!cpmd)
        {
            RAISE_EVENT(CPMMEvents::ev_getting_cpmd_object_ref_failure,
                        LM_ERROR,
                        action_text.c_str(),
                        location.c_str());
        }
        else
        {
            try
            {
                std::list<::ICPM_module::ProcessDef> list = it->second;

                ::ICPM_module::ProcessSeq proc_seq;
                proc_seq.length(CORBA::ULong(list.size()));

                long i(0);
                std::list<::ICPM_module::ProcessDef>::iterator it_proc;
                for (it_proc = list.begin(); it_proc != list.end(); it_proc++)
                {
                    proc_seq[i++] = *it_proc;
                }

                switch(action)
                {
                    case action_start:
                        //CPMTRACER->trace("%s - start...", _SRC);
                        if (cpmd->start(proc_seq))
                        {
                            for (it_proc = list.begin(); it_proc != list.end(); it_proc++)
                            {
                                std::string process_id = it_proc->process_id.operator const CORBA::Char *();
                                {
                                    started_processes_[process_id] = *it_proc;
                                }
                            }
                        }
                        else
                        {
                            RAISE_EVENT(CPMMEvents::ev_cpmd_start_stop_failure,
                                        LM_ERROR,
                                        action_text.c_str(),
                                        node_id.c_str());
                        }
                        //CPMTRACER->trace("%s - start done.", _SRC);
                        break;

                    case action_stop:
                        //CPMTRACER->trace("%s - stop...", _SRC);
                        ACE_DEBUG((LM_DEBUG, "%s - before cpmd->stop()\n", _SRC));
                        if (cpmd->stop(proc_seq))
                        {
                            ACE_DEBUG((LM_DEBUG, "%s - cpmd->stop() - ok\n", _SRC));
                            for (it_proc = list.begin(); it_proc != list.end(); it_proc++)
                            {
                                std::string process_id = it_proc->process_id.operator const CORBA::Char *();
                                {
                                    started_processes_.erase(process_id);
                                }
                            }

                            for (auto it = started_processes_.begin(); it != started_processes_.end(); it++)
                            {
                                ACE_DEBUG((LM_DEBUG, "%s - %s remains in started_processes_\n", _SRC, it->first.c_str()));
                            }
                        }
                        else
                        {
                            ACE_DEBUG((LM_DEBUG, "%s - cpmd->stop() - not ok\n", _SRC));
                            RAISE_EVENT(CPMMEvents::ev_cpmd_start_stop_failure,
                                        LM_ERROR,
                                        action_text.c_str(),
                                        node_id.c_str());
                        }
                        //CPMTRACER->trace("%s - stop done.", _SRC);
                        break;

                    case action_show_console:
                        //ACE_DEBUG((LM_DEBUG, "%s - cpmd->show_console()\n", _SRC));
                        cpmd->show_console(proc_seq, 1);
                        break;

                    case action_hide_console:
                        //ACE_DEBUG((LM_DEBUG, "%s - cpmd->show_console()\n", _SRC));
                        cpmd->show_console(proc_seq, 0);
                        break;

                    case action_show_gui:
                        //ACE_DEBUG((LM_DEBUG, "%s - cpmd->show_console()\n", _SRC));
                        cpmd->show_gui(proc_seq, 1);
                        break;

                    case action_hide_gui:
                        //ACE_DEBUG((LM_DEBUG, "%s - cpmd->show_console()\n", _SRC));
                        cpmd->show_gui(proc_seq, 0);
                        break;
                };
            }
            catch(...)
            {
                RAISE_EVENT(CPMMEvents::ev_cpmd_start_stop_exception,
                            LM_ERROR,
                            action_text.c_str(),
                            node_id.c_str());
            }
        }
    }

    if (action == action_start)
    {
        started_at_ = ACE_OS::gettimeofday();
        started_ = true;

        ACE_DEBUG((LM_DEBUG, "%T - %s - started.\n", _SRC));
    }
    else
    {
        ACE_DEBUG((LM_DEBUG, "%T - %s - stopped.\n", _SRC));
        started_ = false;
    }
    //CPMTRACER->trace("%s - end.", _SRC);
}

#if !defined(NO_BARRIER)
static void *
node_ping_task(void *arguments)
{
    NodeTaskArgs *arg= (NodeTaskArgs*)arguments;

    if (arg->node_.enabled)
    {
        if (CPMUtils_ns::CPMUtils::ping(arg->node_.node_id.operator const CORBA::Char *()))
        {
            arg->barrier_.add_success(arg->node_);
        }
        else
        {
            arg->barrier_.add_failure(arg->node_);
        }
    }

    arg->barrier_.wait();
    delete arg;
    return 0;
}

#endif

int CPMMObj_i::svc()
{
    while (!want_to_stop_.value())
    {
        bool have_to_sleep(false);

        check(have_to_sleep);

        if (have_to_sleep) 
        {
            ACE_OS::sleep(1); // 1 sec
        }
        else
        {
            ACE_Time_Value tv(0, 100000); // 100 msec
            ACE_OS::sleep(tv);
        }
    }

    //CPMTRACER->trace("%s - end.", _SRC);

    return 0;
}

void CPMMObj_i::terminate()
{
    //CPMTRACER->trace("%s - want_to_stop_ = true.", _SRC);
    want_to_stop_ = true;

    //CPMTRACER->trace("%s - wait...", _SRC);
    wait();

    //CPMTRACER->trace("%s - stop_all_processes_on_all_nodes()...", _SRC);
    stop_all_processes_on_all_nodes();
    //CPMTRACER->trace("%s - stop_all_processes_on_all_nodes() done.", _SRC);
}

void CPMMObj_i::check(bool & have_to_sleep)
{
    ACE_DEBUG((LM_DEBUG, "%T - CHECK - begin\n"));

    // Ping all nodes
    ::ICPM_module::NodeSeq nodes;
    {
        ACE_Guard<ACE_Recursive_Thread_Mutex> guard (this->settings_mutex_);
        nodes = current_settings_.nodes;
    }

    // Create a barrier.
    NodesBarrier nodes_barrier(nodes.length());

#   ifdef NO_BARRIER

        size_t n_enabled(0);
        size_t n = nodes.length();
        for (CORBA::ULong i = 0; i < n; i++)
        {
            if (nodes[i].enabled)
            {
                ++n_enabled;
            }
        }

        NodePingThread ** array_of_threads = new NodePingThread *[n_enabled];
        size_t j(0);
        for (::CORBA::ULong i = 0; i != n; i++)
        {
            if (nodes[i].enabled)
            {
                NodeTaskArgs * args = new NodeTaskArgs(nodes_barrier, nodes[i]);

                array_of_threads[j] = new NodePingThread(args);

                array_of_threads[j]->activate();

                ++j;
            }
        }

        for (size_t i = 0; i != n_enabled; i++)
        {
            array_of_threads[i]->wait();
            delete array_of_threads[i];
        }

        delete [] array_of_threads;

#   else
        // Quand ACE_Barrier va être debuggué, on pourra lancer les ping en parallèle à l'aide de ACE_Barrier (cross-platform)

        // Create threads args
        std::auto_ptr<NodeTaskArgs *> node_task_args(new NodeTaskArgs *[nodes.length()]);
        for (int i = 0; i != nodes.length(); i++)
        {
            node_task_args.get()[i] = new NodeTaskArgs(nodes_barrier, nodes[i]);
        }

        //Spawn threads
        for(::CORBA::ULong i= 0; i < nodes.length(); i++)
        {
            int grp_id = ACE_Thread::spawn(
                            (ACE_THR_FUNC)node_ping_task,
                            (void*)node_task_args.get()[i],
                            THR_DETACHED|THR_NEW_LWP|THR_SCOPE_PROCESS);
            if(grp_id == -1)
            {
                CPMUtils_ns::CPMUtils::log(LM_ERROR,
                            "Can't spawn node pigner task %d/%d\n",
                            i, nodes.length());
            }
        }

        // Wait for threads
        nodes_barrier.wait();
#   endif

    ACE_Guard<ACE_Recursive_Thread_Mutex> guard (unavailable_mutex_);

    unavailable_nodes_.clear();
    unavailable_processes_.clear();

    // Fill unavailable nodes and for available ones, fill unavailable processes.
    std::list<std::string>::iterator it;
    {
        // Failures with nodes
        std::list<const ::ICPM_module::NodeDef> & failures = nodes_barrier.failures();
        std::list<const ::ICPM_module::NodeDef>::iterator it;
        have_to_sleep = (failures.size() == 0);
        for (it = failures.begin(); it != failures.end(); it++)
        {
            std::string node_id = it->node_id.operator const CORBA::Char *();

            ACE_DEBUG((LM_DEBUG, "%T -   node: %s ping failed\n", node_id.c_str()));
            unavailable_nodes_.push_back(node_id);

            // For unavailable nodes, associated processes aren't available anymore.
            for (::CORBA::ULong i = 0; i != it->processes.length(); i++)
            {
                if (it->processes[i].enabled)
                {
                    std::string proc_id = it->processes[i].process_id.operator const CORBA::Char *();
                    std::string long_proc_id = node_id + ":" + proc_id;

                    unavailable_processes_.push_back(long_proc_id);
                }
            }
        }

        // For nodes that are available, let's check processes availability.
        std::list<const ::ICPM_module::NodeDef> & successes = nodes_barrier.successes();
        for (it = successes.begin(); it != successes.end(); it++)
        {
            ::ICPM_module::NodeDef node = *it;
            std::string node_id = node.node_id.operator const CORBA::Char *();

            ACE_DEBUG((LM_DEBUG, "%T -   node: %s ping ok\n", node_id.c_str()));

            std::list<std::string> cpmd_processes;
            if (!get_procs_from_cpmd(node_id, cpmd_processes))
            {
                ACE_DEBUG((LM_DEBUG, "%T -   get_procs_from_cpmd failed on %s\n", node_id.c_str()));
                // TO_DO
                for (::CORBA::ULong j = 0; j != node.processes.length(); j++)
                {
                    if (node.processes[j].enabled)
                    {
                        std::string node_process_id = node.processes[j].process_id;

                        bool found(false);
                        std::list<std::string>::iterator it_proc;
                        for (it_proc = cpmd_processes.begin(); it_proc != cpmd_processes.end(); it_proc++)
                        {
                            if (*it_proc == node_process_id)
                            {
                                found = true;
                            }
                        }

                        ACE_DEBUG((LM_DEBUG, "%T -     %s : %s\n", node_process_id.c_str(), found ? "reached" : "not reached"));

                        if (!found)
                        {
                            std::string long_proc_id = node_id + ":" + node_process_id;

                            unavailable_processes_.push_back(long_proc_id);
                        }
                    }
                }

                continue;
            }

            for (::CORBA::ULong i = 0; i != nodes.length(); i++)
            {
                ::ICPM_module::NodeDef settings_node = current_settings_.nodes[i];
                std::string node_settings_id = settings_node.node_id.operator const CORBA::Char *();

                if (node_settings_id == node_id)
                {
                    for (::CORBA::ULong j = 0; j != node.processes.length(); j++)
                    {
                        if (node.processes[j].enabled)
                        {
                            std::string node_process_id = node.processes[j].process_id;

                            bool found(false);
                            std::list<std::string>::iterator it_proc;
                            for (it_proc = cpmd_processes.begin(); it_proc != cpmd_processes.end(); it_proc++)
                            {
                                if (*it_proc == node_process_id)
                                {
                                    found = true;
                                }
                            }

                            ACE_DEBUG((LM_DEBUG, "%T -     %s : %s\n", node_process_id.c_str(), found ? "alive" : "dead"));

                            if (!found)
                            {
                                std::string long_proc_id = node_id + ":" + node_process_id;

                                unavailable_processes_.push_back(long_proc_id);
                            }
                        }
                    }

                    break;
                }
            }
        }
    }

    ACE_DEBUG((LM_DEBUG, "%T - CHECK - end\n"));
}

bool CPMMObj_i::get_procs_from_cpmd(std::string & node_id, std::list<std::string> & processes)
{
    // Where the cpmd is:
    std::string location("corbaloc:iiop:");

    location += node_id + ":" + cpmd_port_;
    location += std::string("/") +"CPMDObj";

    // Get a reference on CPMD from node

    //ACE_DEBUG((LM_DEBUG, "%T - %s - get_ref on %s\n", _SRC, location.c_str()));
    int prev_connection_timeout = CPMTAOLAYER->set_connection_timeout_orb(100);
    CPMTAOLayer::GetRefStatus status;
    ::ICPM_module::ICPMD_var cpmd = CPMTAOLAYER->get_ref_from_iortable<::ICPM_module::ICPMD>(location.c_str(), status, 1, 0);
    CPMTAOLAYER->set_connection_timeout_orb(prev_connection_timeout);
    //ACE_DEBUG((LM_DEBUG, "%T - %s - after get_ref on %s\n", _SRC, location.c_str()));

    if (!cpmd)
    {
        //ACE_DEBUG((LM_DEBUG, "%T - %s - %s unavailable, return false\n", _SRC, location.c_str()));
        RAISE_EVENT(CPMMEvents::ev_getting_cpmd_object_ref_failure2,
                    LM_ERROR,
                    location.c_str());
        return false;
    }

    try
    {
        ::ICPM_module::ProcessSeq_var procs;
        cpmd->get_launched_processes(procs);

        for (CORBA::ULong i = 0; i != procs->length(); i++)
        {
            processes.push_back(procs[i].process_id.operator const CORBA::Char *());
        }
    }
    catch(...)
    {
        //ACE_DEBUG((LM_DEBUG, "%T - %s - can't get launched processes on %s\n", _SRC, location.c_str()));
        RAISE_EVENT(CPMMEvents::ev_getting_launched_processes_failure,
                    LM_ERROR,
                    location.c_str());
        return false;
    }

    //ACE_DEBUG((LM_DEBUG, "%T - %s - %s unavailable, return true\n", _SRC, location.c_str()));

    return true;
}

bool CPMMObj_i::stop_all_processes_on_all_nodes()
{
    bool ok(true);
    for (::CORBA::ULong i = 0; i != current_settings_.nodes.length(); i++)
    {
        ICPM_module::NodeDef node = current_settings_.nodes[i];
        if (node.enabled)
        {
            if (!stop(node.processes))
            {
                ok = false;
            }
        }
    }

    if (!ok)
    {
        ACE_DEBUG((LM_DEBUG, "Failed to stop all processes\n"));
        return false;
    }

    return true;
}

void CPMMObj_i::wait_for_required_time_before_stop_since_start()
{
    while (!started_.value())
    {
        ACE_OS::sleep(ACE_Time_Value(0, 10000));
    }

    while ((ACE_OS::gettimeofday() - started_at_).get_msec() < current_settings_.startup.required_time_before_stop_since_start_ms)
    {
        ACE_OS::sleep(ACE_Time_Value(0, 10000));
    }
}
