/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   vtkITK
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Libs/vtkITK/vtkITKArchetypeImageSeriesScalarReader.h $
  Date:      $Date: 2010-01-22 14:45:00 -0500 (Fri, 22 Jan 2010) $
  Version:   $Revision: 11747 $

==========================================================================*/

#ifndef __vtkITKArchetypeImageSeriesScalarReader_h
#define __vtkITKArchetypeImageSeriesScalarReader_h

#include "vtkITKArchetypeImageSeriesReader.h"

#include "itkImageFileReader.h"

class VTK_ITK_EXPORT vtkITKArchetypeImageSeriesScalarReader : public vtkITKArchetypeImageSeriesReader
{
 public:
  static vtkITKArchetypeImageSeriesScalarReader *New();
  vtkTypeRevisionMacro(vtkITKArchetypeImageSeriesScalarReader,vtkITKArchetypeImageSeriesReader);
  void PrintSelf(ostream& os, vtkIndent indent);

 protected:
  vtkITKArchetypeImageSeriesScalarReader();
  ~vtkITKArchetypeImageSeriesScalarReader();

  void ExecuteData(vtkDataObject *data);
//BTX
  static void ReadProgressCallback(itk::ProcessObject* obj,const itk::ProgressEvent&, void* data);
//ETX
  /// private:
};

#endif
