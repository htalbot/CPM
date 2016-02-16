#ifndef CPMFOOAPPLAYER_H_
#define CPMFOOAPPLAYER_H_

#include "CPMCore/CPMAppLayer.h"

#include "CPMObjDummy/CPMObjDummy_i.h"

class CPMFooAppLayer: public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    CPMFooAppLayer();
    ~CPMFooAppLayer();
    virtual bool init_corba_objects();
    virtual bool run();

protected:

    virtual bool objects_register();
    virtual bool objects_init();
    virtual void terminate_();

protected:
    CPMObjDummy_i cpm_obj_dummy_;
    CPMObjDummy_i cpm_obj_dummy2_;
};


#endif
