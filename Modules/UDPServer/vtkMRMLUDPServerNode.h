/*=auto=========================================================================

  Portions (c) Copyright 2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $RCSfile: vtkMRMLUDPServerNode.h,v $
  Date:      $Date: 2009/10/15 17:12:29 $
  Version:   $Revision: 1.3 $

=========================================================================auto=*/

#ifndef VTKMRMLUDPSERVERNODE_H_
#define VTKMRMLUDPSERVERNODE_H_

#include "vtkUDPServerWin32Header.h"
#include "vtkMRML.h"
#include "vtkMRMLModelNode.h"
#include "vtkMRMLStorageNode.h"
#include "vtkObject.h"
#include "vtkMatrix4x4.h"

class VTK_UDPServer_EXPORT vtkMRMLUDPServerNode : public vtkMRMLNode
{

public:
  //----------------------------------------------------------------
  // Standard methods for MRML nodes
  //----------------------------------------------------------------
  static vtkMRMLUDPServerNode *New();
  vtkTypeMacro(vtkMRMLUDPServerNode, vtkMRMLNode);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Create instance of a HybridNavTool node.
  virtual vtkMRMLNode* CreateNodeInstance();

  // Set node attributes from name/value pairs
  virtual void ReadXMLAttributes( const char** atts);

  // Write this node's information to a MRML file in XML format.
  virtual void WriteXML(ostream& of, int indent);

  // Copy the node's attributes to this object
  virtual void Copy(vtkMRMLNode *node);

  // Get unique node XML tag name (like Volume, Model)
  virtual const char* GetNodeTagName() {return "UDPServerNode";};
  
  //Disactivate the possibility to apply non-linear transforms
  virtual bool CanApplyNonLinearTransforms() {return 0;};

protected:
  //----------------------------------------------------------------
  // Constructor and destructor
  //----------------------------------------------------------------
  vtkMRMLUDPServerNode();
  ~vtkMRMLUDPServerNode();
  vtkMRMLUDPServerNode(const vtkMRMLUDPServerNode&);
  void operator=(const vtkMRMLUDPServerNode&);

public:
  //----------------------------------------------------------------
  // Gamma Probe Variables
  //----------------------------------------------------------------

  // Smoothed Counts, beta counts and gamma counts
  vtkSetMacro(SmoothedCounts, int);
  vtkSetMacro(BetaCounts, int);
  vtkSetMacro(GammaCounts, int);
  vtkGetMacro(SmoothedCounts, int);
  vtkGetMacro(BetaCounts, int);
  vtkGetMacro(GammaCounts, int);
  
  //Date and Time
  //BTX
  void SetDate(std::string date){ this->strDate = date; };
  void SetTime(std::string time){ this->strDate = time; };
  std::string GetDate(){ return this->strDate; };
  std::string GetTime(){ return this->strTime; };
  //ETX

private:
  //BTX
  int SmoothedCounts;
  int BetaCounts;
  int GammaCounts;
  std::string strDate;
  std::string strTime;
  //ETX

};

#endif /* VTKMRMLHYBRIDNAVNODETOOL_H_ */
