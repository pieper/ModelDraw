#ifndef __vtkPharmacokineticsCurveAnalysisInterface_h
#define __vtkPharmacokineticsCurveAnalysisInterface_h


#include "vtkObject.h"
#include "vtkPharmacokineticsWin32Header.h"
#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"
#include "vtkSlicerConfigure.h" 
#include <vector>

class VTK_Pharmacokinetics_EXPORT vtkPharmacokineticsCurveAnalysisInterface : public vtkObject
{
 public:

  vtkGetObjectMacro ( CurveAnalysisNode, vtkMRMLPharmacokineticsCurveAnalysisNode );

  static vtkPharmacokineticsCurveAnalysisInterface *New();
  vtkTypeRevisionMacro(vtkPharmacokineticsCurveAnalysisInterface, vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  int SetCurveAnalysisNode(vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode);
  //BTX
  int SetCurveAnalysisNode(std::vector<vtkMRMLPharmacokineticsCurveAnalysisNode*> curveNodeVector);
  //ETX
  int ClearNodeInformation();

  /// Description:
  /// Method should be defined in derived classes.
  /// returns 0 on error.
  /// returns 1 for success.
  virtual int Run() {return 0; };

 protected:
  vtkPharmacokineticsCurveAnalysisInterface();
  virtual ~vtkPharmacokineticsCurveAnalysisInterface();
  //BTX
  std::vector<vtkMRMLPharmacokineticsCurveAnalysisNode*> CurveAnalysisNodeVector;
  //ETX
  vtkMRMLPharmacokineticsCurveAnalysisNode* CurveAnalysisNode;
  
 private:

};


#endif //__vtkPharmacokineticsCurveAnalysisInterface_h
