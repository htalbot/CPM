
#include "test_CPMEventRaiserAppLayer.h"
#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Log_Msg.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"
#include "CPMCore/CPMUtils.h"
#include "CPMCore/CPMTAOLayer.h"

int main (int argc, char * argv[])
{
    // To test RAISE_EVENT when CPMD is not started yet,
    // comment the following if (also, be sure that CPMD
    // is not running)
    if (!CPMUtils_ns::CPMUtils::boot_cpmd())
    {
        ACE_DEBUG((LM_DEBUG, "Failed to boot CPMD."));
        return -1;
    }

    try
    {
        Test_CPMEventRaiserAppLayer app;
        {
            // Declare TAO layer.
            CPMTAOLayer tao_layer;

            // Initialize tao_layer
            if (tao_layer.init(argc, argv))
            {
                // Set tao_layer to app_layer.
                app.tao_layer(&tao_layer);

                try
                {
                    // Run app_layer.
                    if (app.run() != 0)
                    {
                        ACE_DEBUG((LM_ERROR, "Applayer return false.\n"));
                        return -1;
                    }
                }
                catch(...)
                {
                    ACE_DEBUG((LM_ERROR, "failed to get ref on object\n"));
                    return -1;
                }
            }
        }
    }
    catch(...)
    {
        ACE_DEBUG((LM_DEBUG, "Exception with Test_CPMEventRaiserAppLayer\n"));
        return -1;
    }

    return 0;
}

