
#ifndef CPMPLAYERAPPLAYER_H_
#define CPMPLAYERAPPLAYER_H_

#include "CPMCore/CPMAPPLAYER.h"

class CPMPlayerAppLayer: public CPMAppLayer_ns::CPMAppLayerBase
{
public:

    CPMPlayerAppLayer();

    int run(int argc, char * argv[]);

    virtual bool objects_register()
    {
        return true;
    }

    virtual bool objects_init()
    {
        return true;
    }

    virtual void terminate_()
    {
    }

protected:

};

#endif
