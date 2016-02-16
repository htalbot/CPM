
#ifndef CPMOBJDUMMY_I_H
#define CPMOBJDUMMY_I_H

#include "CPMOBJDUMMY_Export.h"
#include "version.h"

#include "ICPM/ICPMS.h"
#include "ICPM/ICPMC.h"

extern "C" CPMOBJDUMMY_Export void CPMObjDummy_get_version(int & major, int & minor, int & patch);

class CPMOBJDUMMY_Export CPMObjDummy_i : public POA_ICPM_module::ICPMObject
{
public:
    CPMObjDummy_i();
    ~CPMObjDummy_i();

    virtual void get_version (
      ::CORBA::Long_out major,
      ::CORBA::Long_out minor,
      ::CORBA::Long_out patch);

    virtual void ctrl_ping (
      void);

    virtual CORBA::Long get();

    bool init_i();
};

#endif /* CPMOBJDUMMY_I_H */


