#ifdef CPMPROCESS_WX

#pragma once

#include "CPMProcWxAppLayer.h"

#include "CPMProcWxFrame.h"

class CPMProcWxAppWx;

class CPMFrameWx: public CPMAppLayer_ns::CPMAppFrame
{
public:
    CPMFrameWx(CPMProcWxFrame * frame)
    : frame_(frame)
    {
    }

    virtual void show(bool show);
    virtual void close();

protected:
    CPMProcWxFrame * frame_;
};

#endif // CPMPROCESS_WX
