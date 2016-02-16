
// -*- C++ -*-
// $Id$
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl MYDLL
// ------------------------------
#ifndef MYDLL_EXPORT_H
#define MYDLL_EXPORT_H

#include "ace/config-all.h"

#if defined (ACE_AS_STATIC_LIBS) && !defined (MYDLL_HAS_DLL)
#  define MYDLL_HAS_DLL 0
#endif /* ACE_AS_STATIC_LIBS && MYDLL_HAS_DLL */

#if !defined (MYDLL_HAS_DLL)
#  define MYDLL_HAS_DLL 1
#endif /* ! MYDLL_HAS_DLL */

#if defined (MYDLL_HAS_DLL) && (MYDLL_HAS_DLL == 1)
#  if defined (MYDLL_BUILD_DLL)
#    define MYDLL_Export ACE_Proper_Export_Flag
#    define MYDLL_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define MYDLL_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* MYDLL_BUILD_DLL */
#    define MYDLL_Export ACE_Proper_Import_Flag
#    define MYDLL_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define MYDLL_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* MYDLL_BUILD_DLL */
#else /* MYDLL_HAS_DLL == 1 */
#  define MYDLL_Export
#  define MYDLL_SINGLETON_DECLARATION(T)
#  define MYDLL_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* MYDLL_HAS_DLL == 1 */

// Set MYDLL_NTRACE = 0 to turn on library specific tracing even if
// tracing is turned off for ACE.
#if !defined (MYDLL_NTRACE)
#  if (ACE_NTRACE == 1)
#    define MYDLL_NTRACE 1
#  else /* (ACE_NTRACE == 1) */
#    define MYDLL_NTRACE 0
#  endif /* (ACE_NTRACE == 1) */
#endif /* !MYDLL_NTRACE */

#if (MYDLL_NTRACE == 1)
#  define MYDLL_TRACE(X)
#else /* (MYDLL_NTRACE == 1) */
#  if !defined (ACE_HAS_TRACE)
#    define ACE_HAS_TRACE
#  endif /* ACE_HAS_TRACE */
#  define MYDLL_TRACE(X) ACE_TRACE_IMPL(X)
#  include "ace/Trace.h"
#endif /* (MYDLL_NTRACE == 1) */

#endif /* MYDLL_EXPORT_H */

// End of auto generated file.
