// -*- C++ -*-
// $Id$

/**
 * Code generated by the The ACE ORB (TAO) IDL Compiler v2.3.1
 * TAO and the TAO IDL Compiler have been developed by:
 *       Center for Distributed Object Computing
 *       Washington University
 *       St. Louis, MO
 *       USA
 *       http://www.cs.wustl.edu/~schmidt/doc-center.html
 * and
 *       Distributed Object Computing Laboratory
 *       University of California at Irvine
 *       Irvine, CA
 *       USA
 * and
 *       Institute for Software Integrated Systems
 *       Vanderbilt University
 *       Nashville, TN
 *       USA
 *       http://www.isis.vanderbilt.edu/
 *
 * Information about TAO is available at:
 *     http://www.dre.vanderbilt.edu/~schmidt/TAO.html
 **/

// TAO_IDL - Generated from
// be\be_codegen.cpp:715

#ifndef _TAO_IDL_ICPMS_T_EVJNXZ_CPP_
#define _TAO_IDL_ICPMS_T_EVJNXZ_CPP_

#include "ICPMS_T.h"

// TAO_IDL - Generated from
// be\be_visitor_interface\tie_ss.cpp:82

template <class T>
POA_ICPM_module::ICPMProcess_tie<T>::ICPMProcess_tie (T &t)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMProcess_tie<T>::ICPMProcess_tie (T &t, ::PortableServer::POA_ptr poa)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMProcess_tie<T>::ICPMProcess_tie (T *tp, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMProcess_tie<T>::ICPMProcess_tie (T *tp, ::PortableServer::POA_ptr poa, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMProcess_tie<T>::~ICPMProcess_tie (void)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
}

template <class T> T *
POA_ICPM_module::ICPMProcess_tie<T>::_tied_object (void)
{
  return this->ptr_;
}

template <class T> void
POA_ICPM_module::ICPMProcess_tie<T>::_tied_object (T &obj)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = &obj;
  this->rel_ = false;
}

template <class T> void
POA_ICPM_module::ICPMProcess_tie<T>::_tied_object (T *obj, ::CORBA::Boolean release)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = obj;
  this->rel_ = release;
}

template <class T>  ::CORBA::Boolean
POA_ICPM_module::ICPMProcess_tie<T>::_is_owner (void)
{
  return this->rel_;
}

template <class T> void
POA_ICPM_module::ICPMProcess_tie<T>::_is_owner ( ::CORBA::Boolean b)
{
  this->rel_ = b;
}

template <class T> PortableServer::POA_ptr
POA_ICPM_module::ICPMProcess_tie<T>::_default_POA ()
{
  if (! ::CORBA::is_nil (this->poa_.in ()))
    {
      return ::PortableServer::POA::_duplicate (this->poa_.in ());
    }
  
  return this->ICPMProcess::_default_POA ();
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::object_list  (
  ::ICPM_module::ObjectDescSeq_out list)
{
  this->ptr_->object_list (
    list
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::ctrl_end  (
  void)
{
  this->ptr_->ctrl_end (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::ctrl_ping  (
  void)
{
  this->ptr_->ctrl_ping (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::show_console  (
  ::CORBA::Boolean show)
{
  this->ptr_->show_console (
    show
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::show_gui  (
  ::CORBA::Boolean show)
{
  this->ptr_->show_gui (
    show
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::as_server  (
  ::CORBA::Boolean flag)
{
  this->ptr_->as_server (
    flag
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMProcess_tie<T>::as_gui  (
  ::CORBA::Boolean flag)
{
  this->ptr_->as_gui (
    flag
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Long POA_ICPM_module::ICPMProcess_tie<T>::getpid  (
  void)
{
  return this->ptr_->getpid (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_interface\tie_ss.cpp:82

template <class T>
POA_ICPM_module::ICPMD_tie<T>::ICPMD_tie (T &t)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMD_tie<T>::ICPMD_tie (T &t, ::PortableServer::POA_ptr poa)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMD_tie<T>::ICPMD_tie (T *tp, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMD_tie<T>::ICPMD_tie (T *tp, ::PortableServer::POA_ptr poa, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMD_tie<T>::~ICPMD_tie (void)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
}

template <class T> T *
POA_ICPM_module::ICPMD_tie<T>::_tied_object (void)
{
  return this->ptr_;
}

template <class T> void
POA_ICPM_module::ICPMD_tie<T>::_tied_object (T &obj)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = &obj;
  this->rel_ = false;
}

template <class T> void
POA_ICPM_module::ICPMD_tie<T>::_tied_object (T *obj, ::CORBA::Boolean release)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = obj;
  this->rel_ = release;
}

template <class T>  ::CORBA::Boolean
POA_ICPM_module::ICPMD_tie<T>::_is_owner (void)
{
  return this->rel_;
}

template <class T> void
POA_ICPM_module::ICPMD_tie<T>::_is_owner ( ::CORBA::Boolean b)
{
  this->rel_ = b;
}

template <class T> PortableServer::POA_ptr
POA_ICPM_module::ICPMD_tie<T>::_default_POA ()
{
  if (! ::CORBA::is_nil (this->poa_.in ()))
    {
      return ::PortableServer::POA::_duplicate (this->poa_.in ());
    }
  
  return this->ICPMD::_default_POA ();
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMD_tie<T>::start  (
  const ::ICPM_module::ProcessSeq & procs)
{
  return this->ptr_->start (
    procs
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMD_tie<T>::stop  (
  const ::ICPM_module::ProcessSeq & procs)
{
  return this->ptr_->stop (
    procs
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMD_tie<T>::show_console  (
  const ::ICPM_module::ProcessSeq & procs,
  ::CORBA::Boolean show)
{
  this->ptr_->show_console (
    procs,
    show
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMD_tie<T>::show_gui  (
  const ::ICPM_module::ProcessSeq & procs,
  ::CORBA::Boolean show)
{
  this->ptr_->show_gui (
    procs,
    show
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMD_tie<T>::get_launched_processes  (
  ::ICPM_module::ProcessSeq_out processes)
{
  this->ptr_->get_launched_processes (
    processes
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMD_tie<T>::on_new_getting_reference_timeout  (
  ::CORBA::Long loop_timeout_ms,
  ::CORBA::Long attempts)
{
  this->ptr_->on_new_getting_reference_timeout (
    loop_timeout_ms,
    attempts
  );
}

// TAO_IDL - Generated from
// be\be_visitor_interface\tie_ss.cpp:82

template <class T>
POA_ICPM_module::ICPMObject_tie<T>::ICPMObject_tie (T &t)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMObject_tie<T>::ICPMObject_tie (T &t, ::PortableServer::POA_ptr poa)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMObject_tie<T>::ICPMObject_tie (T *tp, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMObject_tie<T>::ICPMObject_tie (T *tp, ::PortableServer::POA_ptr poa, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMObject_tie<T>::~ICPMObject_tie (void)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
}

template <class T> T *
POA_ICPM_module::ICPMObject_tie<T>::_tied_object (void)
{
  return this->ptr_;
}

template <class T> void
POA_ICPM_module::ICPMObject_tie<T>::_tied_object (T &obj)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = &obj;
  this->rel_ = false;
}

template <class T> void
POA_ICPM_module::ICPMObject_tie<T>::_tied_object (T *obj, ::CORBA::Boolean release)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = obj;
  this->rel_ = release;
}

template <class T>  ::CORBA::Boolean
POA_ICPM_module::ICPMObject_tie<T>::_is_owner (void)
{
  return this->rel_;
}

template <class T> void
POA_ICPM_module::ICPMObject_tie<T>::_is_owner ( ::CORBA::Boolean b)
{
  this->rel_ = b;
}

template <class T> PortableServer::POA_ptr
POA_ICPM_module::ICPMObject_tie<T>::_default_POA ()
{
  if (! ::CORBA::is_nil (this->poa_.in ()))
    {
      return ::PortableServer::POA::_duplicate (this->poa_.in ());
    }
  
  return this->ICPMObject::_default_POA ();
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMObject_tie<T>::get_version  (
  ::CORBA::Long_out major,
  ::CORBA::Long_out minor,
  ::CORBA::Long_out patch)
{
  this->ptr_->get_version (
    major,
    minor,
    patch
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMObject_tie<T>::ctrl_ping  (
  void)
{
  this->ptr_->ctrl_ping (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_interface\tie_ss.cpp:82

template <class T>
POA_ICPM_module::ICPMM_tie<T>::ICPMM_tie (T &t)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMM_tie<T>::ICPMM_tie (T &t, ::PortableServer::POA_ptr poa)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMM_tie<T>::ICPMM_tie (T *tp, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMM_tie<T>::ICPMM_tie (T *tp, ::PortableServer::POA_ptr poa, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMM_tie<T>::~ICPMM_tie (void)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
}

template <class T> T *
POA_ICPM_module::ICPMM_tie<T>::_tied_object (void)
{
  return this->ptr_;
}

template <class T> void
POA_ICPM_module::ICPMM_tie<T>::_tied_object (T &obj)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = &obj;
  this->rel_ = false;
}

template <class T> void
POA_ICPM_module::ICPMM_tie<T>::_tied_object (T *obj, ::CORBA::Boolean release)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = obj;
  this->rel_ = release;
}

template <class T>  ::CORBA::Boolean
POA_ICPM_module::ICPMM_tie<T>::_is_owner (void)
{
  return this->rel_;
}

template <class T> void
POA_ICPM_module::ICPMM_tie<T>::_is_owner ( ::CORBA::Boolean b)
{
  this->rel_ = b;
}

template <class T> PortableServer::POA_ptr
POA_ICPM_module::ICPMM_tie<T>::_default_POA ()
{
  if (! ::CORBA::is_nil (this->poa_.in ()))
    {
      return ::PortableServer::POA::_duplicate (this->poa_.in ());
    }
  
  return this->ICPMM::_default_POA ();
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMM_tie<T>::get_settings  (
  ::ICPM_module::CPMMSettings_out initial,
  ::ICPM_module::CPMMSettings_out current,
  ::CORBA::String_out current_settings_file)
{
  this->ptr_->get_settings (
    initial,
    current,
    current_settings_file
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMM_tie<T>::start  (
  const ::ICPM_module::ProcessSeq & procs)
{
  return this->ptr_->start (
    procs
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMM_tie<T>::stop  (
  const ::ICPM_module::ProcessSeq & procs)
{
  return this->ptr_->stop (
    procs
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMM_tie<T>::show_console  (
  const ::ICPM_module::ProcessSeq & procs,
  ::CORBA::Boolean show)
{
  this->ptr_->show_console (
    procs,
    show
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMM_tie<T>::show_gui  (
  const ::ICPM_module::ProcessSeq & procs,
  ::CORBA::Boolean show)
{
  this->ptr_->show_gui (
    procs,
    show
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMM_tie<T>::started  (
  void)
{
  return this->ptr_->started (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMM_tie<T>::get_unavailable  (
  ::ICPM_module::UnavailableElementIDSeq_out nodes,
  ::ICPM_module::UnavailableElementIDSeq_out processes)
{
  return this->ptr_->get_unavailable (
    nodes,
    processes
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMM_tie<T>::update_settings  (
  const ::ICPM_module::CPMMSettings & new_initial_settings,
  const ::ICPM_module::CPMMSettings & new_current_settings,
  const char * file)
{
  this->ptr_->update_settings (
    new_initial_settings,
    new_current_settings,
    file
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMM_tie<T>::save_settings_to  (
  const char * target_file)
{
  return this->ptr_->save_settings_to (
    target_file
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
::CORBA::Boolean POA_ICPM_module::ICPMM_tie<T>::set_as_default  (
  void)
{
  return this->ptr_->set_as_default (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_interface\tie_ss.cpp:82

template <class T>
POA_ICPM_module::ICPMEventMgr_tie<T>::ICPMEventMgr_tie (T &t)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMEventMgr_tie<T>::ICPMEventMgr_tie (T &t, ::PortableServer::POA_ptr poa)
  : ptr_ (&t),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (false)
{}

template <class T>
POA_ICPM_module::ICPMEventMgr_tie<T>::ICPMEventMgr_tie (T *tp, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_nil ()),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMEventMgr_tie<T>::ICPMEventMgr_tie (T *tp, ::PortableServer::POA_ptr poa, ::CORBA::Boolean release)
  : ptr_ (tp),
    poa_ ( ::PortableServer::POA::_duplicate (poa)),
    rel_ (release)
{}

template <class T>
POA_ICPM_module::ICPMEventMgr_tie<T>::~ICPMEventMgr_tie (void)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
}

template <class T> T *
POA_ICPM_module::ICPMEventMgr_tie<T>::_tied_object (void)
{
  return this->ptr_;
}

template <class T> void
POA_ICPM_module::ICPMEventMgr_tie<T>::_tied_object (T &obj)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = &obj;
  this->rel_ = false;
}

template <class T> void
POA_ICPM_module::ICPMEventMgr_tie<T>::_tied_object (T *obj, ::CORBA::Boolean release)
{
  if (this->rel_)
    {
      delete this->ptr_;
    }
  
  this->ptr_ = obj;
  this->rel_ = release;
}

template <class T>  ::CORBA::Boolean
POA_ICPM_module::ICPMEventMgr_tie<T>::_is_owner (void)
{
  return this->rel_;
}

template <class T> void
POA_ICPM_module::ICPMEventMgr_tie<T>::_is_owner ( ::CORBA::Boolean b)
{
  this->rel_ = b;
}

template <class T> PortableServer::POA_ptr
POA_ICPM_module::ICPMEventMgr_tie<T>::_default_POA ()
{
  if (! ::CORBA::is_nil (this->poa_.in ()))
    {
      return ::PortableServer::POA::_duplicate (this->poa_.in ());
    }
  
  return this->ICPMEventMgr::_default_POA ();
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMEventMgr_tie<T>::get_version  (
  ::CORBA::Long_out major,
  ::CORBA::Long_out minor,
  ::CORBA::Long_out patch)
{
  this->ptr_->get_version (
    major,
    minor,
    patch
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMEventMgr_tie<T>::ctrl_ping  (
  void)
{
  this->ptr_->ctrl_ping (
  );
}

// TAO_IDL - Generated from
// be\be_visitor_operation\tie_ss.cpp:95

template <class T>
void POA_ICPM_module::ICPMEventMgr_tie<T>::push  (
  const ::IBASICEVENTCORBA_module::BasicEventCorba & event)
{
  this->ptr_->push (
    event
  );
}
#endif /* ifndef */

