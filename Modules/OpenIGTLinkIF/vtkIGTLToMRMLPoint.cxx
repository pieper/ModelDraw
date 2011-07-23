/*==========================================================================

  Portions (c) Copyright 2008-2009 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/trunk/Modules/OpenIGTLinkIF/vtkIGTLToMRMLPoint.cxx $
  Date:      $Date: 2009-10-05 17:37:20 -0400 (Mon, 05 Oct 2009) $
  Version:   $Revision: 10577 $

==========================================================================*/

#include <vtksys/SystemTools.hxx>

#include "vtkObjectFactory.h"
#include "vtkIGTLToMRMLPoint.h"

#include "vtkSlicerColorLogic.h"

#include "vtkImageData.h"
#include "vtkMRMLScalarVolumeNode.h"
#include "igtlPointMessage.h"

#include "vtkMRMLFiducialListNode.h"
//#include "vtkMRMLIGTLPointBundleNode.h"
#include "vtkMRMLIGTLQueryNode.h"

vtkStandardNewMacro(vtkIGTLToMRMLPoint);
vtkCxxRevisionMacro(vtkIGTLToMRMLPoint, "$Revision: 10577 $");


//---------------------------------------------------------------------------
vtkIGTLToMRMLPoint::vtkIGTLToMRMLPoint()
{
}


//---------------------------------------------------------------------------
vtkIGTLToMRMLPoint::~vtkIGTLToMRMLPoint()
{
}


//---------------------------------------------------------------------------
void vtkIGTLToMRMLPoint::PrintSelf(ostream& os, vtkIndent indent)
{
}


//---------------------------------------------------------------------------
vtkMRMLNode* vtkIGTLToMRMLPoint::CreateNewNode(vtkMRMLScene* scene, const char* name)
{

  //vtkMRMLIGTLPointBundleNode *node = vtkMRMLIGTLPointBundleNode::New();
  //node->SetName(name);
  //node->SetDescription("Received by OpenIGTLink");

  //scene->AddNode(node);

  //return node;
  return NULL;
}


//---------------------------------------------------------------------------
vtkIntArray* vtkIGTLToMRMLPoint::GetNodeEvents()
{
  vtkIntArray* events;

  events = vtkIntArray::New();
  //events->InsertNextValue(vtkMRMLPointNode::ModifiedEvent); 

  return events;
}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPoint::IGTLToMRML(igtl::MessageBase::Pointer buffer, vtkMRMLNode* node)
{

  if (strcmp(node->GetNodeTagName(), "IGTLPointSplitter") != 0)
    {
    //std::cerr << "Invalid node!!!!" << std::endl;
    return 0;
    }

  //------------------------------------------------------------
  // Allocate Point Message Class

  // Create a message buffer to receive transform data
  igtl::PointMessage::Pointer pointMsg;
  pointMsg = igtl::PointMessage::New();
  pointMsg->Copy(buffer);

  // Deserialize the transform data
  // If you want to skip CRC check, call Unpack() without argument.
  int c = pointMsg->Unpack(1);

  if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
    {
    vtkMRMLFiducialListNode* flnode = vtkMRMLFiducialListNode::SafeDownCast(node);
    if (flnode)
      {
      int modid = flnode->StartModify();
      int nElements = pointMsg->GetNumberOfPointElement();
      for (int i = 0; i < nElements; i ++)
        {
        igtl::PointElement::Pointer pointElement;
        pointMsg->GetPointElement(i, pointElement);
        
        igtlUint8 rgba[4];
        pointElement->GetRGBA(rgba);
        
        igtlFloat32 pos[3];
        pointElement->GetPosition(pos);
        
        std::cerr << "========== Element #" << i << " ==========" << std::endl;
        std::cerr << " Name      : " << pointElement->GetName() << std::endl;
        std::cerr << " GroupName : " << pointElement->GetGroupName() << std::endl;
        std::cerr << " RGBA      : ( " << (int)rgba[0] << ", " << (int)rgba[1] << ", " << (int)rgba[2] << ", " << (int)rgba[3] << " )" << std::endl;
        std::cerr << " Position  : ( " << std::fixed << pos[0] << ", " << pos[1] << ", " << pos[2] << " )" << std::endl;
        std::cerr << " Radius    : " << std::fixed << pointElement->GetRadius() << std::endl;
        std::cerr << " Owner     : " << pointElement->GetOwner() << std::endl;
        std::cerr << "================================" << std::endl;

        int index = flnode->AddFiducialWithLabelXYZSelectedVisibility(pointElement->GetName(),
                                                                       pos[0], pos[1], pos[2], 0, 1);

        }
      flnode->EndModify(modid);
      }
    flnode->Modified();
    return 1;
    }

  return 1;

}


//---------------------------------------------------------------------------
int vtkIGTLToMRMLPoint::MRMLToIGTL(unsigned long event, vtkMRMLNode* mrmlNode, int* size, void** igtlMsg)
{
  if (!mrmlNode)
    {
    return 0;
    }

  if ((event == vtkMRMLFiducialListNode::FiducialModifiedEvent ||
       event == vtkMRMLFiducialListNode::FiducialIndexModifiedEvent ||
       event == vtkMRMLFiducialListNode::DisplayModifiedEvent)
      && strcmp(mrmlNode->GetNodeTagName(), "FiducialList") == 0)
    {
    
    }
  // If mrmlNode is query node
  else if (strcmp(mrmlNode->GetNodeTagName(), "IGTLQuery") == 0 ) // Query Node
    {
    vtkMRMLIGTLQueryNode* qnode = vtkMRMLIGTLQueryNode::SafeDownCast(mrmlNode);
    if (qnode)
      {
      if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_GET)
        {
        /*
        //igtl::TransformMessage::Pointer OutTransformMsg;
        if (this->GetImageMetaMessage.IsNull())
          {
          this->GetImageMetaMessage = igtl::GetTransformDataMessage::New();
          }
        this->GetImageMetaMessage->SetDeviceName(mrmlNode->GetName());
        this->GetImageMetaMessage->Pack();
        *size = this->GetImageMetaMessage->GetPackSize();
        *igtlMsg = this->GetImageMetaMessage->GetPackPointer();
        */
        *size = 0;
        return 0;
        }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_START)
        {
        // N / A
        return 0;
        }
      else if (qnode->GetQueryType() == vtkMRMLIGTLQueryNode::TYPE_STOP)
        {
        // N/  A
        return 0;
        }
      return 0;
      }
    else
      {
      return 0;
      }
    }

  // If mrmlNode is data node
  /*
  if (event == vtkMRMLVolumeNode::ImageDataModifiedEvent)
    {
    return 1;
    }
  else
    {
    return 0;
    }
  */
  return 0;
}



