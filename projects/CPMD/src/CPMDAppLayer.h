
#ifndef CPMDAPPLAYER_H_
#define CPMDAPPLAYER_H_

#include "CPMCore/CPMAppLayer.h"
#include "CPMD/CPMDObj_i.h"
#include "CPMD/CPMEventMgrObj_i.h"

class CPMDAppLayer: 
    public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    CPMDAppLayer();
    ~CPMDAppLayer();

    bool run();

protected:
    virtual bool objects_register();
    virtual bool objects_init();
    virtual void terminate_();

protected:
    CPMDObj_i cpmd_obj_impl_;
    CPMEventMgrObj_i cpm_event_mgr_obj_;
};

#endif
