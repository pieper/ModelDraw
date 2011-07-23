/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageLabelChange.h,v $
  Date:      $Date: 2010-02-20 15:39:37 -0500 (Sat, 20 Feb 2010) $
  Version:   $Revision: 12195 $

=========================================================================auto=*/
///  vtkImageLabelChange -  Change one label value to another
/// 
/// 
//
/// vtkImageLabelChange is will replace one voxel value with another.
/// This is used for editing of labelmaps.
//

#ifndef __vtkImageLabelChange_h
#define __vtkImageLabelChange_h

#include "vtkImageToImageFilter.h"
#include "vtkSlicerBaseLogic.h"

class vtkImageData;
class VTK_SLICER_BASE_LOGIC_EXPORT vtkImageLabelChange : public vtkImageToImageFilter
{
public:
    static vtkImageLabelChange *New();
    vtkTypeRevisionMacro(vtkImageLabelChange,vtkImageToImageFilter);
    void PrintSelf(ostream& os, vtkIndent indent);

    vtkSetMacro(InputLabel, float);
    vtkGetMacro(InputLabel, float);

    vtkSetMacro(OutputLabel, float);
    vtkGetMacro(OutputLabel, float);

protected:
    vtkImageLabelChange();
    ~vtkImageLabelChange() {};
    vtkImageLabelChange(const vtkImageLabelChange&);
    void operator=(const vtkImageLabelChange&);

    float InputLabel;
    float OutputLabel;

    void ThreadedExecute(vtkImageData *inData, vtkImageData *outData, 
        int extent[6], int id);
};

#endif

