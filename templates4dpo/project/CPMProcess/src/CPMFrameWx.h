#ifdef CPMPROCESS_WX

#pragma once

#include "xyzAppLayer.h"

#include "xyzFrame.h"

class XyzAppWx;

class CPMFrameWx: public CPMAppLayer_ns::CPMAppFrame
{
public:
    CPMFrameWx(XyzFrame * frame)
    : frame_(frame)
    {
    }

    virtual void show(bool show);
    virtual void close();

protected:
    XyzFrame * frame_;
};

#endif // CPMPROCESS_WX
