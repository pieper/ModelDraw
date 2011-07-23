/*=auto=========================================================================

  Portions (c) Copyright 2007 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: $
  Date:      $Date: $
  Version:   $Revision: $

=========================================================================auto=*/

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkProstateNavLogic.h"

#include "vtkMRMLModelDisplayNode.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "vtkMRMLLinearTransformNode.h"
#include "vtkSlicerApplication.h"
#include "vtkSlicerApplicationGUI.h"
#include "vtkSlicerFiducialsGUI.h"
#include "vtkSlicerColorLogic.h"
#include "vtkMRMLLabelMapVolumeDisplayNode.h"

#include "vtkMRMLProstateNavManagerNode.h"
#include "vtkMRMLRobotNode.h"

#include "vtkKWRadioButton.h"

// for DICOM read
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkMetaDataDictionary.h"
#include "itkMetaDataObject.h"
#include "itkGDCMImageIO.h"
#include "itkSpatialOrientationAdapter.h"

#include "vtkMRMLBrpRobotCommandNode.h"

#include "vtkProstateNavGUI.h"

#include "ProstateNavMath.h"

vtkCxxRevisionMacro(vtkProstateNavLogic, "$Revision: 1.9.12.1 $");
vtkStandardNewMacro(vtkProstateNavLogic);

//---------------------------------------------------------------------------
vtkProstateNavLogic::vtkProstateNavLogic()
{
  // Timer Handling
  this->DataCallbackCommand = vtkCallbackCommand::New();
  this->DataCallbackCommand->SetClientData( reinterpret_cast<void *> (this) );
  this->DataCallbackCommand->SetCallback(vtkProstateNavLogic::DataCallback);

  this->TimerOn = 0;
}


//---------------------------------------------------------------------------
vtkProstateNavLogic::~vtkProstateNavLogic()
{
  if (this->DataCallbackCommand)
  {
    this->DataCallbackCommand->Delete();
  }
}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::PrintSelf(ostream& os, vtkIndent indent)
{
    this->vtkObject::PrintSelf(os, indent);

    os << indent << "vtkProstateNavLogic:             " << this->GetClassName() << "\n";

}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::DataCallback(vtkObject *caller, 
                                       unsigned long eid, void *clientData, void *callData)
{
    vtkProstateNavLogic *self = reinterpret_cast<vtkProstateNavLogic *>(clientData);
    vtkDebugWithObjectMacro(self, "In vtkProstateNavLogic DataCallback");
    self->UpdateAll();
}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::UpdateAll()
{

}


int vtkProstateNavLogic::Enter()
{      
  vtkKWTkUtilities::CreateTimerHandler(this->GetGUI()->GetApplication(), 200, this, "TimerHandler");
  return 1;
}

//----------------------------------------------------------------------------
void vtkProstateNavLogic::TimerHandler()
{
  if (this->TimerOn)
  {
    vtkMRMLRobotNode* robot=GetRobotNode();
    if (robot!=NULL)
    {
      robot->OnTimer();
    }
  }
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotStop()
{

  std::cerr << "vtkProstateNavLogic::RobotStop()" << std::endl;
  return 1;

}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotMoveTo(float px, float py, float pz,
                                     float nx, float ny, float nz,
                                     float tx, float ty, float tz)
{

  std::cerr << "vtkProstateNavLogic::RobotMoveTo()" << std::endl;
  return 1;
}


//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotMoveTo(float position[3], float orientation[3])
{
  std::cerr << "vtkProstateNavLogic::RobotMoveTo()" << std::endl;

  return 1;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::RobotMoveTo()
{
  vtkMRMLRobotNode* robot=GetRobotNode();
  if (robot==NULL)
  {
    return 0; // failed
  }
  return robot->MoveTo(robot->GetTargetTransformNodeID());  
}
//---------------------------------------------------------------------------
int vtkProstateNavLogic::ScanStart()
{

  return 1;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::ScanPause()
{
  return 1;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::ScanStop()
{

  return 1;
}

//---------------------------------------------------------------------------
void vtkProstateNavLogic::SetSliceViewFromVolume(vtkMRMLVolumeNode *volumeNode)
{
  if (!volumeNode)
    {
    return;
    }

  vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> permutationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();
  vtkSmartPointer<vtkMatrix4x4> rotationMatrix = vtkSmartPointer<vtkMatrix4x4>::New();

  volumeNode->GetIJKToRASDirectionMatrix(matrix);
  vtkMRMLTransformNode *transformNode = volumeNode->GetParentTransformNode();
  if ( transformNode )
    {
    vtkSmartPointer<vtkMatrix4x4> rasToRAS = vtkSmartPointer<vtkMatrix4x4>::New();
    transformNode->GetMatrixTransformToWorld(rasToRAS);
    vtkMatrix4x4::Multiply4x4 (rasToRAS, matrix, matrix);
    }


  //slicerCerr("matrix");
  //slicerCerr("   " << matrix->GetElement(0,0) <<
//             "   " << matrix->GetElement(0,1) <<
  //           "   " << matrix->GetElement(0,2));
  //slicerCerr("   " << matrix->GetElement(1,0) <<
    //         "   " << matrix->GetElement(1,1) <<
    //         "   " << matrix->GetElement(1,2));
  //slicerCerr("   " << matrix->GetElement(2,0) <<
   //          "   " << matrix->GetElement(2,1) <<
    //         "   " << matrix->GetElement(2,2));

  int permutation[3];
  int flip[3];
  ProstateNavMath::ComputePermutationFromOrientation(matrix, permutation, flip);

  //slicerCerr("permutation " << permutation[0] << " " <<
//             permutation[1] << " " << permutation[2]);
  //slicerCerr("flip " << flip[0] << " " <<
  //           flip[1] << " " << flip[2]);

  permutationMatrix->SetElement(0,0,0);
  permutationMatrix->SetElement(1,1,0);
  permutationMatrix->SetElement(2,2,0);

  permutationMatrix->SetElement(0, permutation[0],
                     (flip[permutation[0]] ? -1 : 1));
  permutationMatrix->SetElement(1, permutation[1],
                     (flip[permutation[1]] ? -1 : 1));
  permutationMatrix->SetElement(2, permutation[2],
                     (flip[permutation[2]] ? -1 : 1));

  //slicerCerr("permutationMatrix");
  //slicerCerr("   " << permutationMatrix->GetElement(0,0) <<
//             "   " << permutationMatrix->GetElement(0,1) <<
  //           "   " << permutationMatrix->GetElement(0,2));
  //slicerCerr("   " << permutationMatrix->GetElement(1,0) <<
    //         "   " << permutationMatrix->GetElement(1,1) <<
      //       "   " << permutationMatrix->GetElement(1,2));
  //slicerCerr("   " << permutationMatrix->GetElement(2,0) <<
        //     "   " << permutationMatrix->GetElement(2,1) <<
          //   "   " << permutationMatrix->GetElement(2,2));

  permutationMatrix->Invert();
  vtkMatrix4x4::Multiply4x4(matrix, permutationMatrix, rotationMatrix); 

  vtkSlicerApplicationLogic *appLogic = this->GetGUI()->GetApplicationLogic();

  
  // Set the slice views to match the volume slice orientation
  for (int i = 0; i < 3; i++)
    {
    static const char *panes[3] = { "Red", "Yellow", "Green" };

    vtkMatrix4x4 *newMatrix = vtkMatrix4x4::New();

    vtkSlicerSliceLogic *slice = appLogic->GetSliceLogic(
      const_cast<char *>(panes[i]));
    
    vtkMRMLSliceNode *sliceNode = slice->GetSliceNode();

    // Need to find window center and rotate around that

    // Default matrix orientation for slice
    newMatrix->SetElement(0, 0, 0.0);
    newMatrix->SetElement(1, 1, 0.0);
    newMatrix->SetElement(2, 2, 0.0);
    if (i == 0)
      {
      newMatrix->SetElement(0, 0, -1.0);
      newMatrix->SetElement(1, 1, 1.0);
      newMatrix->SetElement(2, 2, 1.0);
      }
    else if (i == 1)
      {
      newMatrix->SetElement(1, 0, -1.0);
      newMatrix->SetElement(2, 1, 1.0);
      newMatrix->SetElement(0, 2, 1.0);
      }
    else if (i == 2)
      {
      newMatrix->SetElement(0, 0, -1.0);
      newMatrix->SetElement(2, 1, 1.0);
      newMatrix->SetElement(1, 2, 1.0);
      }

    // Next, set the orientation to match the volume
    sliceNode->SetOrientationToReformat();
    vtkMatrix4x4::Multiply4x4(rotationMatrix, newMatrix, newMatrix);
    sliceNode->SetSliceToRAS(newMatrix);
    sliceNode->UpdateMatrices();
    newMatrix->Delete();
    }

}
//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkProstateNavLogic::AddVolumeToScene(const char *fileName, VolumeType volumeType/*=VOL_GENERIC*/)
{
  if (fileName==0)
  {
    vtkErrorMacro("AddVolumeToScene: invalid filename");
    return 0;
  }

  vtksys_stl::string volumeNameString = vtksys::SystemTools::GetFilenameName(fileName);
  vtkMRMLScalarVolumeNode *volumeNode = this->AddArchetypeVolume(fileName, volumeNameString.c_str());

  if (volumeNode==NULL)
  {
    vtkErrorMacro("Error adding volume to the scene");
    return NULL;
  }

  vtkMRMLProstateNavManagerNode* manager=this->GUI->GetProstateNavManagerNode();
  if (manager==NULL)
  {
    vtkErrorMacro("Error adding volume to the scene, manager is invalid");
    return NULL;
  }

  this->SetSliceViewFromVolume(volumeNode);

  switch (volumeType)
  {
  case VOL_GENERIC:
    // don't store a reference in the manager node
    break;
  case VOL_TARGETING:
    manager->SetTargetingVolumeNodeRef(volumeNode->GetID());
    break;
  case VOL_VERIFICATION:
    manager->SetVerificationVolumeNodeRef(volumeNode->GetID());
    break;
  default:
    vtkErrorMacro("AddVolumeToScene: unknown volume type: " << volumeType);
  }
  
  volumeNode->Modified();
  this->Modified();

  return volumeNode;
}

//---------------------------------------------------------------------------
int vtkProstateNavLogic::SelectVolumeInScene(vtkMRMLScalarVolumeNode* volumeNode, VolumeType volumeType)
{
  if (volumeNode==0)
  {
    vtkErrorMacro("SelectVolumeInScene: invalid volume");
    return 0;
  }

  vtkMRMLProstateNavManagerNode* manager=this->GUI->GetProstateNavManagerNode();
  if (manager==NULL)
  {
    vtkErrorMacro("Error adding volume to the scene, manager is invalid");
    return 0;
  }

  this->SetSliceViewFromVolume(volumeNode);

  this->GetGUI()->GetApplicationLogic()->GetSelectionNode()->SetActiveVolumeID( volumeNode->GetID() );
  this->GetGUI()->GetApplicationLogic()->PropagateVolumeSelection();

  switch (volumeType)
  {
  case VOL_GENERIC:
    // don't store a reference in the manager node
    break;
  case VOL_TARGETING:
    manager->SetTargetingVolumeNodeRef(volumeNode->GetID());
    break;
  case VOL_VERIFICATION:
    manager->SetVerificationVolumeNodeRef(volumeNode->GetID());
    break;
  default:
    vtkErrorMacro("AddVolumeToScene: unknown volume type: " << volumeType);
  }
  
  //volumeNode->Modified();
  this->Modified();

  return 1;
}

//---------------------------------------------------------------------------
vtkMRMLScalarVolumeNode *vtkProstateNavLogic::AddArchetypeVolume(const char* fileName, const char *volumeName)
{
  // Set up storageNode
  vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode> storageNode = vtkSmartPointer<vtkMRMLVolumeArchetypeStorageNode>::New(); 
  storageNode->SetFileName(fileName);
  // check to see if can read this type of file
  if (storageNode->SupportedFileType(fileName) == 0)
    {
    vtkErrorMacro("AddArchetypeVolume: can't read this kind of file: " << fileName);
    return 0;
    }
  storageNode->SetCenterImage(false);
  storageNode->SetSingleFile(false);
  storageNode->SetUseOrientationFromFile(true);

  // Set up scalarNode
  vtkSmartPointer<vtkMRMLScalarVolumeNode> scalarNode = vtkSmartPointer<vtkMRMLScalarVolumeNode>::New();
  scalarNode->SetName(volumeName);
  scalarNode->SetLabelMap(false);

  // Set up displayNode
  vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode> displayNode = vtkSmartPointer<vtkMRMLScalarVolumeDisplayNode>::New();   
  displayNode->SetAutoWindowLevel(false);
  displayNode->SetInterpolate(true);  
  vtkSmartPointer<vtkSlicerColorLogic> colorLogic = vtkSmartPointer<vtkSlicerColorLogic>::New(); 
  displayNode->SetAndObserveColorNodeID(colorLogic->GetDefaultVolumeColorNodeID());
  
  // Add nodes to scene
  vtkDebugMacro("LoadArchetypeVolume: adding storage node to the scene");
  storageNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(storageNode);
  vtkDebugMacro("LoadArchetypeVolume: adding display node to the scene");
  displayNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(displayNode);
  vtkDebugMacro("LoadArchetypeVolume: adding scalar node to the scene");
  scalarNode->SetScene(this->GetMRMLScene());
  this->GetMRMLScene()->AddNode(scalarNode);
  scalarNode->SetAndObserveStorageNodeID(storageNode->GetID());
  scalarNode->SetAndObserveDisplayNodeID(displayNode->GetID());
  
  // Read the volume into the node
  vtkDebugMacro("AddArchetypeVolume: about to read data into scalar node " << scalarNode->GetName());
  storageNode->AddObserver(vtkCommand::ProgressEvent, this->LogicCallbackCommand);
  if (this->GetDebug())
    {
    storageNode->DebugOn();
    }
  storageNode->ReadData(scalarNode);
  vtkDebugMacro("AddArchetypeVolume: finished reading data into scalarNode");
  storageNode->RemoveObservers(vtkCommand::ProgressEvent, this->LogicCallbackCommand);
 
  return scalarNode;
}

//--------------------------------------------------------------------------------------
std::string vtkProstateNavLogic::GetFoRStrFromVolumeNodeID(const char* volNodeID)
{  
  vtkMRMLScalarVolumeNode *volNode=vtkMRMLScalarVolumeNode::SafeDownCast(this->GetMRMLScene()->GetNodeByID(volNodeID));  
  if (volNode==NULL)
  {
    vtkErrorMacro("Cannot get FoR, VolumeNode is undefined");
    return std::string("");
  }

  // remaining information to be had from the meta data dictionary     
  const itk::MetaDataDictionary &volDictionary = volNode->GetMetaDataDictionary();
  std::string tagValue; 

  // frame of reference uid
  tagValue.clear();
  itk::ExposeMetaData<std::string>( volDictionary, "0020|0052", tagValue );

  // Need to remove extra '0x00' characters that may be present at the end of the string
  if (!tagValue.empty())
  {
    tagValue=tagValue.c_str();
  }  
  
  return tagValue;
}

void vtkProstateNavLogic::UpdateTargetListFromMRML()
{
  vtkMRMLProstateNavManagerNode* manager=this->GUI->GetProstateNavManagerNode();
  if (manager==NULL)
  {
    vtkErrorMacro("Error updating targetlist from mrml, manager is invalid");
    return;
  }
  vtkMRMLFiducialListNode* fidNode=manager->GetTargetPlanListNode();
  if (fidNode==NULL)
  {
    vtkErrorMacro("Error updating targetlist from mrml, fiducial node is invalid");
    return;
  }

  // True if we modified fiducials at all (typically the label has to be changed)
  bool fidNodeModified=false;
  // If we modified fiducials, then do it in one step, with Start/EndModify. For that we need to remember the previous state.
  int fidNodeModifyOld=0;

  LinkTargetsToFiducials();

  // Remove fiducials in the manager that doesn't have a corresponding FiducialNode
  for (int i=0; i<manager->GetTotalNumberOfTargets(); i++)
  {
    vtkProstateNavTargetDescriptor *t=manager->GetTargetDescriptorAtIndex(i);
    if (fidNode->GetFiducialIndex(t->GetFiducialID())<0)
    {
      // fiducial not found, need to delete it
      manager->RemoveTargetDescriptorAtIndex(i);
      i--; // repeat check on the i-th element
    }
  }

  for (int i=0; i<fidNode->GetNumberOfFiducials(); i++)
  {
    int targetIndex=GetTargetIndexFromFiducialID(fidNode->GetNthFiducialID(i));
    if (targetIndex<0)
    {

      if (!fidNodeModified)
      {
        fidNodeModified=true;
        fidNodeModifyOld=fidNode->StartModify();
      }

      // New fiducial, create associated target
      vtkSmartPointer<vtkProstateNavTargetDescriptor> targetDesc=vtkSmartPointer<vtkProstateNavTargetDescriptor>::New();      
      
      targetDesc->SetFiducialID(fidNode->GetNthFiducialID(i));

      int needleIndex=manager->GetCurrentNeedleIndex();
      NeedleDescriptorStruct needleDesc;
      if (!manager->GetNeedle(needleIndex, needleDesc))
      {
        vtkErrorMacro("Failed to get info for needle "<<needleIndex);
      }
      targetDesc->SetNeedleID(needleDesc.mID); // just to make the passed target and needle info consistent

      needleDesc.mLastTargetIndex++;
      if (!manager->SetNeedle(needleIndex, needleDesc))
      {
        vtkErrorMacro("Failed to set info for needle "<<needleIndex);
      }

      // Haiying: this fixes the issue of target name changes 
      // when you switch among fiducial lists on the Targeting tab.  
      //std::ostrstream strvalue;
      //strvalue << needleDesc.mTargetNamePrefix << needleDesc.mLastTargetIndex << std::ends;        
      //fidNode->SetNthFiducialLabelText(i,strvalue.str());
      //strvalue.rdbuf()->freeze(0);     
      

      std::string FoR = this->GetFoRStrFromVolumeNodeID(manager->GetTargetingVolumeNodeRef());
      targetDesc->SetTargetingVolumeFoR(FoR);

      manager->AddTargetDescriptor(targetDesc);
    }

    targetIndex=GetTargetIndexFromFiducialID(fidNode->GetNthFiducialID(i));
    if (targetIndex>=0)
    {
      // Update fiducial
      vtkProstateNavTargetDescriptor* targetDesc = manager->GetTargetDescriptorAtIndex(targetIndex);
      if (targetDesc!=NULL)
      {
        float *rasLocation=fidNode->GetNthFiducialXYZ(i);
        targetDesc->SetRASLocation(rasLocation[0], rasLocation[1], rasLocation[2]);

        float *rasOrientation=fidNode->GetNthFiducialOrientation(i);
        targetDesc->SetRASOrientation(rasOrientation[0], rasOrientation[1], rasOrientation[2], rasOrientation[3]);

        targetDesc->SetName(fidNode->GetNthFiducialLabelText(i));

        // :TODO: update needle,  etc. parameters ?

      }
      else
      {
        vtkErrorMacro("Invalid target descriptor");
      }
    }
    else
    {
      vtkErrorMacro("Invalid Fiducial ID");
    }
  }

  if (fidNodeModified)
  {
    fidNode->EndModify(fidNodeModifyOld);
    // StartModify/EndModify discarded vtkMRMLFiducialListNode::FiducialModifiedEvent-s, so we have to resubmit them now
    fidNode->InvokeEvent(vtkMRMLFiducialListNode::FiducialModifiedEvent, NULL);
  }
}

//----------------------------------------------------------------------------
void vtkProstateNavLogic::LinkTargetsToFiducials()
{
  vtkMRMLProstateNavManagerNode* manager=this->GUI->GetProstateNavManagerNode();
  if (manager==NULL)
  {
    vtkErrorMacro("Error in LinkTargetsToFiducials, manager is invalid");
    return;
  }
  vtkMRMLFiducialListNode* fidNode=manager->GetTargetPlanListNode();
  if (fidNode==NULL)
  {
    vtkErrorMacro("Error in LinkTargetsToFiducials, fiducial node is invalid");
    return;
  }

  // if all the targets have empty FiducialID reference it means that the targets are not linked
  // to the fiducials yet
  for (int i=0; i<manager->GetTotalNumberOfTargets(); i++)
  {
    vtkProstateNavTargetDescriptor *t=manager->GetTargetDescriptorAtIndex(i);
    if (!t->GetFiducialID().empty())
    {
      // a non-empty FiducialID is found, it means that there is already existing linking
      return;
    }
  }

  const float rasTolerance=0.1;
  for (int targetInd=0; targetInd<manager->GetTotalNumberOfTargets(); targetInd++)
  {
    vtkProstateNavTargetDescriptor *t=manager->GetTargetDescriptorAtIndex(targetInd);
    float targetXYZ[3]={0,0,0};
    targetXYZ[0]=t->GetRASLocation()[0];
    targetXYZ[1]=t->GetRASLocation()[1];
    targetXYZ[2]=t->GetRASLocation()[2];
    for (int fidIndex=0; fidNode->GetNumberOfFiducials(); fidIndex++)
    {
      float* fidXYZ=fidNode->GetNthFiducialXYZ(fidIndex);
      if ( (fabs(targetXYZ[0]-fidXYZ[0])<rasTolerance)
        && (fabs(targetXYZ[1]-fidXYZ[1])<rasTolerance)
        && (fabs(targetXYZ[2]-fidXYZ[2])<rasTolerance)
        && (t->GetName().compare(fidNode->GetNthFiducialLabelText(fidIndex))==0) )
      {
        // matching fiducial found
        t->SetFiducialID(fidNode->GetNthFiducialID(fidIndex));
        break;
      }
    }
  }
}

//----------------------------------------------------------------------------
int vtkProstateNavLogic::GetTargetIndexFromFiducialID(const char* fiducialID)
{
  if (fiducialID==NULL)
  {
    vtkWarningMacro("Fiducial ID is invalid");
    return -1;
  }
  vtkMRMLProstateNavManagerNode* manager=this->GUI->GetProstateNavManagerNode();
  if (manager==NULL)
  {
    vtkErrorMacro("Manager is invalid");
    return -1;
  }
  for (int i=0; i<manager->GetTotalNumberOfTargets(); i++)
  {
    vtkProstateNavTargetDescriptor *t=manager->GetTargetDescriptorAtIndex(i);
    if (t->GetFiducialID().compare(fiducialID)==0)
    {
      // found the target corresponding to the fiducialID
      return i;
    }
  }
  return -1;
}

//----------------------------------------------------------------------------
int vtkProstateNavLogic::SetMouseInteractionMode(int mode)
{  
  if (GetApplicationLogic()==NULL)
  {
   vtkErrorMacro("Application logic is invalid");
    return 0;
  }
  if (GetApplicationLogic()->GetMRMLScene()==NULL)
  {
    vtkErrorMacro("Scene is invalid");
    return 0;
  }
  vtkMRMLInteractionNode *interactionNode = vtkMRMLInteractionNode::SafeDownCast(GetApplicationLogic()->GetMRMLScene()->GetNthNodeByClass(0, "vtkMRMLInteractionNode"));
  if (interactionNode==NULL)
  {
    vtkErrorMacro("Interaction node is invalid");
    return 0;
  }
  
  if (this->GetGUI()==NULL)
  {
    vtkErrorMacro("GUI is invalid");
    return 0;
  }  
  vtkSlicerApplication* app=vtkSlicerApplication::SafeDownCast(this->GetGUI()->GetApplication());
  if (app==NULL)
  {
    vtkErrorMacro("Application is invalid");
    return 0;
  }
  vtkSlicerApplicationGUI* appGUI = app->GetApplicationGUI();
  if (appGUI==NULL)
  {
    vtkErrorMacro("Application GUI is invalid");
    return 0;
  }
  vtkSlicerToolbarGUI *tGUI = appGUI->GetApplicationToolbar();
  if (tGUI==NULL)
  {
    vtkErrorMacro("Application toolbar GUI is invalid");
    return 0;
  }

  // Set logic state
  interactionNode->SetCurrentInteractionMode(mode); 

  // Set pick/place state to persistent (stay in the staet after picking/placing a fiducial)
  if (mode==vtkMRMLInteractionNode::Place)
  {
    interactionNode->SetPlaceModePersistence(1);
  }
  else if (mode==vtkMRMLInteractionNode::PickManipulate)
  {
    interactionNode->SetPickModePersistence(1);
  }
  
  return 1;
}

//----------------------------------------------------------------------------
int vtkProstateNavLogic::SetCurrentFiducialList(vtkMRMLFiducialListNode* fidNode)
{
  if (fidNode==NULL)
  {
    vtkErrorMacro("Fiducial node is invalid");
    return 0;
  }

  if (this->GetGUI()==NULL)
  {
    vtkErrorMacro("GUI is invalid");
    return 0;
  }  
  vtkSlicerApplication* app=vtkSlicerApplication::SafeDownCast(this->GetGUI()->GetApplication());
  if (app==NULL)
  {
    vtkErrorMacro("Application is invalid");
    return 0;
  }

  vtkSlicerFiducialsGUI* fidGUI = vtkSlicerFiducialsGUI::SafeDownCast ( app->GetModuleGUIByName ("Fiducials"));
  if (fidGUI==NULL)
  {
    vtkErrorMacro("Fiducial GUI is invalid");
    return 0;
  }
  
  // Activate target fiducials in the Fiducial GUI
  fidGUI->Enter();
  fidGUI->SetFiducialListNodeID(fidNode->GetID());
  
  return 1;
}

vtkMRMLRobotNode* vtkProstateNavLogic::GetRobotNode()
{
  if (this->GUI==NULL)
  {
    return NULL;
  }
  if (this->GUI->GetProstateNavManagerNode()==NULL)
  {
    return NULL;
  }
  return this->GUI->GetProstateNavManagerNode()->GetRobotNode();
  {
    return NULL;
  }
}

//----------------------------------------------------------------------------
int vtkProstateNavLogic::ShowWorkspaceModel(bool show)
{
  vtkMRMLRobotNode* robot=GetRobotNode();
  if (robot==NULL)
  {
    vtkWarningMacro("Cannot show workspace model, robot is invalid");
    return 0; // failed
  }
  vtkMRMLModelNode*   modelNode = vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(robot->GetWorkspaceObjectModelId()));
  if (modelNode==NULL)
  {
    vtkWarningMacro("Cannot show workspace model, workspace model node is invalid");
    return 0; // failed
  }
  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
  if (displayNode==NULL)
  {
    vtkWarningMacro("Cannot show workspace model, displayNode is invalid");
    return 0; // failed
  }
  displayNode->SetVisibility(show);
  displayNode->SetSliceIntersectionVisibility(show);
  modelNode->Modified();
  this->MRMLScene->Modified();
  return 1;
} 

//----------------------------------------------------------------------------
bool vtkProstateNavLogic::IsWorkspaceModelShown()
{
  vtkMRMLRobotNode* robot=GetRobotNode();
  if (robot==NULL)
  {
    return false;
  }
  vtkMRMLModelNode*   modelNode = vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(robot->GetWorkspaceObjectModelId()));
  if (modelNode==NULL)
  {
    return 0;
  }
  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
  if (displayNode==NULL)
  {
    return 0;
  }
  return displayNode->GetVisibility();
}

//----------------------------------------------------------------------------
int vtkProstateNavLogic::ShowRobotModel(bool show)
{
  vtkMRMLRobotNode* robot=GetRobotNode();
  if (robot==NULL)
  {
    vtkWarningMacro("Cannot show robot model, robot is invalid");
    return 0; // failed
  }
  vtkMRMLModelNode*   modelNode = vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(robot->GetRobotModelId()));
  if (modelNode==NULL)
  {
    vtkWarningMacro("Cannot show robot model, workspace model node is invalid");
    return 0; // failed
  }
  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
  if (displayNode==NULL)
  {
    vtkWarningMacro("Cannot show robot model, displayNode is invalid");
    return 0; // failed
  }
  displayNode->SetVisibility(show);
  displayNode->SetSliceIntersectionVisibility(show);
  modelNode->Modified();
  this->MRMLScene->Modified();
  return 1;
} 

//----------------------------------------------------------------------------
bool vtkProstateNavLogic::IsRobotModelShown()
{
  vtkMRMLRobotNode* robot=GetRobotNode();
  if (robot==NULL)
  {
    return false;
  }
  vtkMRMLModelNode*   modelNode = vtkMRMLModelNode::SafeDownCast(this->MRMLScene->GetNodeByID(robot->GetRobotModelId()));
  if (modelNode==NULL)
  {
    return 0;
  }
  vtkMRMLDisplayNode* displayNode = modelNode->GetDisplayNode();
  if (displayNode==NULL)
  {
    return 0;
  }
  return displayNode->GetVisibility();
}
