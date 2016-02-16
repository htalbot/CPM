
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl ICPM
// ------------------------------
#ifndef ICPM_EXPORT_H
#define ICPM_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (ICPM_HAS_DLL)
#  define ICPM_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && ICPM_HAS_DLL */

#if !defined (ICPM_HAS_DLL)
#  define ICPM_HAS_DLL 1
#endif /* ! ICPM_HAS_DLL */

#if defined (ICPM_HAS_DLL) && (ICPM_HAS_DLL == 1)
#  if defined (ICPM_BUILD_DLL)
#    define ICPM_Export ACE_Proper_Export_Flag
#    define ICPM_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define ICPM_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* ICPM_BUILD_DLL */
#    define ICPM_Export ACE_Proper_Import_Flag
#    define ICPM_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define ICPM_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* ICPM_BUILD_DLL */
#else /* ICPM_HAS_DLL == 1 */
#  define ICPM_Export
#  define ICPM_SINGLETON_DECLARATION(T)
#  define ICPM_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* ICPM_HAS_DLL == 1 */

// Set ICPM_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (ICPM_NTRACE)
#  if (ACE_NTRACE == 1)
#    define ICPM_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define ICPM_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !ICPM_NTRACE */

#if (ICPM_NTRACE == 1)
#  define ICPM_TRACE(X)
#else /* (ICPM_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define ICPM_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (ICPM_NTRACE == 1) */

#endif /* ICPM_EXPORT_H */

// End of auto generated file.
