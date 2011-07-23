/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include "vtkProstateNavVersion.h"

#include "vtkObjectFactory.h"

#include "vtkMRMLProstateNavManagerNode.h"
#include "vtkMRMLIGTLConnectorNode.h"
#include "vtkStringArray.h"

#include "vtkMRMLScene.h"

#include "vtkProstateNavStep.h"
#include "vtkMRMLRobotNode.h"

#include "vtkSlicerComponentGUI.h" // for vtkSetAndObserveMRMLNodeEventsMacro
#include "vtkSmartPointer.h"

#include "vtkXMLUtilities.h"
#include "vtkXMLDataElement.h"

//------------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode* vtkMRMLProstateNavManagerNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLProstateNavManagerNode"); if(ret)
    {
      return (vtkMRMLProstateNavManagerNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLProstateNavManagerNode;
}

//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLProstateNavManagerNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLProstateNavManagerNode");
  if(ret)
    {
      return (vtkMRMLProstateNavManagerNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLProstateNavManagerNode;
}

//----------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode::vtkMRMLProstateNavManagerNode()
{
  this->Initialized=false;

  this->StepList=vtkStringArray::New();
  this->CurrentStep = 0;

  this->CurrentNeedleIndex=-1;
  this->CurrentTargetIndex=-1;

  this->TargetDescriptorsVector.clear();

  this->TargetingVolumeNodeRef=NULL;
  this->VerificationVolumeNodeRef=NULL;

  this->TargetPlanListNodeID=NULL;
  this->TargetPlanListNode=NULL;

  this->RobotNodeID=NULL;
  this->RobotNode=NULL;

  this->HideFromEditorsOff();
}

//----------------------------------------------------------------------------
vtkMRMLProstateNavManagerNode::~vtkMRMLProstateNavManagerNode()
{  
  this->StepList->Delete();
  this->StepList=NULL;

  SetTargetingVolumeNodeRef(NULL);
  SetVerificationVolumeNodeRef(NULL);  
  
  if (this->TargetPlanListNodeID) 
  {
    SetAndObserveTargetPlanListNodeID(NULL);
  }
  if (this->RobotNodeID) 
  {
    SetAndObserveRobotNodeID(NULL);
  }

  for (std::vector<vtkProstateNavTargetDescriptor*>::iterator it=this->TargetDescriptorsVector.begin(); it!=this->TargetDescriptorsVector.end(); ++it)
    {   
    if ((*it)!=NULL)
      {
      (*it)->Delete();
      (*it)=NULL;
      }
    }
  this->TargetDescriptorsVector.clear();
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " ProstateNavModuleVersion=\"" << GetProstateNavModuleVersion() << "\"";
  of << indent << " CurrentWorkflowStep=\"" << this->CurrentStep << "\"";    

  of << indent << " CurrentNeedleIndex=\"" << this->CurrentNeedleIndex << "\"";
  for (unsigned int needleInd=0; needleInd<this->NeedlesVector.size(); needleInd++)
  {
    of << " Needle" << needleInd << "_ID=\"" << this->NeedlesVector[needleInd].mID << "\"";
    of << " Needle" << needleInd << "_TargetNamePrefix=\"" << this->NeedlesVector[needleInd].mTargetNamePrefix << "\"";
    of << " Needle" << needleInd << "_Description=\"" << this->NeedlesVector[needleInd].mDescription << "\"";
    of << " Needle" << needleInd << "_Length=\"" << this->NeedlesVector[needleInd].mLength << "\"";
    of << " Needle" << needleInd << "_TipLength=\"" << this->NeedlesVector[needleInd].mTipLength << "\"";
    of << " Needle" << needleInd << "_Throw=\"" << this->NeedlesVector[needleInd].mThrow << "\"";
    of << " Needle" << needleInd << "_TargetLength=\"" << this->NeedlesVector[needleInd].mTargetLength << "\"";
    of << " Needle" << needleInd << "_TargetBase=\"" << this->NeedlesVector[needleInd].mTargetBase << "\"";
    of << " Needle" << needleInd << "_Diameter=\"" << this->NeedlesVector[needleInd].mDiameter << "\"";
    of << " Needle" << needleInd << "_LastTargetIndex=\"" << this->NeedlesVector[needleInd].mLastTargetIndex << "\"";
  }

  of << indent << " CurrentTargetIndex=\"" << this->CurrentTargetIndex << "\"";
  for (unsigned int targetInd=0; targetInd<this->TargetDescriptorsVector.size(); targetInd++)
  {
    of << " Target" << targetInd << "_Name=\"" << this->TargetDescriptorsVector[targetInd]->GetName() << "\"";
    of << " Target" << targetInd << "_Comment=\"" << this->TargetDescriptorsVector[targetInd]->GetComment() << "\"";
    of << " Target" << targetInd << "_RASLocation=\""
      << this->TargetDescriptorsVector[targetInd]->GetRASLocation()[0] << " "
      << this->TargetDescriptorsVector[targetInd]->GetRASLocation()[1] << " "
      << this->TargetDescriptorsVector[targetInd]->GetRASLocation()[2] << "\"";
    of << " Target" << targetInd << "_RASOrientation=\""
      << this->TargetDescriptorsVector[targetInd]->GetRASOrientation()[0] << " "
      << this->TargetDescriptorsVector[targetInd]->GetRASOrientation()[1] << " "
      << this->TargetDescriptorsVector[targetInd]->GetRASOrientation()[2] << " "
      << this->TargetDescriptorsVector[targetInd]->GetRASOrientation()[3] << "\"";
    of << " Target" << targetInd << "_TargetingVolumeRef=\"" << this->TargetDescriptorsVector[targetInd]->GetTargetingVolumeRef() << "\"";
    of << " Target" << targetInd << "_TargetingVolumeFoR=\"" << this->TargetDescriptorsVector[targetInd]->GetTargetingVolumeFoR() << "\"";
    of << " Target" << targetInd << "_NeedleID=\"" << this->TargetDescriptorsVector[targetInd]->GetNeedleID() << "\"";
    of << " Target" << targetInd << "_FiducialID=\"" << this->TargetDescriptorsVector[targetInd]->GetFiducialID() << "\"";
    of << " Target" << targetInd << "_TargetCompleted=\"" << this->TargetDescriptorsVector[targetInd]->GetTargetCompleted() << "\"";
    of << " Target" << targetInd << "_TargetCompletedInfo=\"" << this->TargetDescriptorsVector[targetInd]->GetTargetCompletedInfo() << "\"";
    of << " Target" << targetInd << "_TargetValidated=\"" << this->TargetDescriptorsVector[targetInd]->GetTargetValidated() << "\"";
    of << " Target" << targetInd << "_ValidationVolumeRef=\"" << this->TargetDescriptorsVector[targetInd]->GetValidationVolumeRef() << "\"";
    of << " Target" << targetInd << "_ValidationVolumeFoR=\"" << this->TargetDescriptorsVector[targetInd]->GetValidationVolumeFoR() << "\"";
    of << " Target" << targetInd << "_NeedleTipValidationPosition=\""
      << this->TargetDescriptorsVector[targetInd]->GetNeedleTipValidationPosition()[0] << " "
      << this->TargetDescriptorsVector[targetInd]->GetNeedleTipValidationPosition()[1] << " "
      << this->TargetDescriptorsVector[targetInd]->GetNeedleTipValidationPosition()[2] << "\"";
    of << " Target" << targetInd << "_NeedleBaseValidationPosition=\""
      << this->TargetDescriptorsVector[targetInd]->GetNeedleBaseValidationPosition()[0] << " "
      << this->TargetDescriptorsVector[targetInd]->GetNeedleBaseValidationPosition()[1] << " "
      << this->TargetDescriptorsVector[targetInd]->GetNeedleBaseValidationPosition()[2] << "\"";
    of << " Target" << targetInd << "_ErrorDistanceFromNeedleLine=\"" << this->TargetDescriptorsVector[targetInd]->GetErrorDistanceFromNeedleLine() << "\"";
    of << " Target" << targetInd << "_ErrorVector=\""
      << this->TargetDescriptorsVector[targetInd]->GetErrorVector()[0] << " "
      << this->TargetDescriptorsVector[targetInd]->GetErrorVector()[1] << " "
      << this->TargetDescriptorsVector[targetInd]->GetErrorVector()[2] << "\"";
  }

  if (this->TargetingVolumeNodeRef != NULL) 
  {
    of << indent << " TargetingVolumeNodeRef=\"" << this->TargetingVolumeNodeRef << "\"";
  }
  if (this->VerificationVolumeNodeRef != NULL) 
  {
    of << indent << " VerificationVolumeNodeRef=\"" << this->VerificationVolumeNodeRef << "\"";
  }
  if (this->TargetPlanListNodeID != NULL) 
  {
    of << indent << " TargetPlanListNodeRef=\"" << this->TargetPlanListNodeID << "\"";
  }
  if (this->RobotNodeID != NULL) 
  {
    of << indent << " RobotNodeRef=\"" << this->RobotNodeID << "\"";
  }

}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);

  this->NeedlesVector.clear();
  this->TargetDescriptorsVector.clear();

  const char* attName;
  const char* attValue;
  
  while (*atts != NULL)
  {
    attName = *(atts++);
    attValue = *(atts++);

    // ProstateNavModuleVersion - ignore (may be used in the future for backward compatibility)

    if (!strcmp(attName, "CurrentWorkflowStep"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->CurrentStep;
    }
    else if (!strcmp(attName, "CurrentNeedleIndex"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->CurrentNeedleIndex;
    }
    else if (!strcmp(attName, "CurrentTargetIndex"))
    {
      std::stringstream ss;
      ss << attValue;
      ss >> this->CurrentTargetIndex;
    }
    else if (!strcmp(attName, "TargetingVolumeNodeRef"))
    {
      this->SetTargetingVolumeNodeRef(attValue);
    }
    else if (!strcmp(attName, "VerificationVolumeNodeRef"))
    {
      this->SetVerificationVolumeNodeRef(attValue);
    }
    else if (!strcmp(attName, "TargetPlanListNodeRef"))
    {
      this->SetAndObserveTargetPlanListNodeID(NULL);
      this->SetTargetPlanListNodeID(attValue);
    }
    else if (!strcmp(attName, "RobotNodeRef"))
    {
      this->SetAndObserveRobotNodeID(NULL);
      this->SetRobotNodeID(attValue);
    }

    unsigned int sectionInd=0;
    std::string sectionName;
    if (GetAttNameSection(attName,"Needle", sectionInd, sectionName))
    {
      if (sectionInd>=this->NeedlesVector.size())
      {
        this->NeedlesVector.resize(sectionInd+1);
      }
      NeedleDescriptorStruct *needle=&(this->NeedlesVector[sectionInd]);

      if (!sectionName.compare("ID"))
      {
        needle->mID=attValue;
      }
      else if (!sectionName.compare("TargetNamePrefix"))
      {
        needle->mTargetNamePrefix=attValue;
      }
      else if (!sectionName.compare("Description"))
      {
        needle->mDescription=attValue;
      }
      else if (!sectionName.compare("Length"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mLength;
      }
      else if (!sectionName.compare("TipLength"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mTipLength;
      }
      else if (!sectionName.compare("Throw"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mThrow;
      }
      else if (!sectionName.compare("TargetLength"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mTargetLength;
      }
      else if (!sectionName.compare("TargetBase"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mTargetBase;
      }
      else if (!sectionName.compare("Diameter"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mDiameter;
      }
      else if (!sectionName.compare("LastTargetIndex"))
      {
        std::stringstream ss;
        ss << attValue;
        ss >> needle->mLastTargetIndex;
      }
    }
    if (GetAttNameSection(attName,"Target", sectionInd, sectionName))
    {
      if (sectionInd>=this->TargetDescriptorsVector.size())
      {
        this->TargetDescriptorsVector.resize(sectionInd+1, NULL);
      }
      vtkProstateNavTargetDescriptor *targetDesc=this->TargetDescriptorsVector[sectionInd];
      if (targetDesc==NULL)
      {
        // the descriptor has not been created yet
        targetDesc=vtkProstateNavTargetDescriptor::New();
        this->TargetDescriptorsVector[sectionInd]=targetDesc;
      }      
      if (!sectionName.compare("Name"))
      {
        targetDesc->SetName(attValue);
      }
      else if (!sectionName.compare("Comment"))
      {
        targetDesc->SetComment(attValue);
      }      
      else if (!sectionName.compare("RASLocation"))
      {
        std::stringstream ss;
        ss << attValue;
        double ras[3]={0,0,0};
        ss >> ras[0];
        ss >> ras[1];
        ss >> ras[2];
        targetDesc->SetRASLocation(ras);
      }
      else if (!sectionName.compare("RASOrientation"))
      {
        std::stringstream ss;
        ss << attValue;
        double wxyz[4]={0,0,0};
        ss >> wxyz[0];
        ss >> wxyz[1];
        ss >> wxyz[2];
        ss >> wxyz[3];
        targetDesc->SetRASOrientation(wxyz);
      }
      else if (!sectionName.compare("TargetingVolumeRef"))
      {
        targetDesc->SetTargetingVolumeRef(attValue);
      }
      else if (!sectionName.compare("TargetingVolumeFoR"))
      {
        targetDesc->SetTargetingVolumeFoR(attValue);
      }
      else if (!sectionName.compare("NeedleID"))
      {
        targetDesc->SetNeedleID(attValue);
      }
      else if (!sectionName.compare("FiducialID"))
      {
        // :CLUTCH: When loading a FiducialList from file the original FiducialIDs are lost,
        // so we cannot link the targets to the fiducial based on the FiducialID.
        // The solution (kind of a hack, until this is cleaned up in Slicer) is to not fill
        // the FiducialID attribute and let vtkProstateNavLogic::UpdateTargetListFromMRML
        // do the linking.
        // targetDesc->SetFiducialID(attValue); - this should be the correct way
        targetDesc->SetFiducialID("");
      }
      else if (!sectionName.compare("TargetCompleted"))
      {
        std::stringstream ss;
        ss << attValue;
        bool b;
        ss >> b;
        targetDesc->SetTargetCompleted(b);
      }
      else if (!sectionName.compare("TargetCompletedInfo"))
      {
        targetDesc->SetTargetCompletedInfo(attValue);
      }
      else if (!sectionName.compare("TargetValidated"))
      {
        std::stringstream ss;
        ss << attValue;
        bool b;
        ss >> b;
        targetDesc->SetTargetValidated(b);
      }
      else if (!sectionName.compare("TargetCompletedInfo"))
      {
        targetDesc->SetTargetCompletedInfo(attValue);
      }
      else if (!sectionName.compare("ValidationVolumeRef"))
      {
        targetDesc->SetValidationVolumeRef(attValue);
      }
      else if (!sectionName.compare("ValidationVolumeFoR"))
      {
        targetDesc->SetValidationVolumeFoR(attValue);
      }
      else if (!sectionName.compare("NeedleTipValidationPosition"))
      {
        std::stringstream ss;
        ss << attValue;
        double ras[3]={0,0,0};
        ss >> ras[0];
        ss >> ras[1];
        ss >> ras[2];
        targetDesc->SetNeedleTipValidationPosition(ras);
      }
      else if (!sectionName.compare("NeedleBaseValidationPosition"))
      {
        std::stringstream ss;
        ss << attValue;
        double ras[3]={0,0,0};
        ss >> ras[0];
        ss >> ras[1];
        ss >> ras[2];
        targetDesc->SetNeedleBaseValidationPosition(ras);
      }
      else if (!sectionName.compare("ErrorDistanceFromNeedleLine"))
      {
        std::stringstream ss;
        ss << attValue;
        double val;
        ss >> val;
        targetDesc->SetErrorDistanceFromNeedleLine(val);
      }
      else if (!sectionName.compare("ErrorVector"))
      {
        std::stringstream ss;
        ss << attValue;
        double ras[3]={0,0,0};
        ss >> ras[0];
        ss >> ras[1];
        ss >> ras[2];
        targetDesc->SetErrorVector(ras);
      }

    }
  }
  
  this->Initialized=true;
}

//----------------------------------------------------------------------------
// Separate attName = Needle123_Length to a sectionInd=123 and sectionName=Length
bool vtkMRMLProstateNavManagerNode::GetAttNameSection(const std::string& attName, const std::string& groupName, unsigned int &sectionInd, std::string &sectionName)
{
  int groupNameLen=groupName.length();
  if (attName.compare(0,groupNameLen,groupName)!=0)
  {
    // group name doesn't match
    return false;
  }
  // search for the first separator character after the group name
  std::string::size_type separatorPos=attName.find('_', groupNameLen);
  if (separatorPos==std::string::npos)
  {
    // separator not found
    return false;
  }
  sectionName = attName.substr ( separatorPos+1, attName.length()-separatorPos+1 );
  if (sectionName.empty())
  {
    // failed to read sectionName
    return false;
  }
  std::string indexString = attName.substr ( groupNameLen, separatorPos-groupNameLen );
  std::stringstream ss;
  ss << indexString;
  ss.clear();
  ss >> sectionInd;
  if (!ss)
  {
    // failed to read sectionIndex
    return false;
  }
  return true;
}

//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLProstateNavManagerNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  vtkMRMLProstateNavManagerNode *node = (vtkMRMLProstateNavManagerNode *) anode;

  this->CurrentStep = node->CurrentStep;

  SetTargetingVolumeNodeRef(node->TargetingVolumeNodeRef);
  SetVerificationVolumeNodeRef(node->VerificationVolumeNodeRef);  
  
  this->SetAndObserveTargetPlanListNodeID(NULL); // remove observers
  this->SetTargetPlanListNodeID(node->TargetPlanListNodeID);
  
  this->SetAndObserveRobotNodeID(NULL); // remove observers
  this->SetRobotNodeID(node->RobotNodeID);

  this->StepList->Reset();
  for (int i=0; i>node->StepList->GetSize(); i++)
  {
    this->StepList->SetValue(i, node->StepList->GetValue(i));
  }

  this->HideFromEditors=node->HideFromEditors;

  //copy the contents of the other
  this->NeedlesVector.assign(node->NeedlesVector.begin(), node->NeedlesVector.end() );

  // clean up old fiducial list vector
  for (std::vector<vtkProstateNavTargetDescriptor*>::iterator it=this->TargetDescriptorsVector.begin(); it!=this->TargetDescriptorsVector.end(); ++it)
    {   
    if ((*it)!=NULL)
      {
      (*it)->Delete();
      (*it)=NULL;
      }
    }
  this->TargetDescriptorsVector.clear();
  //copy the contents of the other
  vtkProstateNavTargetDescriptor* tdesc=NULL;
  for (std::vector<vtkProstateNavTargetDescriptor*>::iterator it=node->TargetDescriptorsVector.begin(); it!=node->TargetDescriptorsVector.end(); ++it)
    {    
    tdesc=vtkProstateNavTargetDescriptor::New();
    tdesc->DeepCopy(*(*it));
    this->TargetDescriptorsVector.push_back(tdesc);
    }

}

//-----------------------------------------------------------
void vtkMRMLProstateNavManagerNode::UpdateReferences()
{
   Superclass::UpdateReferences();

  if (this->TargetingVolumeNodeRef != NULL && this->Scene->GetNodeByID(this->TargetingVolumeNodeRef) == NULL)
    {
    this->SetTargetingVolumeNodeRef(NULL);
    }
  if (this->VerificationVolumeNodeRef != NULL && this->Scene->GetNodeByID(this->VerificationVolumeNodeRef) == NULL)
    {
    this->SetVerificationVolumeNodeRef(NULL);
    }

  if (this->TargetPlanListNodeID != NULL && this->Scene->GetNodeByID(this->TargetPlanListNodeID) == NULL)
    {
    this->SetAndObserveTargetPlanListNodeID(NULL);
    }
  if (this->RobotNodeID != NULL && this->Scene->GetNodeByID(this->RobotNodeID) == NULL)
    {
    this->SetAndObserveRobotNodeID(NULL);
    }

}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);

  if (this->TargetingVolumeNodeRef && !strcmp(oldID, this->TargetingVolumeNodeRef))
    {
    this->SetTargetingVolumeNodeRef(newID);
    }
  if (this->VerificationVolumeNodeRef && !strcmp(oldID, this->VerificationVolumeNodeRef))
    {
    this->SetVerificationVolumeNodeRef(newID);
    }

  if (this->TargetPlanListNodeID && !strcmp(oldID, this->TargetPlanListNodeID))
    {
    this->SetAndObserveTargetPlanListNodeID(newID);
    }
  if (this->RobotNodeID && !strcmp(oldID, this->RobotNodeID))
    {
    this->SetAndObserveRobotNodeID(newID);
    }
}

//-----------------------------------------------------------
void vtkMRMLProstateNavManagerNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);

   this->SetAndObserveTargetPlanListNodeID(this->TargetPlanListNodeID);
   this->SetAndObserveRobotNodeID(this->RobotNodeID);    
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);

  if (this->TargetPlanListNode && this->TargetPlanListNode == vtkMRMLFiducialListNode::SafeDownCast(caller) &&
      event ==  vtkCommand::ModifiedEvent)
    {
    //this->ModifiedSinceReadOn();
    //this->InvokeEvent(vtkMRMLVolumeNode::ImageDataModifiedEvent, NULL);
    //this->UpdateFromMRML();
    return;
    }

  return;
}


//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
  // :TODO: implement this
}


//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::GetNumberOfSteps()
{
  if (this->StepList==NULL)
  {
    return 0;
  }
  return this->StepList->GetNumberOfValues();
}


//----------------------------------------------------------------------------
const char* vtkMRMLProstateNavManagerNode::GetStepName(int i)
{
  if (i>=0 && i < this->StepList->GetNumberOfValues())
    {
      return this->StepList->GetValue(i);
    }
  else
    {
    return NULL;
    }
}
//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::SwitchStep(int newStep)
{
  if (newStep<0 || newStep>=GetNumberOfSteps())
  {
    return 0;
  }
  this->CurrentStep = newStep; 

  // Tentatively, this function calls vtkMRMLBrpRobotCommandNode::SwitchStep().
  // (we cannot test with the robot without sending workphase commands...)
  if (this->RobotNode)
    {
    this->RobotNode->SwitchStep(this->StepList->GetValue(newStep).c_str());
    }
  return 1;
}


//----------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::GetCurrentStep()
{
  return this->CurrentStep;
}

//----------------------------------------------------------------------------
vtkStdString vtkMRMLProstateNavManagerNode::GetWorkflowStepsString()
{
  vtkStdString workflowSteps;
  for (int i=0; i<this->StepList->GetNumberOfValues(); i++)
    {
    workflowSteps += this->StepList->GetValue(i);
    if (i<this->StepList->GetNumberOfValues()-1)
      {
      workflowSteps += " "; // add separator after each step but the last one
      }
    }
  return workflowSteps;
}

//----------------------------------------------------------------------------
bool vtkMRMLProstateNavManagerNode::SetWorkflowStepsFromString(const vtkStdString& workflowStepsString)
{ 
  if (this->StepList==NULL)
  {
    // the manager node is being destroyed, don't process any events
    return false;
  }
  this->StepList->Reset();

  if (workflowStepsString.empty())
    {
    return false;
    }

  vtkstd::stringstream workflowStepsStream(workflowStepsString);
  vtkstd::string stepName;
  while(workflowStepsStream>>stepName)
    {
    this->StepList->InsertNextValue(stepName);
    }
  Modified();
  return true;
}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveTargetPlanListNodeID(const char *nodeID)
{
  vtkSetAndObserveMRMLObjectMacro(this->TargetPlanListNode, NULL);
  this->SetTargetPlanListNodeID(nodeID);
  vtkMRMLFiducialListNode *tnode = this->GetTargetPlanListNode();
  vtkSetAndObserveMRMLObjectMacro(this->TargetPlanListNode, tnode);
}

//----------------------------------------------------------------------------
vtkMRMLFiducialListNode* vtkMRMLProstateNavManagerNode::GetTargetPlanListNode()
{
  vtkMRMLFiducialListNode* node = NULL;
  if (this->GetScene() && this->TargetPlanListNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->TargetPlanListNodeID);
    node = vtkMRMLFiducialListNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetAndObserveRobotNodeID(const char *nodeID)
{
  if (nodeID!=NULL && this->RobotNodeID!=NULL && strcmp(nodeID,this->RobotNodeID)==0)
  {
    // no change in ID, check if node pointer is changed
    vtkMRMLRobotNode* node = vtkMRMLRobotNode::SafeDownCast(this->GetScene()->GetNodeByID(nodeID));
    if (node==this->RobotNode)
    {
      // no change at all
      return;
    }
  }
  vtkSetAndObserveMRMLObjectMacro(this->RobotNode, NULL);

  this->SetRobotNodeID(nodeID);
  vtkMRMLRobotNode *tnode = this->GetRobotNode();

  vtkSetAndObserveMRMLObjectMacro(this->RobotNode, tnode);

  // update workflow steps from the new Robot node
  // (the Manager node is the owner of workflow steps, as it may add additional steps or modify the steps that
  // it gets from the robot)
  if (this->GetScene() && nodeID != NULL )
  {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(nodeID);
    vtkMRMLRobotNode* node = vtkMRMLRobotNode::SafeDownCast(snode);
    if (node != NULL)
    {      
      SetWorkflowStepsFromString(node->GetWorkflowStepsString());    
    }
    else
    {
      SetWorkflowStepsFromString("");
    }
  }
  else
  {
    SetWorkflowStepsFromString("");
  }
 
}

//----------------------------------------------------------------------------
vtkMRMLRobotNode* vtkMRMLProstateNavManagerNode::GetRobotNode()
{
  vtkMRMLRobotNode* node = NULL;
  if (this->GetScene() && this->RobotNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->RobotNodeID);
    node = vtkMRMLRobotNode::SafeDownCast(snode);
    }
  return node;
}



//-------------------------------------------------------------------------------
unsigned int vtkMRMLProstateNavManagerNode::AddTargetDescriptor(vtkProstateNavTargetDescriptor* target)
{
  unsigned int index = this->TargetDescriptorsVector.size();
  target->Register(this);
  this->TargetDescriptorsVector.push_back(target);
  return index;
}
//-------------------------------------------------------------------------------
vtkProstateNavTargetDescriptor* vtkMRMLProstateNavManagerNode::GetTargetDescriptorAtIndex(unsigned int index)
{
  if (index < this->TargetDescriptorsVector.size())
    {
    return this->TargetDescriptorsVector[index];
    }
  else
    {
    return NULL;
    }
}
//-------------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::RemoveTargetDescriptorAtIndex(unsigned int index)
{
  if (index >= this->TargetDescriptorsVector.size())
    {
    return 0;
    }
  vtkProstateNavTargetDescriptor* targetDesc=GetTargetDescriptorAtIndex(index);
  if (targetDesc!=NULL)
  {
    targetDesc->Delete();
  }
  this->TargetDescriptorsVector.erase(this->TargetDescriptorsVector.begin()+index);
  return 1;
}
//-------------------------------------------------------------------------------
int vtkMRMLProstateNavManagerNode::SetCurrentTargetIndex(int index)
{
  if (index >= (int)this->TargetDescriptorsVector.size())
    {
    // invalid index, do not change current
    return this->CurrentTargetIndex;
    }
  if (this->CurrentTargetIndex==index)
  {
    // no change
    return this->CurrentTargetIndex;
  }
  this->CurrentTargetIndex=index;

  if (GetRobotNode())
  {
    vtkProstateNavTargetDescriptor* targetDesc=GetTargetDescriptorAtIndex(this->CurrentTargetIndex);
    NeedleDescriptorStruct *needle = GetNeedle(targetDesc); 
    GetRobotNode()->ShowRobotAtTarget(targetDesc, needle);
  }

  this->Modified();
  this->InvokeEvent(vtkMRMLProstateNavManagerNode::CurrentTargetChangedEvent);
  return this->CurrentTargetIndex;
}

//-------------------------------------------------------------------------------
bool vtkMRMLProstateNavManagerNode::AddTargetToFiducialList(double targetRAS[3], unsigned int fiducialListIndex, unsigned int targetNr, int & fiducialIndex)
{
  if (fiducialListIndex >= this->NeedlesVector.size())
    {
    vtkErrorMacro("Invalid fiducial list index");
    return false;
    }
  std::string targetNamePrefix = this->NeedlesVector[fiducialListIndex].mTargetNamePrefix;  

  vtkMRMLFiducialListNode* fidNode=this->GetTargetPlanListNode();
  if (fidNode==NULL)
    {
    vtkErrorMacro("No fiducial node is selected");
    return false;
    }

  int modifyOld=fidNode->StartModify();
  fiducialIndex = fidNode->AddFiducialWithXYZ(targetRAS[0], targetRAS[1], targetRAS[2], false);
  bool success=false;
  if (fiducialIndex==-1)
  {
    success=false;
  }
  else
  {
    std::ostrstream os;
    os << targetNamePrefix << "_" << targetNr << std::ends;
    fidNode->SetNthFiducialLabelText(fiducialIndex, os.str());
    fidNode->SetNthFiducialID(fiducialIndex, os.str());
    os.rdbuf()->freeze();
    fidNode->SetNthFiducialVisibility(fiducialIndex, true);    
    success=true;
  }
  fidNode->EndModify(modifyOld);
  // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
  fidNode->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
  return true;
}
//-------------------------------------------------------------------------------
bool vtkMRMLProstateNavManagerNode::GetTargetFromFiducialList(int fiducialListIndex, int fiducialIndex, double &r, double &a, double &s)
{
    if (fiducialListIndex < (int)this->NeedlesVector.size() && fiducialListIndex != -1)
      {
      vtkMRMLFiducialListNode* fidNode=NULL;
      fidNode=this->GetTargetPlanListNode();
      if (fidNode!=NULL && fiducialIndex >=0 && fiducialIndex < fidNode->GetNumberOfFiducials())
        {
        float *ras = new float[3];
        ras = fidNode->GetNthFiducialXYZ(fiducialIndex);
        r = ras[0];
        a = ras[1];
        s = ras[2];
        return true;
        }
      }
    return false;
    
}

//-------------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::SetFiducialColor(int fiducialIndex, bool selected)
{
  vtkMRMLFiducialListNode* fidNode=this->GetTargetPlanListNode();
  if (fidNode==NULL)
  {
    vtkErrorMacro("No fiducial list node is selected");
    return;
  }
  int oldModify=fidNode->StartModify();
  fidNode->SetNthFiducialSelected(fiducialIndex, selected);
  fidNode->EndModify(oldModify);
  // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
  fidNode->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
}

//------------------------------------------------------------------------------
void vtkMRMLProstateNavManagerNode::Init(const char* defaultNeedleListConfig)
{
  if (this->Initialized)
  {
    return;
  } 

  ReadNeedleListFromConfigXml(defaultNeedleListConfig);

  if (GetTargetPlanListNode()==NULL && this->Scene!=NULL)
  {
    // the plan list node hasn't been created yet
    // create targeting fiducials list
    vtkSmartPointer<vtkMRMLFiducialListNode> targetFidList = vtkSmartPointer<vtkMRMLFiducialListNode>::New();  
    targetFidList->SetLocked(true);
    targetFidList->SetName("Tgts");
    targetFidList->SetDescription("ProstateNav target point list");
    targetFidList->SetColor(1.0,1.0,0);
    targetFidList->SetSelectedColor(1.0, 0.0, 0.0);
    targetFidList->SetGlyphType(vtkMRMLFiducialListNode::Sphere3D);
    targetFidList->SetOpacity(0.7);
    targetFidList->SetAllFiducialsVisibility(true);
    targetFidList->SetSymbolScale(5);
    targetFidList->SetTextScale(5);    
    GetScene()->AddNode(targetFidList);
    SetAndObserveTargetPlanListNodeID(targetFidList->GetID());
  }
  
  this->Initialized=true;
}

bool vtkMRMLProstateNavManagerNode::ReadNeedleListFromConfigXml(const char* needleListConfigStr)
{  
  vtkXMLDataElement *config=vtkXMLUtilities::ReadElementFromString(needleListConfigStr);
  if (config==NULL)
  {
    return false;
  }
  if (strcmp(config->GetName(), "NeedleList")!=0)
  {
    config->Delete();
    vtkWarningMacro("Cannot read needle configuration (cannot parse: "<<needleListConfigStr<<")");
    return false;
  }

  vtkXMLDataElement* needleElem=NULL;
  for (int elemIndex=0; elemIndex<config->GetNumberOfNestedElements(); elemIndex++)
  {
    needleElem=config->GetNestedElement(elemIndex);
    if (needleElem!=NULL && strcmp(needleElem->GetName(),"Needle")==0)
    {
      NeedleDescriptorStruct needle;
      if (needleElem->GetAttribute("ID")!=NULL)
      {
        needle.mID=needleElem->GetAttribute("ID");
      }
      if (needleElem->GetAttribute("TargetNamePrefix")!=NULL)
      {
        needle.mTargetNamePrefix=needleElem->GetAttribute("TargetNamePrefix");
      }
      if (needleElem->GetAttribute("Description")!=NULL)
      {
        needle.mDescription = needleElem->GetAttribute("Description");
      } 
      needleElem->GetScalarAttribute("Length", needle.mLength);
      needleElem->GetScalarAttribute("TipLength", needle.mTipLength);
      needleElem->GetScalarAttribute("Throw", needle.mThrow);
      needleElem->GetScalarAttribute("TargetLength", needle.mTargetLength);      
      needleElem->GetScalarAttribute("TargetBase", needle.mTargetBase);      
      needleElem->GetScalarAttribute("Diameter", needle.mDiameter);      
      needleElem->GetScalarAttribute("LastTargetIndex", needle.mLastTargetIndex);
      this->NeedlesVector.push_back(needle);
    }
  }

  this->CurrentNeedleIndex=0;
  const char* defNeedleID=config->GetAttribute("DefaultNeedle");
  if (defNeedleID!=NULL)
  {
    int needleCount=GetNumberOfNeedles();
    for (int i=0; i<needleCount; i++)
    {
      if (this->NeedlesVector[i].mID.compare(defNeedleID)==0)
      {
        // this is the default needle
        this->CurrentNeedleIndex=i;
        break;
      }
    }
  }
  
  config->Delete();
  
  return true;
}

bool vtkMRMLProstateNavManagerNode::SetNeedle(unsigned int needleIndex, NeedleDescriptorStruct needleDesc)
{
  if (needleIndex >= this->NeedlesVector.size())
    {
    return false;
    }
  this->NeedlesVector[needleIndex]=needleDesc;
  return true;
}

bool vtkMRMLProstateNavManagerNode::GetNeedle(unsigned int needleIndex, NeedleDescriptorStruct &needleDesc)
{
  if (needleIndex >= this->NeedlesVector.size())
    {
    return false;
    }
  needleDesc=this->NeedlesVector[needleIndex];
  return true;
}

NeedleDescriptorStruct* vtkMRMLProstateNavManagerNode::GetNeedle(vtkProstateNavTargetDescriptor *targetDesc)
{
  if (targetDesc==NULL)
  {
    return NULL;
  }
  std::string needleID=targetDesc->GetNeedleID();
  int needleCount=GetNumberOfNeedles();
  for (int i=0; i<needleCount; i++)
  {
    if (this->NeedlesVector[i].mID.compare(needleID)==0)
    {
      // found
      return &(this->NeedlesVector[i]);
    }
  }
  // not found
  return NULL;
}

const char* vtkMRMLProstateNavManagerNode::GetProstateNavModuleVersion()
{
  return PROSTATE_NAV_MODULE_VERSION;
}

int vtkMRMLProstateNavManagerNode::GetProstateNavWorkingCopyRevision()
{
  return ProstateNav_REVISION;
}
