/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkImageLabelOutline.h,v $
  Date:      $Date: 2010-02-20 15:39:37 -0500 (Sat, 20 Feb 2010) $
  Version:   $Revision: 12195 $

=========================================================================auto=*/
///  vtkImageLabelOutline -  Display labelmap outlines
/// 
///  Used  in slicer for the Label layer to outline the segmented
///  structures (instead of showing them filled-in).

#ifndef __vtkImageLabelOutline_h
#define __vtkImageLabelOutline_h

#include "vtkImageNeighborhoodFilter.h"
#include "vtkSlicerBaseLogic.h"

class vtkImageData;
class VTK_SLICER_BASE_LOGIC_EXPORT vtkImageLabelOutline : public vtkImageNeighborhoodFilter
{
public:
  static vtkImageLabelOutline *New();
  vtkTypeRevisionMacro(vtkImageLabelOutline,vtkImageNeighborhoodFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// 
  /// background pixel value in the image (usually 0)
  vtkSetMacro(Background, float);
  vtkGetMacro(Background, float);

  /// 
  /// not used (don't know what it was intended for)
  vtkSetMacro(Outline, int);
  vtkGetMacro(Outline, int);

protected:
  vtkImageLabelOutline();
  ~vtkImageLabelOutline();

  float Background;
  int Outline;

  void ThreadedExecute(vtkImageData *inData, vtkImageData *outData,
    int extent[6], int id);
};

#endif



