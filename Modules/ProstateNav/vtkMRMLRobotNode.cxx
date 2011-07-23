/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLGradientAnisotropicDiffusionFilterNode.cxx,v $
Date:      $Date: 2006/03/17 15:10:10 $
Version:   $Revision: 1.2 $

=========================================================================auto=*/

#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"

#include "vtkMRMLRobotNode.h"
#include "vtkMRMLScene.h"
#include "vtkLinearTransform.h"
#include "vtkSmartPointer.h"

//------------------------------------------------------------------------------
vtkMRMLRobotNode* vtkMRMLRobotNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLRobotNode"); if(ret)
    {
      return (vtkMRMLRobotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLRobotNode;
}

//----------------------------------------------------------------------------
vtkMRMLNode* vtkMRMLRobotNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLRobotNode");
  if(ret)
    {
      return (vtkMRMLRobotNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLRobotNode;
}

//----------------------------------------------------------------------------
vtkMRMLRobotNode::vtkMRMLRobotNode()
{
  this->TargetTransformNodeID = NULL;
  this->TargetTransformNode = NULL;

  this->HideFromEditors=0;
}

//----------------------------------------------------------------------------
vtkMRMLRobotNode::~vtkMRMLRobotNode()
{ 
  if (this->TargetTransformNodeID) 
  {
    SetAndObserveTargetTransformNodeID(NULL);
  }
}

//----------------------------------------------------------------------------
void vtkMRMLRobotNode::RemoveChildNodes()
{
  if (this->TargetTransformNodeID!=NULL && this->GetScene()!=NULL)
  {
    this->GetScene()->RemoveNode(this->GetTargetTransformNode());
  }
}

//----------------------------------------------------------------------------
void vtkMRMLRobotNode::WriteXML(ostream& of, int nIndent)
{

  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);

  vtkIndent indent(nIndent);

  if (this->TargetTransformNodeID != NULL) 
    {
    of << indent << " TargetTransformNodeRef=\"" << this->TargetTransformNodeID << "\"";
    }
}


//----------------------------------------------------------------------------
void vtkMRMLRobotNode::ReadXMLAttributes(const char** atts)
{
  Superclass::ReadXMLAttributes(atts);

  // Read all MRML node attributes from two arrays of names and values
  const char* attName;
  const char* attValue;

  while (*atts != NULL)
    {
    attName  = *(atts++);
    attValue = *(atts++);

    if (!strcmp(attName, "TargetTransformNodeRef")) 
      {
      this->SetAndObserveTargetTransformNodeID(NULL);
      this->SetTargetTransformNodeID(attValue);
      }

    }
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
void vtkMRMLRobotNode::Copy(vtkMRMLNode *anode)
{  
  int disabledModify = this->StartModify();

  Superclass::Copy(anode);
  vtkMRMLRobotNode *node = (vtkMRMLRobotNode *) anode;
  SetAndObserveTargetTransformNodeID(NULL); // remove observers
  this->SetTargetTransformNodeID(node->TargetTransformNodeID);

  this->EndModify(disabledModify);
}

//-----------------------------------------------------------
void vtkMRMLRobotNode::UpdateReferences()
{
  Superclass::UpdateReferences();

  if (this->TargetTransformNodeID != NULL && this->Scene->GetNodeByID(this->TargetTransformNodeID) == NULL)
    {
    this->SetAndObserveTargetTransformNodeID(NULL);
    }
}

//----------------------------------------------------------------------------
void vtkMRMLRobotNode::UpdateReferenceID(const char *oldID, const char *newID)
{
  Superclass::UpdateReferenceID(oldID, newID);
  if (this->TargetTransformNodeID && !strcmp(oldID, this->TargetTransformNodeID))
    {
    this->SetAndObserveTargetTransformNodeID(newID);
    }
}

//-----------------------------------------------------------
void vtkMRMLRobotNode::UpdateScene(vtkMRMLScene *scene)
{
   Superclass::UpdateScene(scene);
   this->SetAndObserveTargetTransformNodeID(this->TargetTransformNodeID);
}

//-----------------------------------------------------------
void vtkMRMLRobotNode::ProcessMRMLEvents( vtkObject *caller, unsigned long event, void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
  return;
}


//----------------------------------------------------------------------------
void vtkMRMLRobotNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);

  os << indent << "TargetTransformNodeID: " <<
    (this->TargetTransformNodeID ? this->TargetTransformNodeID : "(none)") << "\n";
}

//----------------------------------------------------------------------------
void vtkMRMLRobotNode::SetAndObserveTargetTransformNodeID(const char *transformNodeID)
{
  vtkSetAndObserveMRMLObjectMacro(this->TargetTransformNode, NULL);

  this->SetTargetTransformNodeID(transformNodeID);

  vtkMRMLTransformNode *tnode = this->GetTargetTransformNode();

  vtkIntArray *events = vtkIntArray::New();
  events->InsertNextValue(vtkMRMLTransformableNode::TransformModifiedEvent);
  vtkSetAndObserveMRMLObjectEventsMacro(this->TargetTransformNode, tnode, events);
  events->Delete();
}

//----------------------------------------------------------------------------
vtkMRMLTransformNode* vtkMRMLRobotNode::GetTargetTransformNode()
{
  vtkMRMLTransformNode* node = NULL;
  if (this->GetScene() && this->TargetTransformNodeID != NULL )
    {
    vtkMRMLNode* snode = this->GetScene()->GetNodeByID(this->TargetTransformNodeID);
    node = vtkMRMLTransformNode::SafeDownCast(snode);
    }
  return node;
}

//----------------------------------------------------------------------------
int vtkMRMLRobotNode::Init(vtkSlicerApplication* app, const char* moduleShareDir)
{
  if (moduleShareDir!=NULL)
  {
    this->ModuleShareDirectory=moduleShareDir;
  }
  else
  {
    this->ModuleShareDirectory.clear();
  }
  if (GetTargetTransformNode()==NULL)
  {
    // Target node
    vtkSmartPointer<vtkMRMLLinearTransformNode> tnode = vtkSmartPointer<vtkMRMLLinearTransformNode>::New();
    tnode->SetName("ProstateNavRobotTarget");

    vtkSmartPointer<vtkMatrix4x4> transform = vtkSmartPointer<vtkMatrix4x4>::New();
    transform->Identity();
    tnode->ApplyTransform(transform);
    tnode->SetScene(this->GetScene());
    tnode->SetModifiedSinceRead(true);

    this->GetScene()->AddNode(tnode);
    SetAndObserveTargetTransformNodeID(tnode->GetID());
  }

  return 1;
}

//----------------------------------------------------------------------------
int vtkMRMLRobotNode::GetStatusDescriptorCount()
{
  return this->StatusDescriptors.size();
}

//----------------------------------------------------------------------------
int vtkMRMLRobotNode::GetStatusDescriptor(unsigned int index, std::string &text, STATUS_ID &indicator)
{
  if (index>=this->StatusDescriptors.size())
  {
    return 0;
  }
  text=this->StatusDescriptors[index].text;
  indicator=this->StatusDescriptors[index].indicator;
  return 1;
}
//----------------------------------------------------------------------------
bool vtkMRMLRobotNode::CanApplyNonLinearTransforms() 
{ 
  // Only linear transforms are supported
  return false; 
}
//----------------------------------------------------------------------------
void vtkMRMLRobotNode::ApplyTransform(vtkAbstractTransform* transform) 
{ 
  if (!transform->IsA("vtkLinearTransform"))
    {
    vtkErrorMacro(<<"Only linear transforms can be applied on vtkMRMLRobotNode nodes.");
    }
  vtkLinearTransform *linTransform= vtkLinearTransform::SafeDownCast(transform);
  if (linTransform!=NULL)
  {
    ApplyTransform(linTransform->GetMatrix());
  }
}

//----------------------------------------------------------------------------
void vtkMRMLRobotNode::ApplyTransform(vtkMatrix4x4* transformMatrix)
{ 
  Superclass::ApplyTransform(transformMatrix); 
}

//----------------------------------------------------------------------------
void vtkMRMLRobotNode::SplitTargetInfoText(const std::string targetInfoText, std::string &mainInfo, std::string &additionalInfo)
{   
  std::string info=targetInfoText; // copy the input string, as it will be modified
  mainInfo.clear();
  additionalInfo.clear();

  std::string::size_type separatorPos=targetInfoText.find(GetTargetInfoSectionSeparator());    
  if (separatorPos != std::string::npos )
  {
    // separator found, there will be separate main and additional info section
    mainInfo=info.substr(0, separatorPos);
    info.erase(0, separatorPos+GetTargetInfoSectionSeparator().size());
    additionalInfo=info;
  }
  else
  {
    mainInfo=info;
  }
}
