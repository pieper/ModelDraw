
#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"

#include "vtkSlicerColorLogic.h"
#include "vtkPharmacokineticsLogic.h"
#include "vtkPharmacokineticsCurveAnalysisITKInterface.h"

//TODO Adapt python SCIPY solvers
#ifdef Pharmacokinetics_USE_SCIPY
#include "vtkPharmacokineticsCurveAnalysisPythonInterface.h"
#endif

#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLScalarVolumeDisplayNode.h"
#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkCommandLineModuleLogic.h"
#include "vtkCommandLineModuleGUI.h"

#include "itkMetaDataDictionary.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkNumericSeriesFileNames.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkImageFileWriter.h"
#include "itkTimeProbesCollectorBase.h"

#include "vtkMRMLLinearTransformNode.h"
#include "vtkMRMLBSplineTransformNode.h"
#include "itksys/DynamicLoader.hxx"

#include "vtkMRMLTimeSeriesBundleNode.h"

#include "vtkGlobFileNames.h"

#include "vtkSlicerVolumesGUI.h"
#include "vtkSlicerVolumesLogic.h"

#include "vtkMultiThreader.h"
#include "vtkMutexLock.h"

#include <cmath>
#include <math.h>
#if WIN32
#include <float.h>
#define finite(a) (_finite(a))
#endif


vtkCxxRevisionMacro(vtkPharmacokineticsLogic, "$Revision: 3633 $");
vtkStandardNewMacro(vtkPharmacokineticsLogic);

//---------------------------------------------------------------------------
vtkPharmacokineticsLogic::vtkPharmacokineticsLogic()
{

  // Timer Handling

  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkPharmacokineticsLogic::DataCallback);

  this->FrameNodeVector.clear();

  this->CurveCache.clear();
  this->CurveAnalysisNode = NULL;

  this->VolumeBundleID = "";
  this->RegisteredVolumeBundleID = "";


}


//---------------------------------------------------------------------------
vtkPharmacokineticsLogic::~vtkPharmacokineticsLogic()
{

  this->SetCurveAnalysisNode ( NULL );
  if (this->DataCallbackCommand)
    {
    this->DataCallbackCommand->Delete();
    }

}


//---------------------------------------------------------------------------
void vtkPharmacokineticsLogic::PrintSelf(ostream& os, vtkIndent indent)
{
  this->vtkObject::PrintSelf(os, indent);
  os << indent << "vtkPharmacokineticsLogic:             " << this->GetClassName() << "\n";
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsLogic::ProcessLogicEvents(vtkObject *caller, 
                                            unsigned long event, 
                                            void *callData)
{
  if (event ==  vtkCommand::ProgressEvent) 
    {
    this->InvokeEvent ( vtkCommand::ProgressEvent,callData );
    }
}

//---------------------------------------------------------------------------
void vtkPharmacokineticsLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
  vtkPharmacokineticsLogic *self = reinterpret_cast<vtkPharmacokineticsLogic *>(clientData);
  vtkDebugWithObjectMacro(self, "In vtkPharmacokineticsLogic DataCallback");
  self->UpdateAll();
}



//----------------------------------------------------------------------------
void vtkPharmacokineticsLogic::AddKnownPharmacokineticModels()
{
  this->AddNewPharmacokineticModel ( "Tofts Kinetic Model" );
  this->AddNewPharmacokineticModel ( "Tofts Kinetic Model (python)" );
  this->AddNewPharmacokineticModel ( "Simple Tofts Kinetic Model (python)" );
  this->AddNewPharmacokineticModel ( "Tofts Kinetic Model Step Input (python)");
  this->AddNewPharmacokineticModel ( "Kety Model (python)");
  this->AddNewPharmacokineticModel ( "Tofts Kinetic Model Biexponential Input (python)" );
  //--- set up default selection in node.
  if ( this->CurveAnalysisNode )
    {
    this->CurveAnalysisNode->SetMethodName ( "Tofts Kinetic Model" );
    }
}

//----------------------------------------------------------------------------
int vtkPharmacokineticsLogic::GetNumberOfPharmacokineticModels ()
{
  return ( this->ModelNamesArray.size() );
}

//----------------------------------------------------------------------------
const char *vtkPharmacokineticsLogic::GetNthPharmacokineticModelName ( int n )
{
  if ( n <= this->ModelNamesArray.size() )
    {
    return (this->ModelNamesArray[n].c_str() );
    }
  else
    {
    return ( NULL );
    }
}

//----------------------------------------------------------------------------
void vtkPharmacokineticsLogic::AddNewPharmacokineticModel( const char *modelName)
{
  this->ModelNamesArray.push_back ( modelName );
}


//----------------------------------------------------------------------------
void vtkPharmacokineticsLogic::SetPharmacokineticModel (const char *modelName )
{

  if ( this->CurveAnalysisNode == NULL )
    {
    vtkErrorMacro ("SetPharmacokineticModel: Got NULL Curve Analysis Node.");
    return;
    }

  //--- make sure model is included in the
  //--- module's suite.
  std::vector<std::string>::iterator citer;
  std::string modelToPick = modelName;
  bool found = false;

  for (citer = this->ModelNamesArray.begin();
       citer != this->ModelNamesArray.end();
       citer ++ )
    {
    if ( *citer == modelToPick )
      {
      this->CurveAnalysisNode->SetMethodName (modelName);
      //--- found and set.
      found = true;
      break;
      }
    }
  if ( !found )
    {
    //--- not found and create appropriate error message.
    this->CurveAnalysisNode->SetErrorMessage ( "Requested model is not available. Please choose a different model." );
    }
}



//----------------------------------------------------------------------------
const char * vtkPharmacokineticsLogic::GetCurrentPharmacokineticModel ()
{
  if ( this->CurveAnalysisNode != NULL )
    {
    return ( this->CurveAnalysisNode->GetMethodName() );
    }
  else
    {
    return NULL;
    }

}



//---------------------------------------------------------------------------
void vtkPharmacokineticsLogic::UpdateAll()
{
}


//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode* vtkPharmacokineticsLogic::AddMapVolumeNode(vtkMRMLTimeSeriesBundleNode* bundleNode,
                                                                 const char* nodeName)
{
  vtkMRMLScene* scene = this->GetMRMLScene();

  vtkMRMLScalarVolumeNode *volumeNode = vtkMRMLScalarVolumeNode::New();
  vtkMRMLScalarVolumeDisplayNode* displayNode = vtkMRMLScalarVolumeDisplayNode::New();

  volumeNode->SetScene(scene);

  vtkImageData* imageData = vtkImageData::New();
  vtkMRMLScalarVolumeNode *firstFrameNode 
    = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(0));

  if (firstFrameNode && firstFrameNode->GetImageData())
    {
    vtkImageData* firstImageData = firstFrameNode->GetImageData();

    imageData->SetDimensions(firstImageData->GetDimensions());
    imageData->SetExtent(firstImageData->GetExtent());
    imageData->SetSpacing(firstImageData->GetSpacing());
    imageData->SetOrigin(firstImageData->GetOrigin());
    imageData->SetNumberOfScalarComponents(1);
    imageData->SetScalarTypeToFloat();  // Set to float to store parameters
    imageData->AllocateScalars();
    }

  volumeNode->SetAndObserveImageData(imageData);
  volumeNode->SetName(nodeName);
  vtkMatrix4x4* mat = vtkMatrix4x4::New();
  firstFrameNode->GetIJKToRASMatrix(mat);
  volumeNode->SetIJKToRASMatrix(mat);
  mat->Delete();

  scene->SaveStateForUndo();
  volumeNode->SetScene(scene);
  volumeNode->SetDescription("Created by 4D Analysis Module");
  
  double range[2];
  vtkDebugMacro("Set basic display info");
  //volumeNode->GetImageData()->GetScalarRange(range);
  range[0] = 0.0;
  range[1] = 256.0;
  displayNode->SetAutoWindowLevel(0);
  displayNode->SetAutoThreshold(0);
  displayNode->SetLowerThreshold(range[0]);
  displayNode->SetUpperThreshold(range[1]);
  displayNode->SetWindow(range[1] - range[0]);
  displayNode->SetLevel(0.5 * (range[1] + range[0]) );
  
  vtkSlicerColorLogic *colorLogic = vtkSlicerColorLogic::New();
  displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
  displayNode->SetScene(scene);
  scene->AddNode(displayNode);  
  scene->AddNode(volumeNode);  

  volumeNode->SetAndObserveDisplayNodeID(displayNode->GetID());

  //volumeNode->Delete();
  //storageNode->Delete();
  //displayNode->Delete();
  //colorLogic->Delete();

  volumeNode->Modified();
  volumeNode->GetImageData()->Modified();

  return volumeNode;
}


//---------------------------------------------------------------------------
int vtkPharmacokineticsLogic::GetNumberOfFrames()
{
  return this->FrameNodeVector.size();
}


//---------------------------------------------------------------------------
const char* vtkPharmacokineticsLogic::GetFrameNodeID(int index)
{
  if (index >= 0 && index < (int)this->FrameNodeVector.size())
    {
    return this->FrameNodeVector[index].c_str();
    }
  else
    {
    return NULL;
    }
}


//---------------------------------------------------------------------------
vtkMRMLDoubleArrayNode* vtkPharmacokineticsLogic::LoadDoubleArrayNodeFromFile(const char* path)
{
  vtkDoubleArray* array = vtkDoubleArray::New();
  array->SetNumberOfComponents(3);

  std::ifstream fin(path);
  std::string sr;
  
  double row[3];
  row[0] = row[1] = row[2] = 0.0;

  while (std::getline(fin, sr))
    {
    std::stringstream ssr(sr);
    std::string sc;
    int i = 0;
    while (std::getline(ssr, sc, ','))
      {
      std::stringstream ssc(sc);
      double d;
      ssc >> d;
      if (i < 3)
        {
        row[i] = d;
        std::cerr << d << std::endl;
        }
      i ++;
      }
    array->InsertNextTuple(row);
    }

  if (array->GetNumberOfTuples() > 0)
    {
    vtkMRMLDoubleArrayNode* anode = vtkMRMLDoubleArrayNode::New();
    anode->SetArray(array);
    array->Delete();
    return anode;
    }
  else
    {
    array->Delete();
    return NULL;
    }

}


//---------------------------------------------------------------------------
int vtkPharmacokineticsLogic::SaveIntensityCurves(vtkPharmacokineticsIntensityCurves* curves, const char* fileNamePrefix)
{
  if (!curves)
    {
    return 0;
    }

  vtkIntArray* labels = curves->GetLabelList();
  int nLabels = labels->GetNumberOfTuples();
  for (int i = 0; i < nLabels; i ++)
    {
    int label = labels->GetValue(i);
    char filename[256];
    sprintf (filename, "%s_%03d.csv", fileNamePrefix, label);
    
    std::ofstream fout;
    fout.open(filename, std::ios::out);
    if (fout.fail())
      {
      labels->Delete();
      vtkErrorMacro ("vtkPharmacokineticsLogic: could not open file " << filename );
      return 0;
      }
    curves->OutputDataInCSV(fout, label);
    fout.close();
    }

  labels->Delete();
  return 1;
}


//---------------------------------------------------------------------------
int vtkPharmacokineticsLogic::SaveCurve(vtkDoubleArray* curve, const char* fileNamePrefix)
{
  char filename[256];
  sprintf (filename, "%s.csv", fileNamePrefix);
  std::ofstream fout;
  fout.open(filename, std::ios::out);
  if (fout.fail())
    {
    vtkErrorMacro ("vtkPharmacokineticsLogic: could not open file " << filename );
    return 0;
    }
  
  int n = curve->GetNumberOfTuples();
  for (int i = 0; i < n; i ++)
    {
    double* xy = curve->GetTuple(i);
    // Write the data
    //      t        ,      mean
    //   ---------------------------
    fout << xy[0] << ", " << xy[1] << std::endl;
    }

  fout.close();
  
  return 1;
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsLogic::GenerateParameterMap(vtkPharmacokineticsCurveAnalysisInterface* interface,
                                                 vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode,
                                                 vtkMRMLTimeSeriesBundleNode* bundleNode,
                                                 const char* outputNodeNamePrefix,
                                                 int start, int end,
                                                 int imin, int imax, int jmin, int jmax, int kmin, int kmax)
{

  if (!interface || !bundleNode || !curveNode)
    {
    return;
    }

  vtkPharmacokineticsCurveAnalysisITKInterface *PKsolver = NULL;

  if (vtkPharmacokineticsCurveAnalysisITKInterface::SafeDownCast (interface) != NULL )
    {
    PKsolver = vtkPharmacokineticsCurveAnalysisITKInterface::SafeDownCast (interface);
    }
#ifdef Pharmacokinetics_USE_SCIPY  
  // TODO: extend with Python/SCIPY solvers
  else if (vtkPharmacokineticsCurveAnalysisPythonInterface::SafeDownCast (interface) != NULL )
    {
    PKsolver = vtkPharmacokineticsCurveAnalysisPythonInterface::SafeDownCast (interface);
    }
#endif
  
  int nFrames = bundleNode->GetNumberOfFrames();  
  vtkStringArray* nameArray = curveNode->GetOutputValueNameArray();

  StatusMessageType statusMessage;
  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Preparing maps....";
  this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);        

  // Create map volumes for each parameter
  int numKeys = nameArray->GetNumberOfTuples();
  typedef std::map<std::string, vtkImageData*>            ParameterImageMapType;
  typedef std::map<std::string, vtkMRMLScalarVolumeNode*> ParameterVolumeNodeMapType;

  ParameterImageMapType      ParameterImages;
  ParameterVolumeNodeMapType ParameterImageNodes;

  for (int i = 0; i < numKeys; i ++)
    {
    char  nodeName[256];
    const char* paramName = nameArray->GetValue(i);
    sprintf(nodeName, "%s_%s", outputNodeNamePrefix, paramName);
    std::cerr << "Creating " << nodeName << std::endl;
    vtkMRMLScalarVolumeNode* node  = AddMapVolumeNode(bundleNode, nodeName);
    ParameterImages[paramName]     = node->GetImageData();
    ParameterImageNodes[paramName] = node;
    }
  
  // Check the index range
  int max = nFrames;
  if (start < 0)   start = 0;
  if (end >= max)  end   = max-1;
  if (start > end) start = end;
  
  // Make an array of vtkImageData
  int nSrcPoints = end - start + 1;
  std::vector<vtkImageData*> imageVector;
  std::vector<double>        imageTimeStampVector;
  imageVector.resize(nSrcPoints);
  imageTimeStampVector.resize(nSrcPoints);

  for (int i = start; i <= end; i ++)
    {
    vtkMRMLScalarVolumeNode* node = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(i));
    imageVector[i-start] = node->GetImageData();

    vtkMRMLTimeSeriesBundleNode::TimeStamp ts;
    bundleNode->GetTimeStamp(i, &ts);
    imageTimeStampVector[i-start] = (double)ts.second + (double)ts.nanosecond / 1000000000.0;

    std::cerr << "Listing image data: " << node->GetName() << std::endl;
    }
  
  int* dim = imageVector[0]->GetDimensions();
  int x = dim[0]; int y = dim[1]; int z = dim[2];

  vtkDoubleArray* srcCurve = vtkDoubleArray::New();
  vtkDoubleArray* fittedCurve = vtkDoubleArray::New();
  srcCurve->SetNumberOfComponents(2);
  srcCurve->SetNumberOfTuples(nSrcPoints);

  fittedCurve->SetNumberOfComponents(2);
  fittedCurve->SetNumberOfTuples(0);        

  // Check index
  if (imin < 0 || imin >= imax || imax >= x ||
      jmin < 0 || jmin >= jmax || jmax >= y ||
      kmin < 0 || kmin >= kmax || kmax >= z)
    {
    imin = 0;
    jmin = 0;
    kmin = 0;
    imax = x;
    jmax = y;
    kmax = z;
    }

  // set curve analysis node to the PKsolver interface
  if ( PKsolver )
    {
    PKsolver->SetCurveAnalysisNode(curveNode);
    }
  else
    {
    vtkErrorMacro ( "GenerateParameterMap: Got NULL curve analysis interface" );
    return;
    }

  
  this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);
  double numVoxel = (double) (kmax-kmin)*(jmax-jmin)*(imax-imin);
  double counter  = 0;
  char   progressMsg[128];
  
  for (int k = kmin; k < kmax; k ++)
    {
    std::cerr << std::endl;
    for (int j = jmin; j < jmax; j ++)
      {
      for (int i = imin; i < imax; i ++)
        {
        // Update progress message.
        counter += 1.0;
        statusMessage.progress = counter / numVoxel;
        sprintf(progressMsg, "Fitting curve at (i=%d, j=%d, k=%d)", i, j, k);
        statusMessage.message = progressMsg;
        this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);        

        // Copy intensity data
        for (int t = 0; t < nSrcPoints; t ++)
          {
          double xy[2];
          xy[0] = imageTimeStampVector[t];//(double) t + start;
          xy[1] = imageVector[t]->GetScalarComponentAsDouble(i, j, k, 0);
          srcCurve->SetTuple(t, xy);
          fittedCurve->InsertNextTuple(xy);
          }

        curveNode->SetTargetCurve(srcCurve);
        curveNode->SetFittedCurve(fittedCurve);

        if ( PKsolver )
          {
          PKsolver->Run();
          }
        else
          {
          vtkErrorMacro ( "GenerateParameterMap: Got NULL curve analysis interface." );
          return;
          }

        // Put results
        ParameterImageMapType::iterator iter;
        for (iter = ParameterImages.begin(); iter != ParameterImages.end(); iter ++)
          {
          float param = (float)curveNode->GetOutputValue(iter->first.c_str());
          //hmmm... std::isnormal is there only if C99 is enabled with a
          //combination of C99 macro dynamic.
          // isnormal = neither { zero, subnormal, infinite, nor NaN }
          // isnormal(x) =  _finite(x) && x != 0  && !issubnormal(x)
          // don't know what subnormal is or how to test for it...
//          if (!std::isnormal(param))
          if ( !finite(param) )
            {
            param = 0.0;
            }
          iter->second->SetScalarComponentFromFloat(i, j, k, 0, param);
          }
        }
      }
    //std::cerr << std::endl;
    }
  
  statusMessage.show = 0;
  statusMessage.progress = 0.0;
  statusMessage.message = "";
  this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);

  // Put results
  ParameterVolumeNodeMapType::iterator iter;
  for (iter = ParameterImageNodes.begin(); iter != ParameterImageNodes.end(); iter ++)
    {
    double range[2];
    vtkImageData* imageData = iter->second->GetImageData();
    vtkMRMLScalarVolumeDisplayNode* displayNode 
      = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(iter->second->GetDisplayNode());
    imageData->Update();
    imageData->GetScalarRange(range);
    std::cerr << "range = (" << range[0] << ", " << range[1] << ")" << std::endl;
    displayNode->SetAutoWindowLevel(0);
    displayNode->SetAutoThreshold(0);
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]));
    }
  
  std::cerr << "END " << std::endl;
}

//---------------------------------------------------------------------------
void vtkPharmacokineticsLogic::GenerateParameterMapInMask(vtkPharmacokineticsCurveAnalysisInterface* interface,
                                                       vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode,
                                                       vtkMRMLTimeSeriesBundleNode* bundleNode, 
                                                       const char* outputNodeNamePrefix,
                                                       int start, int end,
                                                       vtkMRMLScalarVolumeNode* maskNode, int label)
{
  // check pointers
  if (!interface || !bundleNode || !curveNode)
    {
    return;
    }

  vtkPharmacokineticsCurveAnalysisInterface *PKsolver = NULL;
  if (vtkPharmacokineticsCurveAnalysisITKInterface::SafeDownCast (interface) != NULL )
    {
    PKsolver = vtkPharmacokineticsCurveAnalysisITKInterface::SafeDownCast (interface);
    }

#ifdef Pharmacokinetics_USE_SCIPY
  // TODO: extend with Python/SCIPY solvers
  else if (vtkPharmacokineticsCurveAnalysisPythonInterface::SafeDownCast (interface) != NULL )
    {
    PKsolver = vtkPharmacokineticsCurveAnalysisPythonInterface::SafeDownCast (interface);
    }
#endif
  
  int nFrames = bundleNode->GetNumberOfFrames();  
  vtkStringArray* nameArray = curveNode->GetOutputValueNameArray();

  StatusMessageType statusMessage;
  statusMessage.show = 1;
  statusMessage.progress = 0.0;
  statusMessage.message = "Preparing maps....";
  this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);        

  // Create map volumes for each parameter
  int numKeys = nameArray->GetNumberOfTuples();
  typedef std::map<std::string, vtkImageData*>            ParameterImageMapType;
  typedef std::map<std::string, vtkMRMLScalarVolumeNode*> ParameterVolumeNodeMapType;

  ParameterImageMapType      ParameterImages;
  ParameterVolumeNodeMapType ParameterImageNodes;

  for (int i = 0; i < numKeys; i ++)
    {
    char  nodeName[256];
    const char* paramName = nameArray->GetValue(i);
    sprintf(nodeName, "%s_%s", outputNodeNamePrefix, paramName);
    std::cerr << "Creating " << nodeName << std::endl;
    vtkMRMLScalarVolumeNode* node  = AddMapVolumeNode(bundleNode, nodeName);
    ParameterImages[paramName]     = node->GetImageData();
    ParameterImageNodes[paramName] = node;
    }
  
  // Make an array of vtkImageData
  int nSrcPoints = end - start + 1;
  std::vector<vtkImageData*> imageVector;
  std::vector<double>        imageTimeStampVector;
  imageVector.resize(nSrcPoints);
  imageTimeStampVector.resize(nSrcPoints);

  for (int i = start; i <= end; i ++)
    {
    vtkMRMLScalarVolumeNode* node = vtkMRMLScalarVolumeNode::SafeDownCast(bundleNode->GetFrameNode(i));
    imageVector[i-start] = node->GetImageData();

    vtkMRMLTimeSeriesBundleNode::TimeStamp ts;
    bundleNode->GetTimeStamp(i, &ts);
    imageTimeStampVector[i-start] = (double)ts.second + (double)ts.nanosecond / 1000000000.0;

    std::cerr << "Listing image data: " << node->GetName() << std::endl;
    }
  
  int* dim = imageVector[0]->GetDimensions();
  int x = dim[0]; int y = dim[1]; int z = dim[2];

  // Check if the dimension of the frames in time series data is same as the mask
  int maskDim[3];
  vtkImageData*  mask = maskNode->GetImageData();
  mask->GetDimensions(maskDim);
  int mx = maskDim[0];
  int my = maskDim[1];
  int mz = maskDim[2];

  if (x != mx || y != my || z!= mz)
    {
    return;
    }

  vtkDoubleArray* srcCurve = vtkDoubleArray::New();
  vtkDoubleArray* fittedCurve = vtkDoubleArray::New();
  srcCurve->SetNumberOfComponents(2);
  srcCurve->SetNumberOfTuples(nSrcPoints);

  fittedCurve->SetNumberOfComponents(2);
  fittedCurve->SetNumberOfTuples(0);        

  // set curve analysis node to the PKsolver interface
  if ( PKsolver )
    {
    PKsolver->SetCurveAnalysisNode(curveNode);
    }
  else
    {
    vtkErrorMacro ( "GenerateParameterMapInMask: Got unknown NULL analysis interface." );
    return;
    }

  this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);

  //----------------------------------------
  // Generate index list
  std::vector<CoordType> indexList;
  indexList.clear();
  
  for (int i = 0; i < x; i ++)
    {
    for (int j = 0; j < y; j ++)
      {
      for (int k = 0; k < z; k ++)
        {
        int l = (int) mask->GetScalarComponentAsDouble(i, j, k, 0);
        if (l == label)
          {
          CoordType index;
          index.x = i;
          index.y = j;
          index.z = k;
          indexList.push_back(index);
          }
        }
      }
    }

  double numVoxel = (double) indexList.size();
  double counter  = 0;
  char   progressMsg[128];

  std::vector<CoordType>::iterator citer;

  for (citer = indexList.begin(); citer != indexList.end(); citer ++)
    {
    // Update progress message.
    int i = (*citer).x;
    int j = (*citer).y;
    int k = (*citer).z;
    
    counter += 1.0;
    statusMessage.progress = counter / numVoxel;
    sprintf(progressMsg, "Fitting curve at (i=%d, j=%d, k=%d)", i, j, k);
    statusMessage.message = progressMsg;
    this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);        
    
    // Copy intensity data
    for (int t = 0; t < nSrcPoints; t ++)
      {
      double xy[2];
      xy[0] = imageTimeStampVector[t];//(double) t + start;
      xy[1] = imageVector[t]->GetScalarComponentAsDouble(i, j, k, 0);
      srcCurve->SetTuple(t, xy);
      fittedCurve->InsertNextTuple(xy);
      }
    
    curveNode->SetTargetCurve(srcCurve);
    curveNode->SetFittedCurve(fittedCurve);
    
    if ( PKsolver )
      {
      PKsolver->Run();
      }
    else
      {
      vtkErrorMacro ( "GenerateParameterMapInMask: Got NULL curve analysis interface." );
      return;
      }

    // Put results
    ParameterImageMapType::iterator iter;
    for (iter = ParameterImages.begin(); iter != ParameterImages.end(); iter ++)
      {
      float param = (float)curveNode->GetOutputValue(iter->first.c_str());
      //hmmm... std::isnormal is there only if C99 is enabled with a
      //combination of C99 macro dynamic.
      // isnormal = neither { zero, subnormal, infinite, nor NaN }
      // isnormal(x) =  _finite(x) && x != 0  && !issubnormal(x)
      // don't know what subnormal is or how to test for it...
      //          if (!std::isnormal(param))
      if ( !finite(param) )
        {
        param = 0.0;
        }
      iter->second->SetScalarComponentFromFloat(i, j, k, 0, param);
      }
    }

  statusMessage.show = 0;
  statusMessage.progress = 0.0;
  statusMessage.message = "";
  this->InvokeEvent ( vtkPharmacokineticsLogic::ProgressDialogEvent, &statusMessage);

  // Put results
  ParameterVolumeNodeMapType::iterator iter;
  for (iter = ParameterImageNodes.begin(); iter != ParameterImageNodes.end(); iter ++)
    {
    double range[2];
    vtkImageData* imageData = iter->second->GetImageData();
    vtkMRMLScalarVolumeDisplayNode* displayNode 
      = vtkMRMLScalarVolumeDisplayNode::SafeDownCast(iter->second->GetDisplayNode());
    imageData->Update();
    imageData->GetScalarRange(range);
    std::cerr << "range = (" << range[0] << ", " << range[1] << ")" << std::endl;
    displayNode->SetAutoWindowLevel(0);
    displayNode->SetAutoThreshold(0);
    displayNode->SetLowerThreshold(range[0]);
    displayNode->SetUpperThreshold(range[1]);
    displayNode->SetWindow(range[1] - range[0]);
    displayNode->SetLevel(0.5 * (range[1] + range[0]));
    }
  
  std::cerr << "END " << std::endl;

}
  
