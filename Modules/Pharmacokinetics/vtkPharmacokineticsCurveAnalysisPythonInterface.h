#ifndef __vtkPharmacokineticsCurveAnalysisPythonInterface_h
#define __vtkPharmacokineticsCurveAnalysisPythonInterface_h


#include "vtkObject.h"
#include "vtkPharmacokineticsWin32Header.h"

#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"

#include "vtkSlicerConfigure.h" /* Slicer3_USE_* */


#ifdef Pharmacokinetics_USE_SCIPY
#include <Python.h>
#endif //Pharmacokinetics_USE_SCIPY

class VTK_Pharmacokinetics_EXPORT vtkPharmacokineticsCurveAnalysisPythonInterface : public vtkObject
{
 public:

  static vtkPharmacokineticsCurveAnalysisPythonInterface *New();
  vtkTypeRevisionMacro(vtkPharmacokineticsCurveAnalysisPythonInterface, vtkObject);

  void PrintSelf(ostream& os, vtkIndent indent);

  int SetScript(const char* filename);
  const char* GetScript() { return this->ScriptName.c_str(); };

  int SetCurveAnalysisNode(vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode);
  //BTX
  int SetCurveAnalysisNode(std::vector<vtkMRMLPharmacokineticsCurveAnalysisNode*> curveNodeVector);
  //ETX
  vtkMRMLPharmacokineticsCurveAnalysisNode* GetCurveAnalysisNode() { return this->CurveAnalysisNode; };

  int GetInfo();
  int Run();

 protected:
  vtkPharmacokineticsCurveAnalysisPythonInterface();
  virtual ~vtkPharmacokineticsCurveAnalysisPythonInterface();

 private:

  int GenerateFittingScript();

  //BTX
  std::string ScriptName;
  std::string PythonCmd;
  //ETX

  vtkMRMLPharmacokineticsCurveAnalysisNode* CurveAnalysisNode;
  //BTX
  std::vector<vtkMRMLPharmacokineticsCurveAnalysisNode*> CurveAnalysisNodeVector;
  //ETX

#ifdef Pharmacokinetics_USE_SCIPY
  PyObject *CompiledObject;
#endif // Slicer3_USE_PYTHON

};


#endif //__vtkPharmacokineticsCurveAnalysisPythonInterface_h
