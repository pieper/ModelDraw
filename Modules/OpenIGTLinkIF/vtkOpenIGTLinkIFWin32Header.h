/*==========================================================================

Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkOpenIGTLinkIFWin32Header.h $
Date:      $Date: 2009-08-12 21:30:38 -0400 (Wed, 12 Aug 2009) $
Version:   $Revision: 10236 $

==========================================================================*/

#ifndef __vtkOpenIGTLinkIFWin32Header_h
#define __vtkOpenIGTLinkIFWin32Header_h

#include <vtkOpenIGTLinkIFConfigure.h>

#if defined(WIN32) && !defined(VTKSLICER_STATIC)
#if defined(OpenIGTLinkIF_EXPORTS)
#define VTK_OPENIGTLINKIF_EXPORT __declspec( dllexport ) 
#else
#define VTK_OPENIGTLINKIF_EXPORT __declspec( dllimport ) 
#endif
#else
#define VTK_OPENIGTLINKIF_EXPORT 
#endif
#endif
