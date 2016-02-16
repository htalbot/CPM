
#include "CPMPlayerAppLayer.h"

int main(int argc, char * argv[])
{
    if (!CPMUtils_ns::CPMUtils::boot_cpmd())
    {
        ACE_DEBUG((LM_ERROR, "Can't boot CPMD.\n"));
        return false;
    }

    CPMPlayerAppLayer app_layer;

    bool exception(false);

    try
    {
        CPMTAOLayer tao_layer;

        if (tao_layer.init(argc, argv))
        {
            app_layer.tao_layer(&tao_layer);

            int rc = app_layer.run(argc, argv);
        }
    }
    catch(...)
    {
        ACE_DEBUG((LM_ERROR, "Exception - CPMPlayer::main()\n"));
        exception = true;
    }

    if (exception)
    {
        return -1;
    }

    return 0;
}
