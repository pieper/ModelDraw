
#include "vtkImagePCATraining.h"
#include "vtkObjectFactory.h"
#include "vtkMath.h"
#include "vtkFloatArray.h"
#include "vtkImageData.h"
#include "assert.h"

//------------------------------------------------------------------------
vtkImagePCATraining* vtkImagePCATraining::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkImagePCATraining");
  if(ret)
  {
    return (vtkImagePCATraining*)ret;
  }
  // If the factory was unable to create the object, then create it here.
  return new vtkImagePCATraining;

}


//----------------------------------------------------------------------------
vtkImagePCATraining::vtkImagePCATraining() { 
  this->EigenVectors.clear();
}


//------------------------------------------------------------------------------
vtkImagePCATraining::~vtkImagePCATraining(){
   if ( this->EigenVectors.size()) {
    for (int i = 0 ; i < (int) this->EigenVectors.size(); i++)
      {
         if (this->EigenVectors[i])
         {
            this->EigenVectors[i]->Delete();
          }
      }
    this->EigenVectors.clear();
  }
}

//----------------------------------------------------------------------------
void vtkImagePCATraining::PrintSelf(ostream& os,vtkIndent indent) { }

//----------------------------------------------------------------------------
void vtkImagePCATraining::SetNumberOfEigenVectors(int init) {
  int oldSize = (int) this->EigenVectors.size();
  if (oldSize > init) 
    {
       for (int i = oldSize -1 ; i >= init; i--)
      {
          if (this->EigenVectors[i])
           {
              this->EigenVectors[i]->Delete();
          }
      }
    }
    this->EigenVectors.resize(init);
    if (oldSize < init) 
    {
         for (int i = oldSize; i < init; i++)
         {
       this->EigenVectors[i] = vtkImageData::New();
         }
    }
    this->SetNumberOfEigenModes(int(this->EigenVectors.size()));
}


template <class TMatrix1, class TMatrix2>
static inline void MatrixMultiplySwitched(TMatrix1 **a, double **b, TMatrix2 **c,
                                  int arows, int acols, 
                                  int brows, int bcols) 
{
  if(acols != brows)  {
     return; // acols must equal br otherwise we can't proceed
  }
  
  // c must have size arows*bcols (we assume this)
  
  for(int i = 0; i < arows; i++) {
    for(int j = 0; j < bcols; j++) {
      c[j][i] = 0.0;
      for(int k = 0; k < acols; k++) {
        c[j][i] += TMatrix2(double(a[k][i])*b[k][j]);
      }
    }
  }
}


//------------------------------------------------------------------------
// Normalise all columns to have length 1
// meaning that all eigenvectors are normalised
template <class T>
static inline void NormaliseColumnsSwitched(T **m, int rows, int cols)
{
  for (int c = 0; c < cols; c++) {
    double cl = 0;
    for (int r = 0; r < rows; r++) {
      cl += double(m[c][r]) * double(m[c][r]);
    }
    cl = sqrt(cl);
    
    // If cl == 0 something is rotten, dont do anything now
    if (cl != 0) {
      for (int r = 0; r < rows; r++) {
        m[c][r] /= T(cl);
      }
    }
  }
}

//------------------------------------------------------------------------
// Here it is assumed that a rows >> a cols
// Output matrix is [a cols X a cols]
template <class T>
static inline void SmallCovarianceMatrixSwitched(T **a, double **c,
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
      /// Originally it was  c[i][j] += a[k][i]*a[k][j];
      // but bc I do not switch the dimension anymore to save memory 
          c[i][j] += double(a[i][k])*double(a[j][k]);
        }
        c[i][j] /= (s-1);
        c[j][i] = c[i][j];
      }
    }
  }
} 

template <class T>
inline void  SubtractMeanColumn_DistanceMapSwitched(T** inDataPtr, int inVtkScalarType, int rows, int cols,  float* outMeanPtr )
{
    cout << "Warning: Do not use input later anymore because I subtract the mean from it" << endl;
    // If it is not float than do no not use it bc otherwise the compuational inaccuracies are to large so that the
    // PCA Model does not properly work 
    assert(inVtkScalarType >= VTK_FLOAT);
 
    int r,c;
    double csum;

    for (r = 0; r < rows; r++) {
      csum = 0.0F;
      for (c = 0; c < cols; c++) csum += double(inDataPtr[c][r]);
      // calculate average value of row
      csum /= double(cols);

      // Mean shape vector is updated
      outMeanPtr[r] = T(csum);

    }

    // Warning Warning Warning: 
    // Do not change the mean by normalizing or doing other thinks becuase otherwise you mess up the PCA model 

    // average value is subtracted from all elements in the row
    for (r = 0; r < rows; r++) {
      for (c = 0; c < cols; c++) {
    inDataPtr[c][r] -= T(outMeanPtr[r]);
      }
    }
}


//----------------------------------------------------------------------------
// This function is copied from Hybrid/vtkPCAAnalysisFilter 
template <class Tin>
static void vtkImagePCATrainingExecute(vtkImagePCATraining *self, Tin **inPtr, float **outPtr)
{
  float *MeanImagePtr   = outPtr[0];
  float **EigenVectorPtr = new float*[self->GetNumberOfEigenVectors()];
  for (int i = 0; i < self->GetNumberOfEigenVectors(); i++)  EigenVectorPtr[i] =  outPtr[i+1];

  // Number of Input images
  const int N_SETS =  self->vtkProcessObject::GetNumberOfInputs();
  
  const int N_POINTS =  self->GetXDim() * self->GetYDim() * self->GetZDim() ;
  cout << "N_POINTS " << N_POINTS << " N_SETS " <<  N_SETS << endl;  
  // Number of shapes
  const int s = N_SETS;
  
  // Number of points in a shape
  const int n = N_POINTS;
  
  // Observation Matrix [number of points number of shapes]
  // do not change because InputVector is number of shapes x number of points 
  // Probabely for speed reasons
  double **D = NewMatrix(n, s);
    
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < s; j++) {
      D[i][j] = (double) *inPtr[j]++; 
    }
  }
  
  // The mean shape is also calculated 
  SubtractMeanColumn(D, MeanImagePtr, n, s);
  
  // Covariance matrix of dim [s x s]
  double **T = NewMatrix(s, s);
  SmallCovarianceMatrix(D, T, n, s);
  
  double *ev       = NewVector(s);
  double **evecMat = NewMatrix(s, s);
  
  vtkMath::JacobiN(T, s, ev, evecMat);
  
  // Compute eigenvecs of DD' instead of T which is D'D
  // evecMat2 of dim [n x s]
  double **evecMat2 = NewMatrix(n, s);
  MatrixMultiply(D, evecMat, evecMat2, n, s, s, s);
  
  // Normalise eigenvectors
  NormaliseColumns(evecMat2, n, s);

    
  // vtkFloatArray *Evals = self->GetEigenValues();
  // Evals->SetNumberOfValues(s);

  // Copy data to output structures
  for (int j = 0; j < s-1; j++) self->vtkImagePCAFilter::SetEigenValue(j, float(ev[j]));
 // Evals->SetValue(j, float(ev[j]));
 
  for (int j = 0; j < self->GetNumberOfEigenVectors(); j++) {
    // EigenVector
    for (int i = 0; i < n; i++) {
        *EigenVectorPtr[j] = (float) evecMat2[i][j];
       EigenVectorPtr[j]++;
    }
  }

  // Clean Up
  DeleteMatrix(evecMat);
  DeleteMatrix(evecMat2);
  DeleteVector(ev);
  DeleteMatrix(T);
  DeleteMatrix(D);
  delete[] EigenVectorPtr;
  // cout << "End vtkImagePCATrainingExecute "<< endl;
}

//----------------------------------------------------------------------------
// This function is copied from Hybrid/vtkPCAAnalysisFilter
// We just wrote it so it would not allocate all this nonsense of memory 
template <class Tin>
static void vtkImagePCATrainingExecuteMemoryEfficient(vtkImagePCATraining *self, Tin **inPtr, float **outPtr)
{
  // cout << "Execute Memory Effient Version NEV:" << self->GetNumberOfEigenVectors() << endl;
  float *MeanImagePtr   = outPtr[0];
  const int N_EV = self->GetNumberOfEigenVectors(); 
  float **EigenVectorPtr = new float*[N_EV];
  for (int i = 0; i < N_EV; i++)  EigenVectorPtr[i] =  outPtr[i+1];

  // Number of Input images
  const int N_SETS = self->vtkProcessObject::GetNumberOfInputs();
  
  const int N_POINTS =  self->GetXDim() * self->GetYDim() * self->GetZDim() ;
  
  // Number of shapes
  const int s = N_SETS;
  
  // Number of points in a shape
  const int n = N_POINTS;
  
  // The mean shape is also calculated and subtracted from the remaining images
  SubtractMeanColumn_DistanceMapSwitched(inPtr,self->GetInputScalarType(),n,s,MeanImagePtr);

  // Covariance matrix of dim [s x s]
  double **T = NewMatrix(s, s);
  SmallCovarianceMatrixSwitched(inPtr, T, n, s);
  
  double *ev       = NewVector(s);
  double **evecMat = NewMatrix(s, s);
  // can be float
  vtkMath::JacobiN(T, s, ev, evecMat);
  // Compute eigenvecs of DD' instead of T which is D'D
  // evecMat2 of dim [n x s]
  MatrixMultiplySwitched(inPtr, evecMat, EigenVectorPtr, n, s , s, N_EV);
  // Normalise eigenvectors
  NormaliseColumnsSwitched(EigenVectorPtr, n, N_EV);
  // Copy data to output structures
  for (int j = 0; j < N_EV; j++) self->vtkImagePCAFilter::SetEigenValue(j, float(ev[j]));
 // Evals->SetValue(j, float(ev[j]));
   // Clean Up
  DeleteMatrix(evecMat);
  DeleteVector(ev);
  DeleteMatrix(T);
  delete[] EigenVectorPtr;
  //cout << "End vtkImagePCATrainingExecute "<< endl;
}

//----------------------------------------------------------------------------
// public
// void GetShapeParameters(vtkImageData *image, vtkFloatArray *b, int bsize)
// {
//   // Local variant of b for fast access.
//   double *bloc = NewVector(bsize);
//  
//   // Check size 
//   const int n = this->GetOutput(0)->GetNumberOfPoints();
//   int i,j;  
// 
//   // Check input 
//   //if(image->GetNumberOfPoints() != n) {
//   //  vtkErrorMacro(<<"Input shape does not have the correct number of points");
//   //  return;
//   //}
//   
//   double *shapevec = NewVector(n);
//   
//   // Copy shape and subtract mean shape
//   for (i = 0; i < n; i++) {
//     float *p = shape->GetPoint(i);
//     shapevec[i] = p[0] - meanshape[i];
//   }
//   
//   for (i = 0; i < bsize; i++) {
//     bloc[i] = 0;
//     
//     // Project the shape onto eigenvector i
//     for (j = 0; j < n; j++) {
//       bloc[i] += shapevec[j] * evecMat2[j][i];
//     }
//   }
// 
//   // Return b in number of standard deviations
//   b->SetNumberOfValues(bsize);
//   for (i = 0; i < bsize; i++) {
//     if (this->Evals->GetValue(i))
//       b->SetValue(i, bloc[i]/sqrt(this->Evals->GetValue(i)));
//     else
//       b->SetValue(i, 0);
//   }
// 
//   DeleteVector(shapevec);
//   DeleteVector(bloc);
// }


//----------------------------------------------------------------------------
void vtkImagePCATraining::InitializeOutput(vtkImageData* data, int Ext[6])
{
  vtkIdType IncX, IncY, IncZ;
    data->SetWholeExtent(Ext);
    data->SetExtent(Ext); 
    data->SetNumberOfScalarComponents(1);
    // Can be easily changed if needed
    data->SetScalarType(VTK_FLOAT);
    data->AllocateScalars(); 

    data->GetContinuousIncrements(Ext,IncX,IncY,IncZ);
    // Can be easily changed 
    if (IncY ||IncZ ) {
      vtkErrorMacro(<<"Currently only works if increments of Y and Z are 0");
      return; 
    }
}

void vtkImagePCATraining::InitializeOutputs(vtkImageData** data)
{
  vtkDebugMacro(<<"Start");
  int Ext[6];
  Ext[0] = Ext[2] = Ext[4] = 0;
  Ext[1] = this->XDim-1;
  Ext[3] = this->YDim-1;
  Ext[5] = this->ZDim-1;
  vtkDebugMacro(<<  Ext[1]  << " "  <<  Ext[3]  << " " <<  Ext[5] );
  this->InitializeOutput(data[0],Ext); 
  for (int i=0; i < this->GetNumberOfEigenVectors(); i++) {
    this->InitializeOutput(this->EigenVectors[i],Ext); 
  }
  vtkDebugMacro(<<"....End");
}


//----------------------------------------------------------------------------
// This method is passed a input and output datas, and executes the filter
// algorithm to fill the output from the inputs.
// It just executes a switch statement to call the correct function for
// the datas data types.
void vtkImagePCATraining::ExecuteData(vtkDataObject *)
{
   vtkDebugMacro(<<"ExecuteData()");
   // Initial Check
   int NumInputs = this->vtkProcessObject::GetNumberOfInputs();
   if(!NumInputs) {
    vtkErrorMacro(<<"No input!");
    return;
  }

   if (NumInputs <= this->GetNumberOfEigenVectors()) {
     vtkWarningMacro(<<"NumberOfEigenVectors (" <<  this->GetNumberOfEigenVectors() <<") is larger or equal to number of inputs ("<<NumInputs <<") !");
  }
   if (  !this->GetNumberOfEigenVectors() )
    {
      vtkWarningMacro(<<"Automatically setting NumberOfEigenVectors !"); 
      this->SetNumberOfEigenVectors(NumInputs-1); 
    }
   vtkDebugMacro(<<"NumberOfEigenVectors: " <<this->GetNumberOfEigenVectors()  );
  // Redefine ImageRelatedClass Parameters   
  vtkImageData **inData  = (vtkImageData **) this->GetInputs();
  { 
    int Ext[6];
    inData[0]->GetWholeExtent(Ext);
    this->XDim= Ext[1] - Ext[0] + 1;
    this->YDim= Ext[3] - Ext[2] + 1;
    this->ZDim= Ext[5] - Ext[4] + 1;
    if (!(this->XDim * this->YDim * this->ZDim)) {
    vtkErrorMacro( << "Input has no points!" );
    return;
    }
    this->InputScalarType = inData[0]->GetScalarType();
  }

  if (this->CheckInput(inData)) return;
  vtkDebugMacro(<<"CheckInput passed " );

  // Set Ouput correctly
  vtkImageData **outData = this->GetOutputs();
  this->InitializeOutputs(outData);
 

  // Run Algorithm  
  float** outPtr = new float*[this->GetNumberOfEigenVectors() + 1];
  // First outPtr is the MeanImage !
  outPtr[0] = (float*) outData[0]->GetScalarPointerForExtent(outData[0]->GetExtent());

  for (int i = 0 ; i <  this->GetNumberOfEigenVectors(); i++)
    {
      outPtr[i+1] = (float*) this->GetEigenVectorIndex(i)->GetScalarPointerForExtent(this->GetEigenVectorIndex(i)->GetExtent());
    }
  vtkDebugMacro(<<"Output set" );

  // Run Algorithm  
  void** inPtr = new void*[NumInputs];
  for (int i = 0 ; i <  NumInputs; i++)
    inPtr[i] = inData[i]->GetScalarPointerForExtent(inData[i]->GetExtent());

  vtkDebugMacro(<<"Input set" );
  // To supress warning messages
  if (this->InputScalarType >= VTK_FLOAT) {
     switch (this->InputScalarType) {
      vtkTemplateMacro3(vtkImagePCATrainingExecuteMemoryEfficient,this, (VTK_TT**)inPtr, outPtr);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
  } else {
    switch (this->InputScalarType) {
      vtkTemplateMacro3(vtkImagePCATrainingExecute,this, (VTK_TT**)inPtr, outPtr);
    default:
      vtkErrorMacro(<< "Execute: Unknown ScalarType");
      return;
    }
  }
  // Clean up 
  delete[] outPtr;
  delete[] inPtr;
}


