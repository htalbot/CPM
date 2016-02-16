
#ifndef TEST_CPMEVENTRAISERAPPLAYER_H_
#define TEST_CPMEVENTRAISERAPPLAYER_H_

#include "CPMCore/CPMAppLayer.h"

class Test_CPMEventRaiserAppLayer:
    public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    Test_CPMEventRaiserAppLayer();
    virtual ~Test_CPMEventRaiserAppLayer();

    int run();

    virtual bool objects_register() { return true; }
    virtual bool objects_init() { return true; }
    virtual void terminate_() {}

protected:
};

#endif
