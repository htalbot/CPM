#ifndef CPM_TAOLAYERCONTRIB_H
#define CPM_TAOLAYERCONTRIB_H

//**********************************************************************
//  This file allows dynamically loaded library to 
//  use CPMUtils too. For example, in CPM, CPMEventHandlerFile loads 
//  CPMUtils to call 'get_cpm_log_dir()' (It's not true anymore but we still
//  need to cope with this issue: to test, see tests/ACE_DLL_vs_PortableServer). 
//  When CPMUtils is loaded, static  variables like 'TAO_Requires_POA_Initializer' and 
//  'TAO_Requires_IORTable_Initializer' are instanciated. If we don't 
//  use this include file in an application where a library is loaded 
//  dynamically, the application crashes when terminating (if this 
//  application doesn't include it yet via CPMTAOLayer.h).
//  The following #includes assures that ACE_Object_Manager
//  manages the static variables like 'TAO_Requires_POA_Initializer'
//  and 'TAO_Requires_IORTable_Initializer'.
//  This file must be particularly included with non-corba applications
//  (not CPMProcess applications) like test_CPMEventRaiser.

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "tao/Utils/ORB_Manager.h"
#include "tao/IORTable/IORTable.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#endif // CPM_TAOLAYERCONTRIB_H
