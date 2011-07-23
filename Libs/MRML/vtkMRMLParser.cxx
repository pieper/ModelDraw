/*=auto=========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLParser.cxx,v $
Date:      $Date: 2006/03/11 19:51:14 $
Version:   $Revision: 1.8 $

=========================================================================auto=*/
#include "vtkObjectFactory.h"
#include "vtkMRMLParser.h"
#include <sstream>
#include <string>
#include <iostream>

//------------------------------------------------------------------------------
vtkMRMLParser* vtkMRMLParser::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLParser");
  if(ret) 
    {
    return (vtkMRMLParser*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLParser;
}


//------------------------------------------------------------------------------
void vtkMRMLParser::StartElement(const char* tagName, const char** atts)
{
  if (!strcmp(tagName, "MRML")) 
    {
    //--- BEGIN test of user tags
    //--- pull out any tags describing the scene and fill the scene's tagtable.
    const char* attName;
    const char* attValue;
    while (*atts != NULL) 
      {
      attName = *(atts++);
      attValue = *(atts++);  
      if (!strcmp(attName, "version")) 
        {
        this->GetMRMLScene()->SetLastLoadedVersion(attValue);
        }
      else if (!strcmp(attName, "userTags")) 
        {
        if ( this->MRMLScene->GetUserTagTable() == NULL )
          {
          //--- null table, no tags are read.
          return;
          }
        std::stringstream ss(attValue);
        std::string kwd = "";
        std::string val = "";
        std::string::size_type i;
        while (!ss.eof())
          {
          std::string tags;
          ss >> tags;
          //--- now pull apart individual tags
          if ( tags.c_str() != NULL )
            {
            i = tags.find("=");
            if ( i != std::string::npos)
              {
              kwd = tags.substr(0, i);
              val = tags.substr(i+1, std::string::npos );
              if ( kwd.c_str() != NULL && val.c_str() != NULL )
                {
                this->MRMLScene->GetUserTagTable()->AddOrUpdateTag ( kwd.c_str(), val.c_str(), 0 );
                }
              }
            }        
          }
        } //--- END test of user tags. 
      } // while
    return;
    } // MRML

  const char* className = this->MRMLScene->GetClassNameByTag(tagName);

  if (className == NULL) 
    {
    return;
    }

  vtkMRMLNode* node = this->MRMLScene->CreateNodeByClass( className );

  node->SetScene(this->MRMLScene);
  node->ReadXMLAttributes(atts);

  // ID will be set by AddNodeNoNotify
  /*
  if (node->GetID() == NULL) 
    {
    node->SetID(this->MRMLScene->GetUniqueIDByClass(className));
    }
  */
  if (!this->NodeStack.empty()) 
    {
    vtkMRMLNode* parentNode = this->NodeStack.top();
    parentNode->ProcessChildNode(node);

    node->ProcessParentNode(parentNode);
    }

  this->NodeStack.push(node);

  if (this->NodeCollection)
    {
    if (node->GetAddToScene()) 
      {
      this->NodeCollection->vtkCollection::AddItem((vtkObject *)node);
      }
    }
  else
    {
    this->MRMLScene->AddNodeNoNotify(node);
    }
  node->Delete();
}

//-----------------------------------------------------------------------------

void vtkMRMLParser::EndElement (const char *name)
{
  if ( !strcmp(name, "MRML") || this->NodeStack.empty() ) 
    {
    return;
    }

  const char* className = this->MRMLScene->GetClassNameByTag(name);
  if (className == NULL) 
    {
    return;
    }

  this->NodeStack.pop();
}
