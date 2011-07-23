/*==========================================================================

Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Date:      $Date: 2006-07-31 22:32:34 -0400 (Mon, 31 Jul 2006) $
Version:   $Revision: 979 $

==========================================================================*/

#include "vtkSlicerApplication.h"

#include "vtkScriptedModuleGUI.h"

// KWWidgets includes
#include "vtkKWApplication.h"
#include "vtkKWTkUtilities.h"

#include "vtkSlicerConfigure.h" /* Slicer3_USE_* */

// VTK includes
#include <vtkObjectFactory.h>

// STL includes
#include <string>
#include <iostream>
#include <sstream>

#ifdef Slicer3_USE_PYTHON
#include "slicerPython.h"
#endif

//------------------------------------------------------------------------------
vtkScriptedModuleGUI* vtkScriptedModuleGUI::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkScriptedModuleGUI");
  if(ret)
    {
      return (vtkScriptedModuleGUI*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkScriptedModuleGUI;
}


//----------------------------------------------------------------------------
vtkScriptedModuleGUI::vtkScriptedModuleGUI()
{
  this->Logic = NULL;
  this->ScriptedModuleNode = NULL;
  this->Language = vtkScriptedModuleGUI::Tcl;
}

//----------------------------------------------------------------------------
vtkScriptedModuleGUI::~vtkScriptedModuleGUI()
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sDestructor %s", 
        this->GetModuleName(), this->GetTclName());
      }

    this->RemoveMRMLNodeObservers();
    this->RemoveLogicObservers();

    this->SetLogic (NULL);
    this->SetScriptedModuleNode (NULL);
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
// NO NEED TO CALL THE DESTRUCTOR EXPLICITLY, THE PYTHON GARBAGE COLLECTOR WILL TAKE CARE OF IT
//    std::stringstream pythonCommand;
//    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].Destructor()\n"; 
//    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
//      {
//      PyErr_Print();
//      }
#endif
    }
}


void vtkScriptedModuleGUI::RemoveMRMLNodeObservers()
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sRemoveMRMLNodeObservers %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].RemoveMRMLNodeObservers()\n"; 
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}


void vtkScriptedModuleGUI::RemoveLogicObservers()
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sRemoveLogicObservers %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].RemoveLogicObservers()\n"; 
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}


//----------------------------------------------------------------------------
void vtkScriptedModuleGUI::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);

  os << indent << "ModuleName: " << (this->ModuleName ? this->ModuleName : "null") << endl;
  os << indent << "Category: " << (this->Category ? this->Category : "null") << endl;
  os << indent << "Logic: " << endl;
  if (this->Logic)
    {
    this->Logic->PrintSelf(os, indent.GetNextIndent());
    }

  os << indent << "ScriptedModuleNode: " << endl;
  if (this->ScriptedModuleNode)
    {
    this->ScriptedModuleNode->PrintSelf(os, indent.GetNextIndent());
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::AddGUIObservers ( ) 
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sAddGUIObservers %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].AddGUIObservers()\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}



//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::RemoveGUIObservers ( )
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sRemoveGUIObservers %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].RemoveGUIObservers()\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::ProcessGUIEvents(vtkObject *caller,
                                            unsigned long event,
                                            void *vtkNotUsed(callData)) 
{
  if ( caller == NULL )
    {
    vtkErrorMacro("ignoring scripted event with null caller.");
    return;
    }

  const char *tclName = vtkKWTkUtilities::GetTclNameFromPointer(
                                            this->GetApplication(), caller);
  if ( tclName == NULL )
    {
    vtkErrorMacro("ignoring scripted event with non-tcl wrapped caller.");
    return;
    }

  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    this->GetApplication()->Script("%sProcessGUIEvents %s %s %ld", 
      this->GetModuleName(), this->GetTclName(), tclName, event);
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].ProcessGUIEvents(Slicer.GetPythonWrapper('" << tclName << "')," << event << ")\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::UpdateMRML ()
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sUpdateMRML %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].UpdateMRML()\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::UpdateGUI ()
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sUpdateGUI %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].UpdateGUI()\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::ProcessMRMLEvents(vtkObject *caller,
                                             unsigned long event,
                                             void *callData) 
{
  vtkDebugMacro("ProcessMRMLEvents()");
  vtkMRMLNode *mrmlNode = vtkMRMLNode::SafeDownCast(caller);
  vtkMRMLScene *mrmlScene = vtkMRMLScene::SafeDownCast(caller);

  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (mrmlNode != NULL)
      {
      vtkDebugMacro("vtkScriptedModuleGUI::ProcessMRMLEvents: calling script " <<  this->GetModuleName() << "ProcessMRMLEvents with event " << event);
      this->GetApplication()->Script("%sProcessMRMLEvents %s %s %ld", 
                                     this->GetModuleName(), this->GetTclName(), mrmlNode->GetID(), event);
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    if (mrmlNode != NULL || mrmlScene != NULL)
      {
      std::stringstream pythonCommand;
      
      if (mrmlNode)
        {
        // event was fired by a MRML node
        pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].ProcessMRMLEvents('" << mrmlNode->GetID() << "'," << event << ")\n";
        }
      else if(mrmlScene)
        {
        // event was fired by the MRML scene
        
        // let's check if we have valid callData
        vtkMRMLNode *mrmlNodeFromCallData = vtkMRMLNode::SafeDownCast((vtkMRMLNode*)callData);
      
        if (mrmlNodeFromCallData)
          {
          // we have valid callData and forward it to the python script
          pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].ProcessMRMLEvents('MRMLScene'," << event << ",'" << mrmlNodeFromCallData->GetID() << "')\n";
          }
        else
          {
          // we don't have a valid calldata, so we just forward the event to the python script
          pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].ProcessMRMLEvents('MRMLScene'," << event << ")\n";
          }
        }
      
      if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
        {
        PyErr_Print();
        }
      }
#endif
    }
  
  // if parameter node has been changed externally, update GUI widgets
  // with new values 
  vtkMRMLScriptedModuleNode* node
    = vtkMRMLScriptedModuleNode::SafeDownCast(caller);
  if (node != NULL && this->GetScriptedModuleNode() == node) 
    {
    this->UpdateGUI();
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::BuildGUI ( ) 
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sBuildGUI %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].BuildGUI()\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::TearDownGUI ( ) 
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sTearDownGUI %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;
    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].TearDownGUI()\n";
    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#endif
    } 
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (const char* methodAndArgs)
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sTearDownGUI %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
    std::stringstream pythonCommand;

    std::vector<std::string> strings;
    std::istringstream f(methodAndArgs);
    std::string s;    
    while (std::getline(f, s, ' ')) {
      strings.push_back(s);
    }

    if (strings.size()==1) 
      {
      // no args, just plain method call
      pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui']." << strings[0] << "()\n";
      }
    else 
      {
  
      std::string argsString;
      for(unsigned int i=1; i<strings.size(); i++)
        {
        argsString += strings[i];
        if (i+1!=strings.size())
          {
          // only append comma if it is not the last element
          argsString += ",";
          }
        }
      
      pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui']." << strings[0] << "("<< argsString <<")\n";
      //std::cout << pythonCommand << std::endl;
      }

    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
      {
      PyErr_Print();
      }
#else
    (void)(methodAndArgs); // To avoid "unused variable warning"
#endif
    } 
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (char* method, char* arg1)
{

  std::string methodAndArgs("");
  methodAndArgs += method;
  methodAndArgs += " ";
  methodAndArgs += arg1;
  this->Invoke(methodAndArgs.c_str());

}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (char* method, char* arg1, char* arg2)
{

  std::string methodAndArgs("");
  methodAndArgs += method;
  methodAndArgs += " ";
  methodAndArgs += arg1;
  methodAndArgs += " ";
  methodAndArgs += arg2;  
  this->Invoke(methodAndArgs.c_str());

}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (char* method, char* arg1, char* arg2, char* arg3)
{

  std::string methodAndArgs("");
  methodAndArgs += method;
  methodAndArgs += " ";
  methodAndArgs += arg1;
  methodAndArgs += " ";
  methodAndArgs += arg2;  
  methodAndArgs += " ";
  methodAndArgs += arg3;    
  this->Invoke(methodAndArgs.c_str());

}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (char* method, char* arg1, char* arg2, char* arg3, char* arg4)
{

  std::string methodAndArgs("");
  methodAndArgs += method;
  methodAndArgs += " ";
  methodAndArgs += arg1;
  methodAndArgs += " ";
  methodAndArgs += arg2;  
  methodAndArgs += " ";
  methodAndArgs += arg3;    
  methodAndArgs += " ";
  methodAndArgs += arg4;
  this->Invoke(methodAndArgs.c_str());

}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (char* method, char* arg1, char* arg2, char* arg3, char* arg4, char* arg5)
{

  std::string methodAndArgs("");
  methodAndArgs += method;
  methodAndArgs += " ";
  methodAndArgs += arg1;
  methodAndArgs += " ";
  methodAndArgs += arg2;  
  methodAndArgs += " ";
  methodAndArgs += arg3;    
  methodAndArgs += " ";
  methodAndArgs += arg4;
  methodAndArgs += " ";
  methodAndArgs += arg5;  
  this->Invoke(methodAndArgs.c_str());

}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Invoke (char* method, char* arg1, char* arg2, char* arg3, char* arg4, char* arg5, char* arg6)
{

  std::string methodAndArgs("");
  methodAndArgs += method;
  methodAndArgs += " ";
  methodAndArgs += arg1;
  methodAndArgs += " ";
  methodAndArgs += arg2;  
  methodAndArgs += " ";
  methodAndArgs += arg3;    
  methodAndArgs += " ";
  methodAndArgs += arg4;
  methodAndArgs += " ";
  methodAndArgs += arg5;  
  methodAndArgs += " ";
  methodAndArgs += arg6;    
  this->Invoke(methodAndArgs.c_str());

}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Enter ( ) 
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sEnter %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
//    std::stringstream pythonCommand;
//    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].Enter()\n";
//    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
//      {
//      PyErr_Print();
//      }
#endif
    }
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::Exit ( ) 
{
  if (this->Language == vtkScriptedModuleGUI::Tcl)
    {
    if (this->GetApplication())
      {
      this->GetApplication()->Script("%sExit %s", 
        this->GetModuleName(), this->GetTclName());
      }
    }
  else if (this->Language == vtkScriptedModuleGUI::Python)
    {
#ifdef Slicer3_USE_PYTHON
//    std::stringstream pythonCommand;
//    pythonCommand << "SlicerScriptedModuleInfo.Modules['" << this->GetModuleName() << "']['gui'].Exit()\n";
//    if (PyRun_SimpleString( pythonCommand.str().c_str() ) != 0)
//      {
//      PyErr_Print();
//      }
#endif
    }
}


//---------------------------------------------------------------------------
unsigned long vtkScriptedModuleGUI::AddObserverByNumber ( vtkObject *observee, unsigned long event ) {

  return ( observee->AddObserver(event, (vtkCommand *)this->GUICallbackCommand) );
} 

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::RemoveObserverByNumber ( vtkObject *observee, unsigned long event )
{
  if (observee == NULL)
    {
    vtkErrorMacro("RemoveGUIObserverByNumber: observee is null");
    return;
    }
  vtkDebugMacro("RemoveGUIObserverByNumber: event = " << event);
  if (observee->HasObserver(event, (vtkCommand *)this->GUICallbackCommand))
    {
    observee->RemoveObservers(event, (vtkCommand *)this->GUICallbackCommand);
    }
}

//---------------------------------------------------------------------------
unsigned long vtkScriptedModuleGUI::AddMRMLObserverByNumber ( vtkObject *observee, unsigned long event )
{
  if (observee == NULL)
    {
    vtkErrorMacro("AddMRMLObserverByNumber: observee is null, returning invalid event tag");
    return VTK_UNSIGNED_LONG_MAX;
    }
  if (observee->HasObserver(event, (vtkCommand *)this->MRMLCallbackCommand) )
    {
    return event;
    }
  return ( observee->AddObserver(event, (vtkCommand *)this->MRMLCallbackCommand) );
}

//---------------------------------------------------------------------------
void vtkScriptedModuleGUI::RemoveMRMLObserverByNumber ( vtkObject *observee, unsigned long event )
{
  if (observee == NULL)
    {
    vtkErrorMacro("RemoveMRMLObserverByNumber: observee is null");
    return;
    }
  vtkDebugMacro("RemoveMRMLObserverByNumber: event = " << event);
  if (observee->HasObserver(event, (vtkCommand *)this->MRMLCallbackCommand))
    {
    observee->RemoveObservers(event, (vtkCommand *)this->MRMLCallbackCommand);
    }
}

unsigned long vtkScriptedModuleGUI::GetNumberForVTKEvent( const char *eventName )
{
  return vtkCommand::GetEventIdFromString( eventName );
}
