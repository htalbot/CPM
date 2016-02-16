#ifdef CPMPROCESS_WX

#pragma once

#include "CPMTraceEraserAppLayer.h"

#include "CPMTraceEraserFrame.h"

class CPMTraceEraserAppWx;

class CPMFrameWx: public CPMAppLayer_ns::CPMAppFrame
{
public:
    CPMFrameWx(CPMTraceEraserFrame * frame)
    : frame_(frame)
    {
    }

    virtual void show(bool show);
    virtual void close();

protected:
    CPMTraceEraserFrame * frame_;
};

#endif // CPMPROCESS_WX
