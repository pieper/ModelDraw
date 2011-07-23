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
// Since 22-Apr-02 vtkImageEMPrivate3DSegmenter is called vtkImagePCAFilter - Kilian
// EMPrivate =  using EM Algorithm with Local Tissue Class Probability
#include "vtkImagePCAFilter.h"
#include "vtkObjectFactory.h"
#include "vtkImageData.h"
#include "vtkImageKilianDistanceTransform.h"
#include "vtkImageMathematics.h"

//------------------------------------------------------------------------
vtkImagePCAFilter* vtkImagePCAFilter::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImagePCAFilter");
  if (ret)
    {
    return (vtkImagePCAFilter*) ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkImagePCAFilter;
}

//----------------------------------------------------------------------------
vtkImagePCAFilter::vtkImagePCAFilter()
{
  // First output is automatically initialized so do not have to do anything 
  this->NumberOfEigenModes = 0;
  this->XDim = this->YDim = this->ZDim = 0;
  this->EigenValues = NULL;
  this->InputScalarType = 0;
}

//------------------------------------------------------------------------------
vtkImagePCAFilter::~vtkImagePCAFilter()
{
  this->DeleteEigenValues();
}

//----------------------------------------------------------------------------
void vtkImagePCAFilter::PrintSelf(ostream& os, vtkIndent indent)
{
  os << indent << "Number of EigenModes  : " << this->NumberOfEigenModes
      << endl;
  os << indent << "EigenValues           : ";
  if (this->EigenValues)
    {
    for (int i = 0; i < this->NumberOfEigenModes; i++)
      os << this->EigenValues[i] << " ";
    }
  else
    os << "(None)";
  os << endl;
  os << indent << "Dimensions            : (" << this->XDim << ","
      << this->YDim << "," << this->ZDim << ")" << endl;
  os << indent << "InputScalarType       : " << this->InputScalarType << endl;
}

//------------------------------------------------------------------------------
// Special Vector and Matrix functions
void vtkImagePCAFilter::DeleteEigenValues()
{
  // Delete output ! -> Check with other classes 
  if (this->EigenValues)
    {
    delete[] this->EigenValues;
    }
  this->EigenValues = NULL;
}

int vtkImagePCAFilter::CheckInput(vtkImageData **InData)
{
  int NumInput = this->vtkProcessObject::GetNumberOfInputs();
  if (!NumInput)
    {
    vtkErrorMacro(<< "No input set!");
    return 1;
    }
  for (int i = 0; i < NumInput; i++)
    {
    if (this->CheckInput(InData[i]))
      return 1;
    }
  return 0;
}

int vtkImagePCAFilter::CheckInput(vtkImageData *InData)
{

  vtkDebugMacro(<<"vtkImagePCAFilter::CheckInput: " << InData << " ..."  );

  int Ext[6];
  vtkIdType Inc[3];

  InData->GetWholeExtent(Ext);
  if ((this->XDim != Ext[1] - Ext[0] + 1)
      || (this->YDim != Ext[3] - Ext[2] + 1) || (this->ZDim != Ext[5] - Ext[4]
      + 1))
    {
    vtkErrorMacro(<< "Input does not have the correct dimensions - should: ("<< this->XDim <<"," << this->YDim <<"," << this->ZDim <<") has: ("<< Ext[1] -Ext[0] +1 <<"," <<Ext[3] -Ext[2] +1 <<","<< Ext[5] -Ext[4] +1 << ")" );
    return 1;
    }
  InData->GetContinuousIncrements(Ext, Inc[0], Inc[1], Inc[2]);
  // We can easily adjust the algorithm to take care of it
  if (Inc[1] != 0 || Inc[2] != 0)
    {
    vtkErrorMacro(<< "Input does have increments unequal to zero!");
    return 1;
    }
  if (InData->GetScalarType() != this->InputScalarType)
    {
    vtkErrorMacro(<< "Inputs have different Scalar Types!");
    return 1;
    }
  // Could be easily adjusted if needed to -> Compare this filter to vtkPCAAnalysis
  if (InData->GetNumberOfScalarComponents() != 1)
    {
    vtkErrorMacro(<< "Number Of Scalar Components for all Input has to be 1");
    return 1;
    }
  vtkDebugMacro(<<".... OK " );
  return 0;
}

void vtkImagePCAFilter::TransfereLabelmapIntoPCADistanceMap(vtkImageData *inData, int Label, float MaxDistanceValue, float BoundaryValue, vtkImageData *outData)
{
  int inExtent[6], outExtent[6];
  inData->GetWholeExtent(inExtent);
  outData->GetWholeExtent(outExtent);
  int i = 0;
  while ((i < 6) && (inExtent[i] == outExtent[i]))
    i++;
  if (i < 6)
    {
    cout
        << "Error::vtkImagePCAFilter::TransfereLabelmapIntoPCADistanceMap: input and output image data do not have the same extent"
        << endl;
    cout << "InExtent:  " << inExtent[0] << " " << inExtent[1] << " "
        << inExtent[2] << " " << inExtent[3] << " " << inExtent[4] << " "
        << inExtent[5] << endl;
    cout << "OutExtent: " << outExtent[0] << " " << outExtent[1] << " "
        << outExtent[2] << " " << outExtent[3] << " " << outExtent[4] << " "
        << outExtent[5] << endl;
    return;
    }

  if (outData->GetScalarType() != VTK_FLOAT)
    {
    cout
        << "Error::vtkImagePCAFilter::TransfereLabelmapIntoPCADistanceMap: output data has to be of type VTK_FLOAT("
        << VTK_FLOAT << ") but is from type " << outData->GetScalarType()
        << "!" << endl;
    return;
    }

  vtkImageKilianDistanceTransform *dist =
      vtkImageKilianDistanceTransform::New();
  dist->SetInput(inData);
  dist->SetAlgorithmToSaito();
  dist->SignedDistanceMapOn();
  dist->SetObjectValue(Label);
  dist->SetZeroBoundaryInside();
  dist->SquareRootDistance();
  dist->SetMaximumDistance(MaxDistanceValue);
  dist->ConsiderAnisotropyOff();
  dist->Update();

  vtkImageMathematics *Boundary = vtkImageMathematics::New();
  Boundary->SetOperationToAddConstant();
  Boundary->SetInput(0, dist->GetOutput());
  Boundary->SetConstantC(BoundaryValue);
  Boundary->Update();

  outData->DeepCopy(Boundary->GetOutput());
  Boundary->Delete();
  dist->Delete();
}

