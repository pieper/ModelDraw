#ifndef __vtkPharmacokineticsCurveAnalysisITKInterface_h
#define __vtkPharmacokineticsCurveAnalysisITKInterface_h


#include "vtkObject.h"
#include "vtkPharmacokineticsWin32Header.h"
#include "vtkPharmacokineticsCurveAnalysisInterface.h"
#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"
#include "vtkSlicerConfigure.h" 


class VTK_Pharmacokinetics_EXPORT vtkPharmacokineticsCurveAnalysisITKInterface : public vtkPharmacokineticsCurveAnalysisInterface
{
 public:
  static vtkPharmacokineticsCurveAnalysisITKInterface *New();
  vtkTypeRevisionMacro(vtkPharmacokineticsCurveAnalysisITKInterface, vtkPharmacokineticsCurveAnalysisInterface);

  void PrintSelf(ostream& os, vtkIndent indent);
  int Run();

 protected:
  vtkPharmacokineticsCurveAnalysisITKInterface();
  virtual ~vtkPharmacokineticsCurveAnalysisITKInterface();

 private:
};

#endif //__vtkPharmacokineticsCurveAnalysisITKInterface_h
