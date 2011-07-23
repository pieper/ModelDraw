/*=auto=========================================================================

(c) Copyright 2001 Massachusetts Institute of Technology

Permission is hereby granted, without payment, to copy, modify, display
and distribute this software and its documentation, if any, for any purpose,
provided that the above copyright notice and the following three paragraphs
appear on all copies of this software.  Use of this software constitutes
acceptance of these terms and conditions.

IN NO EVENT SHALL MIT BE LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS SOFTWARE
AND ITS DOCUMENTATION, EVEN IF MIT HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

MIT SPECIFICALLY DISCLAIMS ANY EXPRESS OR IMPLIED WARRANTIES INCLUDING,
BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR
A PARTICULAR PURPOSE, AND NON-INFRINGEMENT.

THE SOFTWARE IS PROVIDED "AS IS."  MIT HAS NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.

=========================================================================auto=*/
// .NAME vtkImagePCAFilter
// This is a generic class for vtkImagePCATraining and vtkImagePCAAnalysis
#ifndef __vtkImagePCAFilter_h
#define __vtkImagePCAFilter_h

#include "vtkAtlasCreatorCxxModuleWin32Header.h"

// #include "vtkSlicer.h"
#include "vtkImageMultipleInputFilter.h"
#include "vtkFloatArray.h"

class VTK_ATLASCREATORCXXMODULE_EXPORT vtkImagePCAFilter : public vtkImageMultipleInputFilter
{
  public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImagePCAFilter *New();
  vtkTypeMacro(vtkImagePCAFilter,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInputIndex(int index, vtkImageData *image) {this->SetInput(index,image);}

  vtkGetMacro(NumberOfEigenModes, int);

  vtkGetMacro(XDim, int);
  vtkGetMacro(YDim, int);
  vtkGetMacro(ZDim, int);

  float GetEigenValue(int index) {
    if (index < this->NumberOfEigenModes) return this->EigenValues[index];
    else {
      vtkWarningMacro( << "GetEigenValue: Index exceeds dimension of EigenValue" );
      return 0;
    }
  }
  void SetEigenValue(int index, float value) {
     if (index < this->NumberOfEigenModes) this->EigenValues[index] = value;
     else vtkWarningMacro( << "SetEigenValue: Index (" << index << ") exceeds Number of EigenModes ("<< this->NumberOfEigenModes << ") !" );
  }

  void TransfereLabelmapIntoPCADistanceMap(vtkImageData *inData, int Label, float MaxDistanceValue, float BoundaryValue, vtkImageData *outData);

  // Checks the Input if everything is set correctly
  int CheckInput(vtkImageData **InData);
  int CheckInput(vtkImageData *InData);

protected:
  vtkImagePCAFilter();
  vtkImagePCAFilter(const vtkImagePCAFilter&) {};
  ~vtkImagePCAFilter();

  void operator=(const vtkImagePCAFilter&) {};
  void ThreadedExecute(vtkImageData **inData, vtkImageData *outData,int outExt[6], int id){};

  void DeleteEigenValues();

  void SetNumberOfEigenModes(int init) {
    this->DeleteEigenValues();
    this->NumberOfEigenModes =  init;
    this->EigenValues = new float[init];
    memset(this->EigenValues,0,sizeof(float)*init);
  }

  int CheckEigenValues() {
    int i = 0;
    while ((i < this->NumberOfEigenModes) && (this->EigenValues[i] > 0.0)) i++;
    return (i < this->NumberOfEigenModes ? 1 : 0);
  }

  int NumberOfEigenModes;
  float *EigenValues;

  // Save size so that everytime it is the same
  int XDim;
  int YDim;
  int ZDim;
  int InputScalarType;

};

//BTX
//------------------------------------------------------------------------
// Matrix ops. Some taken from vtkThinPlateSplineTransform.cxx
static inline double** NewMatrix(int rows, int cols)
{
  double *matrix = new double[rows*cols];
  double **m = new double *[rows];
  for(int i = 0; i < rows; i++) {
    m[i] = &matrix[i*cols];
  }
  return m;
}

//------------------------------------------------------------------------
static inline void DeleteMatrix(double **m)
{
  delete [] *m;
  delete [] m;
}

//------------------------------------------------------------------------
template <class TMatrix1>
static inline void MatrixMultiply(TMatrix1 **a, double **b, double **c,
                                  int arows, int acols,
                                  int brows, int bcols)
{
  if(acols != brows)  {
    return; // acols must equal br otherwise we can't proceed
  }

  // c must have size arows*bcols (we assume this)

  for(int i = 0; i < arows; i++) {
    for(int j = 0; j < bcols; j++) {
      c[i][j] = 0.0;
      for(int k = 0; k < acols; k++) {
        c[i][j] += double(a[i][k])*b[k][j];
      }
    }
  }
}


//------------------------------------------------------------------------
// Subtracting the mean column from the observation matrix is equal
// to subtracting the mean shape from all shapes.
// The mean column is equal to the Procrustes mean (it is also returned)
template <class TMatrix1, class TMatrix2>
static inline void SubtractMeanColumn(TMatrix1 **m, TMatrix2 *mean, int rows, int cols)
{
  int r,c;
  double csum;
  for (r = 0; r < rows; r++) {
    // What does it mean also done at Simon
    csum = 0.0F;
    for (c = 0; c < cols; c++) {
      csum += double(m[r][c]);
    }
    // calculate average value of row
    csum /= cols;

    // Mean shape vector is updated
    mean[r] =  TMatrix2(csum);

    // average value is subtracted from all elements in the row
    for (c = 0; c < cols; c++) {
      m[r][c] -=  TMatrix1(csum);
    }
  }
}

//------------------------------------------------------------------------
// Normalise all columns to have length 1
// meaning that all eigenvectors are normalised
static inline void NormaliseColumns(double **m, int rows, int cols)
{
  for (int c = 0; c < cols; c++) {
    double cl = 0;
    for (int r = 0; r < rows; r++) {
      cl += m[r][c] * m[r][c];
    }
    cl = sqrt(cl);

    // If cl == 0 something is rotten, dont do anything now
    if (cl != 0) {
      for (int r = 0; r < rows; r++) {
        m[r][c] /= cl;
      }
    }
  }
}

//------------------------------------------------------------------------
// Here it is assumed that a rows >> a cols
// Output matrix is [a cols X a cols]
static inline void SmallCovarianceMatrix(double **a, double **c,
                                         int arows, int acols)
{
  const int s = acols;

  // c must have size acols*acols (we assume this)
  for(int i = 0; i < acols; i++) {
    for(int j = 0; j < acols; j++) {
      // Use symmetry
      if (i <= j) {
        c[i][j] = 0.0;
        for(int k = 0; k < arows; k++) {
          c[i][j] += a[k][i]*a[k][j];
        }
        c[i][j] /= (s-1);
        c[j][i] = c[i][j];
      }
    }
  }
}

//------------------------------------------------------------------------
static inline double* NewVector(int length)
{
  double *vec = new double[length];
  return vec;
}

//------------------------------------------------------------------------
static inline void DeleteVector(double *v)
{
  delete [] v;
}
//ETX
#endif











