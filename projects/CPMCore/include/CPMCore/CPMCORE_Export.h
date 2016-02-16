
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl CPMCORE
// ------------------------------
#ifndef CPMCORE_EXPORT_H
#define CPMCORE_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (CPMCORE_HAS_DLL)
#  define CPMCORE_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && CPMCORE_HAS_DLL */

#if !defined (CPMCORE_HAS_DLL)
#  define CPMCORE_HAS_DLL 1
#endif /* ! CPMCORE_HAS_DLL */

#if defined (CPMCORE_HAS_DLL) && (CPMCORE_HAS_DLL == 1)
#  if defined (CPMCORE_BUILD_DLL)
#    define CPMCORE_Export ACE_Proper_Export_Flag
#    define CPMCORE_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define CPMCORE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* CPMCORE_BUILD_DLL */
#    define CPMCORE_Export ACE_Proper_Import_Flag
#    define CPMCORE_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define CPMCORE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* CPMCORE_BUILD_DLL */
#else /* CPMCORE_HAS_DLL == 1 */
#  define CPMCORE_Export
#  define CPMCORE_SINGLETON_DECLARATION(T)
#  define CPMCORE_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* CPMCORE_HAS_DLL == 1 */

// Set CPMCORE_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (CPMCORE_NTRACE)
#  if (ACE_NTRACE == 1)
#    define CPMCORE_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define CPMCORE_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !CPMCORE_NTRACE */

#if (CPMCORE_NTRACE == 1)
#  define CPMCORE_TRACE(X)
#else /* (CPMCORE_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define CPMCORE_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (CPMCORE_NTRACE == 1) */

#endif /* CPMCORE_EXPORT_H */

// End of auto generated file.
