#include "vtkObjectFactory.h"
#include "vtkPharmacokineticsCurveAnalysisITKInterface.h"

#include "vtkDoubleArray.h"
#include "vtkMRMLTimeSeriesBundleNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerConfigure.h" 


vtkStandardNewMacro(vtkPharmacokineticsCurveAnalysisITKInterface);
vtkCxxRevisionMacro(vtkPharmacokineticsCurveAnalysisITKInterface, "$Revision: $");

//---------------------------------------------------------------------------
vtkPharmacokineticsCurveAnalysisITKInterface::vtkPharmacokineticsCurveAnalysisITKInterface()
{
}


//---------------------------------------------------------------------------
vtkPharmacokineticsCurveAnalysisITKInterface::~vtkPharmacokineticsCurveAnalysisITKInterface()
{
  if ( this->CurveAnalysisNode != NULL )
    {
    this->ClearNodeInformation();
    this->SetCurveAnalysisNode ( NULL );
    }
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsCurveAnalysisITKInterface::PrintSelf(ostream& os, vtkIndent indent)
{
}


//---------------------------------------------------------------------------
int vtkPharmacokineticsCurveAnalysisITKInterface::Run()
{

  if (this->CurveAnalysisNode == NULL)
    {
    return 0;
    }

  ///
  /// runs fitting code. Uses PkSolver Library from GE Global Research
  ///
  /// returns 0 on error.
  /// returns 1 for success.

  return 1 ;
}



