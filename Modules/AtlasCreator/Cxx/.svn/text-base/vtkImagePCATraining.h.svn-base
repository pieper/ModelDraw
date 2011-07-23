#ifndef __vtkImagePCATraining_h
#define __vtkImagePCATraining_h 

#include "vtkAtlasCreatorCxxModuleWin32Header.h"

#include "vtkImagePCAFilter.h"
#include <vtkstd/vector> 

class VTK_ATLASCREATORCXXMODULE_EXPORT vtkImagePCATraining : public vtkImagePCAFilter
  {
public:
  // -----------------------------------------------------
  // Genral Functions for the filter
  // -----------------------------------------------------
  static vtkImagePCATraining *New();
  vtkTypeMacro(vtkImagePCATraining,vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  void SetInputIndex(int index, vtkImageData *image)
    {this->SetInput(index,image);}

  // Set The maximum Number of Eigenvectors 
  void SetNumberOfEigenVectors(int init);
  int GetNumberOfEigenVectors()
    {
    return (int) this->EigenVectors.size();
    }

  vtkImageData* GetMean()
    {return vtkImageSource::GetOutput(0);}
  vtkImageData* GetEigenVectorIndex(int index)
    {return this->EigenVectors[index];}

  // Description:
  // Return the bsize parameters b that best model the given shape
  // (in standard deviations). 
  // That is that the given shape will be approximated by:
  //
  // shape ~ mean + b[0] * sqrt(eigenvalue[0]) * eigenvector[0]
  //              + b[1] * sqrt(eigenvalue[1]) * eigenvector[1]
  //         ...
  //              + b[bsize-1] * sqrt(eigenvalue[bsize-1]) * eigenvector[bsize-1]
  //void GetShapeParameters(vtkImageData *Image, vtkFloatArray *b, int bsize);

  vtkGetMacro(InputScalarType,int);

protected:
  vtkImagePCATraining();
  vtkImagePCATraining(const vtkImagePCATraining&)
    {};
  ~vtkImagePCATraining();
  void DeleteVariables();

  void InitializeOutputs(vtkImageData** data);
  void InitializeOutput(vtkImageData* data, int ext[6]);
  vtkImageData** GetOutputs()
    {return (vtkImageData **) this->Outputs;}
  // Disable this function
  vtkImageData* GetOutput()
    {return NULL;}

  void operator=(const vtkImagePCATraining&)
    {};
  void ExecuteData(vtkDataObject *);
  // Just make sure that nobody uses it in this filter other than own functions
  void SetEigenValue(int index, float value)
    {this->vtkImagePCAFilter::SetEigenValue(index,value);}
  //BTX
  std::vector<vtkImageData*> EigenVectors;
  //ETX
  };
#endif

