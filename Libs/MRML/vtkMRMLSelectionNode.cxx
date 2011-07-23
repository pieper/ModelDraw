/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women\"s Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLSelectionNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkMRMLScene.h"
#include "vtkMRMLSelectionNode.h"

#include "vtkMatrix4x4.h"

//------------------------------------------------------------------------------
vtkMRMLSelectionNode* vtkMRMLSelectionNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLSelectionNode");
  if(ret)
    {
    return (vtkMRMLSelectionNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLSelectionNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLSelectionNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLSelectionNode");
  if(ret)
    {
    return (vtkMRMLSelectionNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLSelectionNode;
}

//----------------------------------------------------------------------------
vtkMRMLSelectionNode::vtkMRMLSelectionNode()
{
  this->SetSingletonTag("vtkMRMLSelectionNode");

  this->ActiveVolumeID = NULL;
  this->SecondaryVolumeID = NULL;
  this->ActiveLabelVolumeID = NULL;
  this->ActiveFiducialListID = NULL;
  this->ActiveROIListID  =NULL;
  this->ActiveCameraID = NULL;
  this->ActiveViewID = NULL;
  this->ActiveLayoutID = NULL;
}

//----------------------------------------------------------------------------
vtkMRMLSelectionNode::~vtkMRMLSelectionNode()
{
  if (this->ActiveVolumeID)
    {
    delete [] this->ActiveVolumeID;
    this->ActiveVolumeID = NULL;
    }
  if (this->SecondaryVolumeID)
    {
    delete [] this->SecondaryVolumeID;
    this->SecondaryVolumeID = NULL;
    }
  if (this->ActiveLabelVolumeID)
    {
    delete [] this->ActiveLabelVolumeID;
    this->ActiveLabelVolumeID = NULL;
    }
  if (this->ActiveFiducialListID)
    {
    delete [] this->ActiveFiducialListID;
    this->ActiveFiducialListID = NULL;
    }
  if (this->ActiveROIListID)
    {
    delete [] this->ActiveROIListID;
    this->ActiveROIListID = NULL;
    }
  if ( this->ActiveCameraID )
    {
    delete [] this->ActiveCameraID;
    this->ActiveCameraID = NULL;
    }
  if ( this->ActiveViewID)
    {
    delete []  this->ActiveViewID;
    this->ActiveViewID = NULL;
    }
  if ( this->ActiveLayoutID)
    {
    delete [] this->ActiveLayoutID;
    this->ActiveLayoutID = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkMRMLSelectionNode::WriteXML(ostream& of, int nIndent)
{
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  of << indent << " activeVolumeID=\"" << (this->ActiveVolumeID ? this->ActiveVolumeID : "NULL") << "\"";
  of << indent << " secondaryVolumeID=\"" << (this->SecondaryVolumeID ? this->SecondaryVolumeID : "NULL") << "\"";
  of << indent << " activeLabelVolumeID=\"" << (this->ActiveLabelVolumeID ? this->ActiveLabelVolumeID : "NULL") << "\"";
  of << indent << " activeFiducialListID=\"" << (this->ActiveFiducialListID ? this->ActiveFiducialListID : "NULL") << "\"";
  of << indent << " activeROIListID=\"" << (this->ActiveROIListID ? this->ActiveROIListID : "NULL") << "\"";
  of << indent << " activeCameraID=\"" << (this->ActiveCameraID ? this->ActiveCameraID : "NULL") << "\"";
  of << indent << " activeViewID=\"" << (this->ActiveViewID ? this->ActiveViewID : "NULL") << "\"";
  of << indent << " activeLayoutID=\"" << (this->ActiveLayoutID ? this->ActiveLayoutID : "NULL") << "\"";
}

//----------------------------------------------------------------------------
void vtkMRMLSelectionNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
  if (this->ActiveVolumeID && !strcmp(oldID, this->ActiveVolumeID))
    {
    this->SetActiveVolumeID(newID);
    }
  if (this->SecondaryVolumeID && !strcmp(oldID, this->SecondaryVolumeID))
    {
    this->SetSecondaryVolumeID(newID);
    }
  if (this->ActiveLabelVolumeID && !strcmp(oldID, this->ActiveLabelVolumeID))
    {
    this->SetActiveLabelVolumeID(newID);
    }
  if (this->ActiveFiducialListID && !strcmp(oldID, this->ActiveFiducialListID))
    {
    this->SetActiveFiducialListID(newID);
    }
  if ( this->ActiveCameraID && !strcmp (oldID, this->ActiveCameraID ))
    {
    this->SetActiveCameraID (newID);
    }
  if ( this->ActiveViewID && !strcmp ( oldID, this->ActiveViewID ))
    {
    this->SetActiveViewID (newID );
    }
  if ( this->ActiveLayoutID && !strcmp ( oldID, this->ActiveLayoutID ))
    {
    this->SetActiveLayoutID (newID );
    }

}

//-----------------------------------------------------------
void vtkMRMLSelectionNode::UpdateReferences()
{
   Superclass::UpdateReferences();

  if (this->ActiveVolumeID != NULL && this->Scene->GetNodeByID(this->ActiveVolumeID) == NULL)
    {
    this->SetActiveVolumeID(NULL);
    }
  if (this->SecondaryVolumeID != NULL && this->Scene->GetNodeByID(this->SecondaryVolumeID) == NULL)
    {
    this->SetSecondaryVolumeID(NULL);
    }
  if (this->ActiveLabelVolumeID != NULL && this->Scene->GetNodeByID(this->ActiveLabelVolumeID) == NULL)
    {
    this->SetActiveLabelVolumeID(NULL);
    }
  if (this->ActiveFiducialListID != NULL && this->Scene->GetNodeByID(this->ActiveFiducialListID) == NULL)
    {
    this->SetActiveFiducialListID(NULL);
    }
  if (this->ActiveViewID != NULL && this->Scene->GetNodeByID(this->ActiveViewID) == NULL)
    {
    this->SetActiveViewID(NULL);
    }
  if (this->ActiveLayoutID != NULL && this->Scene->GetNodeByID(this->ActiveLayoutID) == NULL)
    {
    this->SetActiveLayoutID(NULL);
    }
  if (this->ActiveCameraID != NULL && this->Scene->GetNodeByID(this->ActiveCameraID) == NULL)
    {
    this->SetActiveCameraID(NULL);
    }

}
//----------------------------------------------------------------------------
void vtkMRMLSelectionNode::ReadXMLAttributes(const char** atts)
{
  int disabledModify = this->StartModify();

  Superclass::ReadXMLAttributes(atts);

  const char* attName;
  const char* attValue;
  while (*atts != NULL) 
    {
    attName = *(atts++);
    attValue = *(atts++);
    if (!strcmp(attName, "activeVolumeID")) 
      {
      this->SetActiveVolumeID(attValue);
      //this->Scene->AddReferencedNodeID(this->ActiveVolumeID, this);
      }
    if (!strcmp(attName, "secondaryVolumeID")) 
      {
      this->SetSecondaryVolumeID(attValue);
      //this->Scene->AddReferencedNodeID(this->ActiveVolumeID, this);
      }
    if (!strcmp(attName, "activeLabelVolumeID")) 
      {
      this->SetActiveLabelVolumeID(attValue);
      //this->Scene->AddReferencedNodeID(this->ActiveLabelVolumeID, this);
      }
    if (!strcmp(attName, "activeFiducialListID")) 
      {
      this->SetActiveFiducialListID(attValue);
      //this->Scene->AddReferencedNodeID(this->ActiveFiducialListID, this);
      }
    if (!strcmp (attName, "activeCameraID"))
      {
      this->SetActiveCameraID (attValue );
      //this->Scene->AddReferencedNodeID (this->ActiveCameraID, this);
      }
    if (!strcmp (attName, "activeViewID"))
      {
      this->SetActiveViewID (attValue);
      //this->Scene->AddReferencedNodeID ( this->ActiveViewID, this);
      }
    if (!strcmp (attName, "activeLayoutID"))
      {
      this->SetActiveLayoutID (attValue);
      //this->Scene->AddReferencedNodeID ( this->ActiveLayoutID, this);
      }

    }

  this->EndModify(disabledModify);
}

//----------------------------------------------------------------------------
// Copy the node\"s attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, SliceID
void vtkMRMLSelectionNode::Copy(vtkMRMLNode *anode)
{
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLSelectionNode *node = vtkMRMLSelectionNode::SafeDownCast(anode);

  this->SetActiveVolumeID(node->GetActiveVolumeID());
  this->SetSecondaryVolumeID(node->GetActiveVolumeID());
  this->SetActiveLabelVolumeID(node->GetActiveLabelVolumeID());
  this->SetActiveFiducialListID(node->GetActiveFiducialListID());
  this->SetActiveCameraID (node->GetActiveCameraID());
  this->SetActiveViewID (node->GetActiveViewID() );
  this->SetActiveLayoutID (node->GetActiveLayoutID() );

  this->EndModify(disabledModify);
  
  }

//----------------------------------------------------------------------------
void vtkMRMLSelectionNode::PrintSelf(ostream& os, vtkIndent indent)
{
  Superclass::PrintSelf(os,indent);

  os << "ActiveVolumeID: " << ( (this->ActiveVolumeID) ? this->ActiveVolumeID : "None" ) << "\n";
  os << "SecondaryVolumeID: " << ( (this->SecondaryVolumeID) ? this->SecondaryVolumeID : "None" ) << "\n";
  os << "ActiveLabelVolumeID: " << ( (this->ActiveLabelVolumeID) ? this->ActiveLabelVolumeID : "None" ) << "\n";
  os << "ActiveFiducialListID: " << ( (this->ActiveFiducialListID) ? this->ActiveFiducialListID : "None" ) << "\n";
  os << "ActiveCameraID: " << ( (this->ActiveCameraID) ? this->ActiveCameraID : "None" ) << "\n";
  os << "ActiveViewID: " << ( (this->ActiveViewID) ? this->ActiveViewID : "None" ) << "\n";
  os << "ActiveLayoutID: " << ( (this->ActiveLayoutID) ? this->ActiveLayoutID : "None" ) << "\n";

}


// End
