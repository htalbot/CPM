#ifdef CPMPROCESS_WX

#include "CPMFrameWx.h"
#include "CPMProcWxAppWx.h"

void CPMFrameWx::show(bool show)
{
    if (frame_)
    {
        frame_->Show(show);
    }
}

void CPMFrameWx::close()
{
    if (!terminated_.value())
    {
        exited_by_corba_ = true;
        delete frame_;
    }
}

#endif // CPMPROCESS_WX
