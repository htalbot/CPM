#ifdef CPMPROCESS_WX

#pragma once

#include "CPMFooAppLayer.h"

#include "CPMFooFrame.h"

class CPMFooAppWx;

class CPMFrameWx: public CPMAppLayer_ns::CPMAppFrame
{
public:
    CPMFrameWx(CPMFooFrame * frame)
    : frame_(frame)
    {
    }

    virtual void show(bool show);
    virtual void close();

protected:
    CPMFooFrame * frame_;
};

#endif // CPMPROCESS_WX
