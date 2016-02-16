#ifndef CPMTRACEERASERAPPLAYER_H_
#define CPMTRACEERASERAPPLAYER_H_

#include "CPMCore/CPMAppLayer.h"
#include "CPMTraceEraser/CPMTraceEraserObj.h"

//***************************************
// Add objects headers                                                          // <--
//#include "CPMObjDummy/CPMObjDummy_i.h"
//***************************************

class CPMTraceEraserAppLayer: public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    CPMTraceEraserAppLayer();
    ~CPMTraceEraserAppLayer();
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
    CPMTraceEraserObj cpm_trace_eraser_obj_;    
};


#endif
