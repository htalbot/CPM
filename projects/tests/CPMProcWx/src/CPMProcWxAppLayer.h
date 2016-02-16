#ifndef CPMPROCWXAPPLAYER_H_
#define CPMPROCWXAPPLAYER_H_

#include "CPMCore/CPMAppLayer.h"

//***************************************
// Register objets types                                                        // <--
//REGISTER_TYPEOF(1, CPMObjDummy_i)
//REGISTER_TYPEOF(2, <*>_i)
//***************************************

class CPMProcWxAppLayer: public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    CPMProcWxAppLayer();
    ~CPMProcWxAppLayer();
    virtual bool init_corba_objects();
    virtual bool run();

protected:

    virtual bool objects_register();
    virtual bool objects_init();
    virtual void terminate_();

protected:
//***************************************
// Declare objets                                                               // <--
    //CPMObjDummy_i cpm_obj_dummy_;
    //CPMObjDummy_i cpm_obj_dummy2_;
//***************************************
};


#endif
