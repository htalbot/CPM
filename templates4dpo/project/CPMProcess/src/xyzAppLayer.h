#ifndef XYZAPPLAYER_H_
#define XYZAPPLAYER_H_

#include "CPMCore/CPMAppLayer.h"

//***************************************
// Add objects headers                                                          // <--
//#include "CPMObjDummy/CPMObjDummy_i.h"
//***************************************

class XyzAppLayer: public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    XyzAppLayer();
    ~XyzAppLayer();
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
