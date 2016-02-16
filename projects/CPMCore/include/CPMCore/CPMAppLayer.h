
#ifndef __CPMAPPLAYER_H__
#define __CPMAPPLAYER_H__

#include "CPMCore/CPMCORE_Export.h"

#include "ACEWin64Warnings/BeginSuppressWarnings.h"
#include "ace/Singleton.h"
#include "ace/Synch.h"
#include "ICPM/ICPMS.h"
#include "ACEWin64Warnings/EndSuppressWarnings.h"

#include "CPMCore/CPMTAOLayer.h"
#include "CPMCore/CPMO_i.h"
#include "DPOConsole/DPOConsole.h"

namespace CPMAppLayer_ns
{
    class CPMAppFrame
    {
    public:
        CPMAppFrame()
        : exited_by_corba_(false)
        , terminated_(false)
        {
        }

        virtual bool exited_by_corba()
        {
            return exited_by_corba_;
        }

        virtual void exited_by_corba(bool flag)
        {
            exited_by_corba_ = flag;
        }

        virtual void terminated()
        {
            terminated_ = true;
        }

        virtual void show(bool show) = 0;
        virtual void close() = 0;

    protected:
        ACE_Atomic_Op<ACE_Recursive_Thread_Mutex, bool> terminated_;
        bool exited_by_corba_;
    };


    class CPMCORE_Export CPMAppLayerBase:
        public ::ICPM_module::ICPMProcess
    {
    public:
        CPMAppLayerBase();
        virtual ~CPMAppLayerBase();
        virtual void cpmo_id(const std::string & cpmo_id);
        virtual std::string cpmo_id();
        virtual void tao_layer(CPMTAOLayer * layer);
        virtual CPMTAOLayer * tao_layer();
        virtual void ctrl_end();
        virtual void ctrl_ping();
        virtual void show_console(::CORBA::Boolean show);
        virtual void show_gui(::CORBA::Boolean show);
        virtual void as_server(::CORBA::Boolean as_server);
        virtual void as_gui(::CORBA::Boolean as_gui);
        virtual void object_list(::ICPM_module::ObjectDescSeq_out list);
        virtual bool objects_register() = 0;
        virtual bool objects_init() = 0;
        virtual void terminate_() = 0;
        
        virtual inline void terminating() { terminating_ = true; }
        virtual inline bool is_terminating() { return terminating_.value(); }

        virtual void set_frame(CPMAppFrame * frame) { cpm_frame_ = frame; }
        virtual bool as_gui() { return as_gui_; }
        virtual bool as_server() { return as_server_; }
        virtual void show_console_on_init();

    protected:
        std::string cpmo_id_;
        CPMO_i obj_impl_;
        CPMTAOLayer * tao_layer_;
        std::vector<std::string> objects_ids_;
        ACE_Recursive_Thread_Mutex mutex_;
        ACE_Atomic_Op<ACE_Recursive_Thread_Mutex, bool> terminating_;
        DPOConsole_ns::DPOConsole * console_;

        CPMAppFrame * cpm_frame_;
        bool as_gui_;
        bool as_server_;
    };

    class CPMCORE_Export CPMAppLayer
    {
        friend ACE_Singleton<CPMAppLayer_ns::CPMAppLayer, ACE_Recursive_Thread_Mutex>;

    public:
        void set(CPMAppLayerBase * app_layer);
        CPMAppLayerBase * get();

    protected:
        static CPMAppLayerBase * app_layer_;

    protected:
        CPMAppLayer();
        ~CPMAppLayer();
        //static void destroy(); // Defining destroy this way doesn't work. So, for those who are tempted to, don't do that!

        ACE_Recursive_Thread_Mutex mutex_;
    };
} // CPMAppLayer_ns namespace

typedef ACE_Singleton<CPMAppLayer_ns::CPMAppLayer, ACE_Recursive_Thread_Mutex> CPMAppLayerSingleton;

#ifdef ACE_WIN32
    #pragma warning(push)
    #pragma warning(disable: 4231) // will disapear when VC is conform to C++11
#endif
    CPMCORE_SINGLETON_DECLARE(ACE_Singleton, CPMAppLayer_ns::CPMAppLayer, ACE_Recursive_Thread_Mutex);
#ifdef ACE_WIN32
    #pragma warning(push)
#endif


#define CPMAPPLAYER CPMAppLayerSingleton::instance()
#define CPMTAOLAYER CPMAPPLAYER->get()->tao_layer()

// Since CPMAPPLAYER (or its derived classes) are called
// in the main thread (at the level of AppLayerBase
// inherited class' constructor), it's not considered
// necessary to declare a kind of INIT_APPLAYER.

//--------------------------------------------------------------------
// my_typeof
template<int N> struct typeof_class;
template<class T> struct WrapType { typedef T U; };

#define REGISTER_TYPEOF(N,T) \
    template<> struct typeof_class<N> { typedef WrapType<T>::U V; }; \
    char (*typeof_fct(const WrapType<T>::U &))[N];

#define my_typeof(x) typeof_class<sizeof(*typeof_fct(x))>::V
//--------------------------------------------------------------------

template<class T>
class ObjToRegister
{
public:
    ObjToRegister(T & obj,
                    const std::string & name,
                    const std::string & name_AOT)
    : obj_(obj)
    , name_(name)
    , name_AOT_(name_AOT)
    {
    }

    typename T & obj_;
    std::string name_;
    std::string name_AOT_;

    void register_with_ior_table(std::vector<std::string> & objects_ids)
    {
        CPMTAOLAYER->register_with_ior_table<my_typeof(obj_), ICPM_module::ICPMObject>
            (&obj_,
                name_AOT_.c_str(),
                name_.c_str());

        objects_ids.push_back(name_);
    }
};


#endif // __CPMAPPLAYER_H__
