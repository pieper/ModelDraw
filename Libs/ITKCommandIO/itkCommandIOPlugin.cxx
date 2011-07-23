/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   ITKCommandIO
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/ITKCommandIO/itkCommandIOPlugin.cxx $
  Date:      $Date: 2008-01-14 13:18:54 -0500 (Mon, 14 Jan 2008) $
  Version:   $Revision: 5492 $

==========================================================================*/
#include "itkCommandIOPlugin.h"
#include "itkCommandIOFactory.h"

/**
 * Routine that is called when the shared library is loaded by
 * itk::ObjectFactoryBase::LoadDynamicFactories().
 *
 * itkLoad() is C (not C++) function.
 */
itk::ObjectFactoryBase* itkLoad()
{
  static itk::CommandIOFactory::Pointer f = itk::CommandIOFactory::New();
  return f;
}

