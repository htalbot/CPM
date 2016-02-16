#ifndef CPM_TAOCLIENT_H
#define CPM_TAOCLIENT_H

#include "CPMCore_Export.h"

// Needed TAO headers
#include "CPMCore/CPMTAOLayerContrib.h"
#include "orbsvcs/CosNamingC.h"

//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
//************************ DON'T USE THIS CLASS WITHIN PLUGIN ****************************
//****************************************************************************************
//****************************************************************************************
//****************************************************************************************
class CPMTAOClient
{
public:

    CPMTAOClient()
    : init_(false)
    {
        printf("Hello\n");
    }
    
    ~CPMTAOClient() 
    {
    }

    void cleanup(void * param)
    {
    }

    bool init_orb(
        int argc,
        ACE_TCHAR *argv[],
        const char *orb_name = 0)
    {
        if (init_)
        {
            return true;
        }

        if (orb_manager_.init(argc, argv, orb_name) == -1)
        {
            return false;
        }

        if (CORBA::is_nil(orb_manager_.orb()))
        {
            return false;
        }

        init_ = true;

        return true;
    }

    template <class Interface>
    typename Interface::_ptr_type get_ref_from_ns(const char * szName)
    {
        CosNaming::Name object_name(1);
        object_name.length (1);
        object_name[0].id = CORBA::string_dup (szName);

        typename Interface::_var_type ref;
        try
        {
            // get a ref on naming service
            CosNaming::NamingContext_var nc = resolve_init<CosNaming::NamingContext>(
                                                        orb_manager_.orb(),
                                                        "NameService");

            // resolve name and get object ref
            CORBA::Object_var obj = resolve_name<Interface>(nc.in(), object_name);
            ref = Interface::_narrow(obj.in());
        }
        catch(...)
        {
            return 0;
        }

        return ref._retn();
    }


    template <class Interface>
    typename Interface::_ptr_type get_ref_from_iortable(const char * szName)
    {
        typename Interface::_var_type ref;
        try
        {
            CORBA::Object_var obj = orb_manager_.orb()->string_to_object(szName);
            ref = Interface::_narrow(obj.in());
        }
        catch(...)
        {
            return 0;
        }

        return ref._retn();
    }


protected:

    bool init_;
    TAO_ORB_Manager orb_manager_;
};

#endif // CPM_TAOCLIENT_H
