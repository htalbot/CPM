
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl CPMOBJDUMMY
// ------------------------------
#ifndef CPMOBJDUMMY_EXPORT_H
#define CPMOBJDUMMY_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (CPMOBJDUMMY_HAS_DLL)
#  define CPMOBJDUMMY_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && CPMOBJDUMMY_HAS_DLL */

#if !defined (CPMOBJDUMMY_HAS_DLL)
#  define CPMOBJDUMMY_HAS_DLL 1
#endif /* ! CPMOBJDUMMY_HAS_DLL */

#if defined (CPMOBJDUMMY_HAS_DLL) && (CPMOBJDUMMY_HAS_DLL == 1)
#  if defined (CPMOBJDUMMY_BUILD_DLL)
#    define CPMOBJDUMMY_Export ACE_Proper_Export_Flag
#    define CPMOBJDUMMY_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define CPMOBJDUMMY_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* CPMOBJDUMMY_BUILD_DLL */
#    define CPMOBJDUMMY_Export ACE_Proper_Import_Flag
#    define CPMOBJDUMMY_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define CPMOBJDUMMY_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* CPMOBJDUMMY_BUILD_DLL */
#else /* CPMOBJDUMMY_HAS_DLL == 1 */
#  define CPMOBJDUMMY_Export
#  define CPMOBJDUMMY_SINGLETON_DECLARATION(T)
#  define CPMOBJDUMMY_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* CPMOBJDUMMY_HAS_DLL == 1 */

// Set CPMOBJDUMMY_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (CPMOBJDUMMY_NTRACE)
#  if (ACE_NTRACE == 1)
#    define CPMOBJDUMMY_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define CPMOBJDUMMY_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !CPMOBJDUMMY_NTRACE */

#if (CPMOBJDUMMY_NTRACE == 1)
#  define CPMOBJDUMMY_TRACE(X)
#else /* (CPMOBJDUMMY_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define CPMOBJDUMMY_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (CPMOBJDUMMY_NTRACE == 1) */

#endif /* CPMOBJDUMMY_EXPORT_H */

// End of auto generated file.
