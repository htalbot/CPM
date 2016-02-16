#ifndef CPMM_BARRIERS_H
#define CPMM_BARRIERS_H

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Barrier.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "ICPM/ICPMC.h"
#include "CPMMAppLayer.h"
#include <string>
#include <list>

//************************** Nodes *****************************
class NodesBarrier: public ACE_Barrier
{
public:
    NodesBarrier(unsigned int count,
               const ACE_TCHAR * = 0,
               void * = 0)
    : ACE_Barrier(count)
    {
    }

    void add_failure(const ICPM_module::NodeDef & node)
    {
        failures_.push_back(node);
    }

    std::list<const ICPM_module::NodeDef> & failures()
    {
        return failures_;
    }

    void add_success(const ICPM_module::NodeDef node)
    {
        successes_.push_back(node);
    }

    std::list<const ICPM_module::NodeDef> & successes()
    {
        return successes_;
    }

protected:

    std::list<const ICPM_module::NodeDef> failures_;
    std::list<const ICPM_module::NodeDef> successes_;
};

struct NodeTaskArgs
{
    NodeTaskArgs(NodesBarrier & barrier,
                    const ICPM_module::NodeDef & node)
    : barrier_(barrier)
    , node_(node)
    {
    }

    ICPM_module::NodeDef node_;
    NodesBarrier & barrier_;
};

//************************** Processes *****************************
class ProcBarrier: public ACE_Barrier
{
public:
    ProcBarrier(unsigned int count,
               const ACE_TCHAR * = 0,
               void * = 0)
    : ACE_Barrier(count)
    {
    }

    void add_failure(const std::string & long_proc_id)
    {
        failures_.push_back(long_proc_id);
    }

    std::list<std::string> & failures()
    {
        return failures_;
    }

    void add_success(const std::string & long_proc_id)
    {
        successes_.push_back(long_proc_id);
    }

    std::list<std::string> & successes()
    {
        return successes_;
    }

protected:

    std::list<std::string> failures_;
    std::list<std::string> successes_;
};


struct ProcTaskArgs
{
    ProcTaskArgs(ProcBarrier & barrier,
                    CPMMAppLayer * app_layer,
                    const std::string location,
                    const std::string id,
                    const std::string node_id,
                    bool enabled)
    : barrier_(barrier)
    , app_layer_(app_layer)
    , location_(location)
    , id_(id)
    , node_id_(node_id)
    , enabled_(enabled)
    {
    }

    ProcBarrier & barrier_;
    CPMMAppLayer * app_layer_;
    std::string location_;
    std::string id_;
    std::string node_id_;
    bool enabled_;
};


#endif // CPMM_BARRIERS_H