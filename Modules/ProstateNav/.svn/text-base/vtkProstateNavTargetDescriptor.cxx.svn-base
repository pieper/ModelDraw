#include "vtkProstateNavTargetDescriptor.h"
#include "vtkObjectFactory.h"
//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkProstateNavTargetDescriptor);
vtkCxxRevisionMacro(vtkProstateNavTargetDescriptor, "$Revision: 1.0 $");

//----------------------------------------------------------------------------
vtkProstateNavTargetDescriptor::vtkProstateNavTargetDescriptor()
{
  this->RASLocation[0] = 0;
  this->RASLocation[1] = 0;
  this->RASLocation[2] = 0;
  this->RASOrientation[0] = 0;
  this->RASOrientation[1] = 0;
  this->RASOrientation[2] = 0;
  this->RASOrientation[3] = 0;
  this->TargetCompleted = false;
  this->TargetValidated = false;
  this->NeedleTipValidationPosition[0]=0;
  this->NeedleTipValidationPosition[1]=0;
  this->NeedleTipValidationPosition[2]=0;
  this->NeedleBaseValidationPosition[0]=0;
  this->NeedleBaseValidationPosition[1]=0;
  this->NeedleBaseValidationPosition[2]=0;
  this->ErrorDistanceFromNeedleLine = 0.0;
  this->ErrorVector[0] = 0.0;
  this->ErrorVector[1] = 0.0;
  this->ErrorVector[2] = 0.0;
}

//----------------------------------------------------------------------------
vtkProstateNavTargetDescriptor::~vtkProstateNavTargetDescriptor(void)
{
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetDescriptor::DeepCopy(vtkProstateNavTargetDescriptor& src)
{
  this->Name=src.Name;
  this->Comment=src.Comment;
  this->RASLocation[0]=src.RASLocation[0];
  this->RASLocation[1]=src.RASLocation[1];
  this->RASLocation[2]=src.RASLocation[2];
  this->RASOrientation[0]=src.RASOrientation[0];
  this->RASOrientation[1]=src.RASOrientation[1];
  this->RASOrientation[2]=src.RASOrientation[2];
  this->RASOrientation[3]=src.RASOrientation[3];
  this->TargetingVolumeRef=src.TargetingVolumeRef;
  this->TargetingVolumeFoR=src.TargetingVolumeFoR;
  this->NeedleID=src.NeedleID;
  this->FiducialID=src.FiducialID;
  this->TargetCompleted=src.TargetCompleted;
  this->TargetCompletedInfo=src.TargetCompletedInfo;
  this->TargetValidated=src.TargetValidated;
  this->ValidationVolumeRef=src.ValidationVolumeRef;
  this->ValidationVolumeFoR=src.ValidationVolumeFoR;
  this->NeedleTipValidationPosition[0]=src.NeedleTipValidationPosition[0];
  this->NeedleTipValidationPosition[1]=src.NeedleTipValidationPosition[1];
  this->NeedleTipValidationPosition[2]=src.NeedleTipValidationPosition[2];
  this->NeedleBaseValidationPosition[0]=src.NeedleBaseValidationPosition[0];
  this->NeedleBaseValidationPosition[1]=src.NeedleBaseValidationPosition[1];
  this->NeedleBaseValidationPosition[2]=src.NeedleBaseValidationPosition[2];
  this->ErrorDistanceFromNeedleLine=src.ErrorDistanceFromNeedleLine;
  this->ErrorVector[0]=src.ErrorVector[0];
  this->ErrorVector[1]=src.ErrorVector[1];
  this->ErrorVector[2]=src.ErrorVector[2];
}

//----------------------------------------------------------------------------
void vtkProstateNavTargetDescriptor::PrintSelf(ostream& os, vtkIndent indent)
{
  
  this->Superclass::PrintSelf(os,indent);

  /* :TODO: implement this
  os << indent << "PlanningVolumeRef:   " << 
   (this->PlanningVolumeRef ? this->PlanningVolumeRef : "(none)") << "\n";
 */
}
