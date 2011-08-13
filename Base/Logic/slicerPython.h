/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Base/Logic/slicerPython.h $
  Date:      $Date: 2011-07-31 11:28:27 -0400 (Sun, 31 Jul 2011) $
  Version:   $Revision: 16775 $

==========================================================================*/

//
/// workarounds to incorporate python into slicer's build without warnings
//

#ifdef Slicer3_USE_PYTHON

/// Python will define _LARGEFILE_SOURCE to 1, while kwsys simply defines it - undef it here
/// to avoid getting a warning during the build

#  ifdef _LARGEFILE_SOURCE
#  undef _LARGEFILE_SOURCE
#  endif 

#  undef _XOPEN_SOURCE
#  undef _POSIX_C_SOURCE

/// If debug, Python wants pythonxx_d.lib, so fake it out

#  ifdef _DEBUG
#    undef _DEBUG
#    include <Python.h>
#    define _DEBUG
#  else
#    include <Python.h>
#  endif

#endif
