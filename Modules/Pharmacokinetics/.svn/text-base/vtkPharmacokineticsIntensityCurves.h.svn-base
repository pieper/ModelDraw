#ifndef __vtkPharmacokineticsIntensityCurves_h
#define __vtkPharmacokineticsIntensityCurves_h


#include "vtkObject.h"
#include "vtkPharmacokineticsWin32Header.h"
#include "vtkMRMLTimeSeriesBundleNode.h"
#include "vtkMRMLDoubleArrayNode.h"

class VTK_Pharmacokinetics_EXPORT vtkPharmacokineticsIntensityCurves : public vtkObject
{
 public:
  //BTX
  enum {
    TYPE_MEAN,
    TYPE_MAX,
    TYPE_MIN
  };
  //ETX

 protected:
  //BTX
  typedef struct {
    int x;
    int y;
    int z;
  } CoordType;
  typedef std::vector<CoordType>         IndexTableType;
  typedef std::map<int, IndexTableType>  IndexTableMapType;  // IndexTableMapType[label]
  typedef std::map<int, vtkMRMLDoubleArrayNode*> IntensityCurveMapType;
  //ETX

 public:

  static vtkPharmacokineticsIntensityCurves *New();
  vtkTypeRevisionMacro(vtkPharmacokineticsIntensityCurves, vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro ( BundleNode, vtkMRMLTimeSeriesBundleNode );
  vtkSetObjectMacro ( BundleNode, vtkMRMLTimeSeriesBundleNode );
  vtkGetObjectMacro ( MaskNode,   vtkMRMLScalarVolumeNode );
  vtkSetObjectMacro ( MaskNode,   vtkMRMLScalarVolumeNode );
  vtkSetObjectMacro ( MRMLScene,  vtkMRMLScene );
  vtkGetObjectMacro ( MRMLScene,  vtkMRMLScene );

  vtkSetMacro ( ValueType,  int );
  vtkGetMacro ( ValueType,  int );

  int Update();
  vtkIntArray* GetLabelList();
  vtkMRMLDoubleArrayNode* GetCurve(int label);
  int OutputDataInCSV(ostream& os, int label);

 protected:
  vtkPharmacokineticsIntensityCurves();
  virtual ~vtkPharmacokineticsIntensityCurves();

  void GenerateIntensityCurve();
  void GenerateIndexMap(vtkImageData* mask, IndexTableMapType& indexTableMap);
  double GetMeanIntensity(vtkImageData* image, IndexTableType& indexTable);
  double GetSDIntensity(vtkImageData* image, double mean, IndexTableType& indexTable);
  void GetMeanMaxMinIntensity(vtkImageData* image, IndexTableType& indexTable,
                                double& mean, double& max, double& min);
 private:

  vtkMRMLTimeSeriesBundleNode* BundleNode;
  vtkMRMLScalarVolumeNode* MaskNode;
  vtkMRMLTimeSeriesBundleNode* PreviousBundleNode;
  vtkMRMLScalarVolumeNode* PreviousMaskNode;
  IntensityCurveMapType IntensityCurve;  // IntensityCurveMean[label]

  // Description:
  // CurveType specifies how to calculate the intensity value at given time point from
  // the  region. Can be one of TYPE_MEAN, TYPE_MAX and TYPE_MIN
  int ValueType;

  long PreviousUpdateTime;
  vtkMRMLScene* MRMLScene;

};


#endif //__vtkPharmacokineticsIntensityCurves_h
