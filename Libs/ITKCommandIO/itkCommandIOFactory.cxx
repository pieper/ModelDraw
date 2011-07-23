/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   ITKCommandIO
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/ITKCommandIO/itkCommandIOFactory.cxx $
  Date:      $Date: 2006-12-21 07:33:20 -0500 (Thu, 21 Dec 2006) $
  Version:   $Revision: 1901 $

==========================================================================*/
#include "itkCommandIOFactory.h"
#include "itkCreateObjectFunction.h"
#include "itkCommandIO.h"
#include "itkVersion.h"

  
namespace itk
{
CommandIOFactory::CommandIOFactory()
{
  this->RegisterOverride("itkImageIOBase",
                         "itkCommandIO",
                         "Command IO",
                         1,
                         CreateObjectFunction<CommandIO>::New());
}
  
CommandIOFactory::~CommandIOFactory()
{
}

const char* 
CommandIOFactory::GetITKSourceVersion(void) const
{
  return ITK_SOURCE_VERSION;
}

const char* 
CommandIOFactory::GetDescription() const
{
  return "Command ImageIO Factory, allows the loading of Nrrd images into insight";
}

} // end namespace itk

