
#ifndef __MYDLL_H__
#define __MYDLL_H__

//#include "tao/PortableServer/PortableServer.h"...

//#include "ace/Cleanup.h"

#include "tao/PortableServer/portableserver_export.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "tao/Versioned_Namespace.h"

TAO_BEGIN_VERSIONED_NAMESPACE_DECL

class TAO_PortableServer_Export TAO_POA_Initializer//: public ACE_Cleanup
{
public:
  /// Used to force the initialization of the ORB code.
  static int init (void);
};

static int
TAO_Requires_POA_Initializer = TAO_POA_Initializer::init ();

TAO_END_VERSIONED_NAMESPACE_DECL




#include "mydll/MYDLL_Export.h"
#include "mydll/version.h"

extern "C"
{
    MYDLL_Export void something();
    MYDLL_Export void init();
    MYDLL_Export void fini();
}

namespace Mydll_ns
{
    extern "C" MYDLL_Export void mydll_get_version(int & major, int & minor, int & patch);

    class MYDLL_Export Mydll
    {
    public:
        Mydll(int value);
        void show();

    private:
        int m_value;
    };
} // Mydll_ns namespace


#endif // __MYDLL_H__
