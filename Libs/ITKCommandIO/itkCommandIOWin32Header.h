/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   ITKCommandIO
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/ITKCommandIO/itkCommandIOWin32Header.h $
  Date:      $Date: 2010-01-22 14:41:51 -0500 (Fri, 22 Jan 2010) $
  Version:   $Revision: 11740 $

==========================================================================*/

///  itkCommandIOWin32Header - manage Windows system differences
/// 
/// The itkCommandIOWin32Header captures some system differences between Unix
/// and Windows operating systems. 

#ifndef __itkCommandIOWin32Header_h
#define __itkCommandIOWin32Header_h

#include <itkCommandIOConfigure.h>

#if defined(WIN32) && !defined(ITKCOMMANDIO_STATIC)
#if defined(CommandIO_EXPORTS)
#define CommandIO_EXPORT __declspec( dllexport ) 
#else
#define CommandIO_EXPORT __declspec( dllimport ) 
#endif
#else
#define CommandIO_EXPORT
#endif

#endif
