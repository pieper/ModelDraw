#include "vtkObjectFactory.h"
#include "vtkPharmacokineticsCurveAnalysisInterface.h"

#include "vtkDoubleArray.h"
#include "vtkMRMLTimeSeriesBundleNode.h"
#include "vtkMRMLScalarVolumeNode.h"

#include "vtkSlicerApplication.h"
#include "vtkSlicerConfigure.h" 

vtkStandardNewMacro(vtkPharmacokineticsCurveAnalysisInterface);
vtkCxxRevisionMacro(vtkPharmacokineticsCurveAnalysisInterface, "$Revision: $");

//---------------------------------------------------------------------------
vtkPharmacokineticsCurveAnalysisInterface::vtkPharmacokineticsCurveAnalysisInterface()
{
  this->CurveAnalysisNode = NULL;
}


//---------------------------------------------------------------------------
vtkPharmacokineticsCurveAnalysisInterface::~vtkPharmacokineticsCurveAnalysisInterface()
{
  if ( this->CurveAnalysisNode )
    {
    this->SetCurveAnalysisNode ( NULL );
    }
}


//---------------------------------------------------------------------------
void vtkPharmacokineticsCurveAnalysisInterface::PrintSelf(ostream& os, vtkIndent indent)
{
}


//---------------------------------------------------------------------------
int vtkPharmacokineticsCurveAnalysisInterface::SetCurveAnalysisNode(vtkMRMLPharmacokineticsCurveAnalysisNode* curveNode)
{
  
  if (curveNode && curveNode->GetID())
    {
    this->CurveAnalysisNode = curveNode;
    return 1;
    }
  else
    {
    this->CurveAnalysisNode = NULL;
    return 0;
    }

}


//---------------------------------------------------------------------------
int vtkPharmacokineticsCurveAnalysisInterface::SetCurveAnalysisNode(std::vector<vtkMRMLPharmacokineticsCurveAnalysisNode*> curveNodeVector)
{
  if (curveNodeVector.size() > 0)
    {
    this->CurveAnalysisNodeVector.clear();
    std::vector<vtkMRMLPharmacokineticsCurveAnalysisNode*>::iterator iter;
    for (iter = curveNodeVector.begin(); iter != curveNodeVector.end(); iter ++)
      {
      if ((*iter) != NULL && (*iter)->GetID())
        {
        this->CurveAnalysisNodeVector.push_back((*iter));
        }
      }
//    GenerateFittingScript();
    return 1;
    }
  else
    {
    this->CurveAnalysisNodeVector.clear();
    return 0;
    }
}


//---------------------------------------------------------------------------
int vtkPharmacokineticsCurveAnalysisInterface::ClearNodeInformation()
{
  // Note: This fucntion should be called after the node is registered to the MRML scene.

  if (this->CurveAnalysisNode == NULL)
    {
    return 0;
    }

  // clear curve analysis node
  this->CurveAnalysisNode->ClearInputArrays();
  this->CurveAnalysisNode->ClearInitialParameters();
  this->CurveAnalysisNode->ClearConstants();
  this->CurveAnalysisNode->ClearOutputValues();

  return 1;
}





