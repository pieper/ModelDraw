/*=========================================================================

  Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/

///  vtkITKIslandMath - ITK-based utilities for manipulating connected regions in label maps
/// 
/// vtkITKIslandMath


#ifndef __vtkITKIslandMath_h
#define __vtkITKIslandMath_h


#include "vtkITK.h"
#include "vtkSimpleImageToImageFilter.h"

class VTK_ITK_EXPORT vtkITKIslandMath : public vtkSimpleImageToImageFilter
{
 public:
  static vtkITKIslandMath *New();
  vtkTypeRevisionMacro(vtkITKIslandMath, vtkSimpleImageToImageFilter);
  void PrintSelf(ostream& os, vtkIndent indent);

  /// 
  /// If non-zero, islands are defined by pixels that touch on edges and/or vertices.
  /// If zero, pixels are only considered part of the same island if their faces/edges touch
  vtkGetMacro(FullyConnected, int);
  vtkSetMacro(FullyConnected, int);

  /// 
  /// Minimum island size (in pixels).  Islands smaller than this are ignored.
  vtkGetMacro(MinimumSize, vtkIdType);
  vtkSetMacro(MinimumSize, vtkIdType);

  /// 
  /// Maximum island size (in pixels).  Islands larger than this are ignored.
  vtkGetMacro(MaximumSize, vtkIdType);
  vtkSetMacro(MaximumSize, vtkIdType);

  /// 
  /// TODO: Not yet implemented
  /// If zero, islands are defined by 3D connectivity
  /// If non-zero, islands are evaluated in a sequence of 2D planes
  /// (IJ=3, IK=2, JK=1)
  vtkGetMacro(SliceBySlice, int);
  vtkSetMacro(SliceBySlice, int);
  void SetSliceBySliceToIJ() {this->SetSliceBySlice(3);}
  void SetSliceBySliceToIK() {this->SetSliceBySlice(2);}
  void SetSliceBySliceToJK() {this->SetSliceBySlice(1);}

  /// 
  /// Accessors to describe result of calculations
  vtkGetMacro(NumberOfIslands, unsigned long);
  vtkSetMacro(NumberOfIslands, unsigned long);
  vtkGetMacro(OriginalNumberOfIslands, unsigned long);
  vtkSetMacro(OriginalNumberOfIslands, unsigned long);


protected:
  vtkITKIslandMath();
  ~vtkITKIslandMath();

  virtual void SimpleExecute(vtkImageData* input, vtkImageData* output);

  int FullyConnected;
  int SliceBySlice;
  vtkIdType MinimumSize;
  vtkIdType MaximumSize;

  unsigned long NumberOfIslands;
  unsigned long OriginalNumberOfIslands;
  
private:
  vtkITKIslandMath(const vtkITKIslandMath&);  /// Not implemented.
  void operator=(const vtkITKIslandMath&);  /// Not implemented.
};

#endif




