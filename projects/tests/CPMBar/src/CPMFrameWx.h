#ifdef CPMPROCESS_WX

#pragma once

#include "CPMBarAppLayer.h"

#include "CPMBarFrame.h"

class CPMBarAppWx;

class CPMFrameWx: public CPMAppLayer_ns::CPMAppFrame
{
public:
    CPMFrameWx(CPMBarFrame * frame)
    : frame_(frame)
    {
    }

    virtual void show(bool show);
    virtual void close();

protected:
    CPMBarFrame * frame_;
};

#endif // CPMPROCESS_WX
