
#ifndef __vtkPharmacokineticsLogic_h
#define __vtkPharmacokineticsLogic_h

#include "vtkPharmacokineticsWin32Header.h"

#include "vtkSlicerBaseLogic.h"
#include "vtkSlicerModuleLogic.h"
#include "vtkSlicerApplication.h"
#include "vtkCallbackCommand.h"
#include "vtkDoubleArray.h"

#include "vtkMRMLSliceNode.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLTimeSeriesBundleNode.h"
#include "vtkPharmacokineticsCurveAnalysisInterface.h"
#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"

#include "itkOrientedImage.h"
#include "itkImageSeriesReader.h"

#include "vtkPharmacokineticsIntensityCurves.h"
#include <string>
#include <map>


class vtkIGTLConnector;
class vtkMutexLock;

class VTK_Pharmacokinetics_EXPORT vtkPharmacokineticsLogic : public vtkSlicerModuleLogic 
{
 public:
//BTX
  enum {
    // Events
    StatusUpdateEvent       = 50003,
    ProgressDialogEvent     = 50004,
  };

  enum {
    TYPE_MEAN,
    TYPE_SD,
  };

  typedef struct {
    int show;
    double progress;
    std::string message;
  } StatusMessageType;

  typedef std::vector<std::string> FrameNodeVectorType;
  typedef struct {
    int x;
    int y;
    int z;
  } CoordType;

  typedef struct {
    unsigned long MaskModifiedTime;
    vtkDoubleArray*  Mean;
    vtkDoubleArray*  SD;
  } CurveDataType;
  
  typedef std::map<int, CurveDataType> CurveDataListType;
  typedef struct {
    std::string       MaskNodeID;
    CurveDataListType CurveList;
  } CurveDataSetType;


  typedef std::map<std::string, CurveDataSetType> CurveCacheType;
  typedef std::map<std::string, vtkImageData*>            ParameterImageMapType;
  typedef std::map<std::string, vtkMRMLScalarVolumeNode*> ParameterVolumeNodeMapType;


  typedef struct {
    int id;
    vtkPharmacokineticsLogic* ptr;
    vtkPharmacokineticsCurveAnalysisInterface* curveAnalysisInterface;
    std::vector<vtkImageData*> imageVector;
    std::vector<double> imageTimeStampVector;
    ParameterImageMapType parameterImageMap;
    vtkMutexLock* mutex;
    int rangei[2];
    int rangej[2];
    int rangek[2];
  } ThreadInfo;
//ETX

 public:
  
  static vtkPharmacokineticsLogic *New();
  
  vtkTypeRevisionMacro(vtkPharmacokineticsLogic,vtkObject);
  void PrintSelf(ostream&, vtkIndent);

  // Description:
  // Loads series of volumes from the directory that contains the file
  // specified by 'path' argument.
  // Returns number of volumes in the series.

//BTX
  //const int SpaceDim = 3;
  typedef short PixelValueType;
  typedef itk::OrientedImage< PixelValueType, 3 > VolumeType;
  typedef itk::ImageSeriesReader< VolumeType > ReaderType;
//ETX

  vtkGetObjectMacro ( CurveAnalysisNode, vtkMRMLPharmacokineticsCurveAnalysisNode);
  vtkSetObjectMacro ( CurveAnalysisNode, vtkMRMLPharmacokineticsCurveAnalysisNode );  

  vtkMRMLScalarVolumeNode* AddMapVolumeNode(vtkMRMLTimeSeriesBundleNode* bundleNode,
                                            const char* nodeName);
  int         GetNumberOfFrames();
  const char* GetFrameNodeID(int index);

  vtkMRMLDoubleArrayNode*  LoadDoubleArrayNodeFromFile(const char* path);
  int  SaveIntensityCurves(vtkPharmacokineticsIntensityCurves* curves, const char* fileNamePrefix);
  int  SaveCurve(vtkDoubleArray* curve, const char* fileNamePrefix);

  void SetApplication(vtkSlicerApplication *app) { this->Application = app; };
  vtkSlicerApplication* GetApplication() { return this->Application; };

  // methods are generalized to take python or c++ interfaces
  void GenerateParameterMap(vtkPharmacokineticsCurveAnalysisInterface* interface,
                            vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode,
                            vtkMRMLTimeSeriesBundleNode* bundleNode, 
                            const char* outputNodeNamePrefix,
                            int start, int end,
                            int imin, int imax, int jmin, int jmax, int kmin, int kmax);
  
  void GenerateParameterMapInMask(vtkPharmacokineticsCurveAnalysisInterface* interface,
                                  vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode,
                                  vtkMRMLTimeSeriesBundleNode* bundleNode, 
                                  const char* outputNodeNamePrefix,
                                  int start, int end,
                                  vtkMRMLScalarVolumeNode* mask, int label);

  // Description:
  // Use this method to extend the suite of models by name.
  void AddKnownPharmacokineticModels( );
  void AddNewPharmacokineticModel( const char *modelName);
  int GetNumberOfPharmacokineticModels ();
  const char *GetNthPharmacokineticModelName ( int n );

  
 protected:
  
  vtkPharmacokineticsLogic();
  ~vtkPharmacokineticsLogic();

  void operator=(const vtkPharmacokineticsLogic&);
  vtkPharmacokineticsLogic(const vtkPharmacokineticsLogic&);

  virtual void ProcessLogicEvents ( vtkObject * /*caller*/, 
                                  unsigned long /*event*/, 
                                  void * /*callData*/ );  

  static void DataCallback(vtkObject*, unsigned long, void *, void *);
  void UpdateAll();

  vtkSlicerApplication *Application;
  vtkCallbackCommand *DataCallbackCommand;

  
  // Description:
  // Get/Set the current pk model from node.
  void SetPharmacokineticModel (const char *modelName );
  const char *GetCurrentPharmacokineticModel ();
  
//BTX
  std::vector<std::string> ModelNamesArray;
//ETX
 private:

  vtkMRMLScene* MRMLScene;
  vtkMRMLPharmacokineticsCurveAnalysisNode *CurveAnalysisNode;

//BTX
  FrameNodeVectorType FrameNodeVector;
  FrameNodeVectorType RegisteredFrameNodeVector;
  std::string VolumeBundleID;
  std::string RegisteredVolumeBundleID;
  CurveCacheType CurveCache;  // CurveCache[<4d bundle name>][<label number>].<member of CurveDataType>
//ETX

};

#endif


  
