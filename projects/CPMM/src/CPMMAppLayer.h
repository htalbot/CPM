
#ifndef CPMMAPPLAYER_H_
#define CPMMAPPLAYER_H_

#include "CPMM/CPMMObj_i.h"
#include "CPMCore/CPMAppLayer.h"

class CPMMAppLayer: 
    public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    CPMMAppLayer();
    ~CPMMAppLayer();

    void set_overwrite_autostart_to_false(bool);

    bool run();

protected:
    virtual bool objects_register();
    virtual bool objects_init();
    virtual void terminate_();

protected:
    CPMMObj_i cpmm_obj_impl_;
    bool overwrite_autostart_to_false_;
};

#endif
