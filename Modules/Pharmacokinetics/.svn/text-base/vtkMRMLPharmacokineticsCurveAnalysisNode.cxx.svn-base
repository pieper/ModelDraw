#include <string>
#include <iostream>
#include <sstream>
#include <map>

#include "vtkObjectFactory.h"

#include "vtkMRMLPharmacokineticsCurveAnalysisNode.h"
#include "vtkMRMLScene.h"

//------------------------------------------------------------------------------
vtkMRMLPharmacokineticsCurveAnalysisNode* vtkMRMLPharmacokineticsCurveAnalysisNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPharmacokineticsCurveAnalysisNode");
  if(ret)
    {
      return (vtkMRMLPharmacokineticsCurveAnalysisNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLPharmacokineticsCurveAnalysisNode;
}

//----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLPharmacokineticsCurveAnalysisNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLPharmacokineticsCurveAnalysisNode");
  if(ret)
    {
      return (vtkMRMLPharmacokineticsCurveAnalysisNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLPharmacokineticsCurveAnalysisNode;
}

//----------------------------------------------------------------------------
vtkMRMLPharmacokineticsCurveAnalysisNode::vtkMRMLPharmacokineticsCurveAnalysisNode()
{
  this->HideFromEditors = true;

  this->TargetCurve = NULL;
  this->FittedCurve = NULL;
  this->OutputValues.clear();
  this->Constants.clear();
  this->InitialParameters.clear();
  this->InputArrays.clear();
}

//----------------------------------------------------------------------------
vtkMRMLPharmacokineticsCurveAnalysisNode::~vtkMRMLPharmacokineticsCurveAnalysisNode()
{
}

//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::SetErrorMessage( const char *message)
{
  this->ErrorMessage.clear();
  this->ErrorMessage = message;
}







//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::WriteXML(ostream& of, int nIndent)
{
  // Start by having the superclass write its information
  Superclass::WriteXML(of, nIndent);
}

//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::ReadXMLAttributes(const char** atts)
{
  vtkMRMLNode::ReadXMLAttributes(atts);
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, VolumeID
void vtkMRMLPharmacokineticsCurveAnalysisNode::Copy(vtkMRMLNode *anode)
{

  Superclass::Copy(anode);
  vtkMRMLPharmacokineticsCurveAnalysisNode *node = (vtkMRMLPharmacokineticsCurveAnalysisNode *) anode;
  
  vtkStringArray* paramNames      = node->GetInitialParameterNameArray();
  vtkStringArray* inputParamNames = node->GetConstantNameArray();
  vtkStringArray* inputDataNames  = node->GetInputArrayNameArray();

  int numParameters      = paramNames->GetNumberOfTuples();
  int numInputParameters = inputParamNames->GetNumberOfTuples();
  int numInputCurves     = inputDataNames->GetNumberOfTuples();

  for (int i = 0; i < numParameters; i ++)
    {
    const char* name = paramNames->GetValue(i);
    double value = node->GetInitialParameter(name);
    this->SetInitialParameter(name, value);
    std::cerr << name << " = " << value << std::endl;
    }

  for (int i = 0; i < numInputParameters; i ++)
    {
    //int row = i + numParameters;
    const char* name = inputParamNames->GetValue(i);
    double value = node->GetConstant(name);
    this->SetConstant(name, value);
    std::cerr << name << " = " << value << std::endl;
    }

  for (int i = 0; i < numInputCurves; i ++)
    {
    //int row = i + numParameters + numInputParameters;
    const char* name = inputDataNames->GetValue(i);
    vtkDoubleArray* curve = node->GetInputArray(name);

    if (curve)
      {
      vtkDoubleArray* inputCurve = vtkDoubleArray::New();
      inputCurve->SetNumberOfComponents( curve->GetNumberOfComponents() );
      int nPoints   = curve->GetNumberOfTuples();

      for (int i = 0; i <= nPoints; i ++)
        {
        double* xy = curve->GetTuple(i);
        inputCurve->InsertNextTuple(xy);
        std::cerr << "input xy = " << xy[0] << ", " << xy[1] << std::endl;
        }
      this->SetInputArray(name, inputCurve);
      }
    }

}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::PrintSelf(ostream& os, vtkIndent indent)
{
  vtkMRMLNode::PrintSelf(os,indent);
}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::SetInputArray(const char* name, vtkDoubleArray* data)
{
  std::cerr << "void vtkMRMLPharmacokineticsCurveAnalysisNode::SetInputArray(const char* name, vtkDoubleArray* data)" << std::endl;
  std::cerr << name << std::endl;
  this->InputArrays[name] = data;
}


//----------------------------------------------------------------------------
vtkDoubleArray* vtkMRMLPharmacokineticsCurveAnalysisNode::GetInputArray(const char* name)
{
  ArrayMapType::iterator iter;
  iter = this->InputArrays.find(name);
  if (iter != this->InputArrays.end())
    {
    return iter->second;
    }
  else
    {
    return NULL;
    }
}

//----------------------------------------------------------------------------
vtkStringArray* vtkMRMLPharmacokineticsCurveAnalysisNode::GetInputArrayNameArray()
{
  ArrayMapType::iterator iter;
  vtkStringArray* array = vtkStringArray::New();

  for (iter = this->InputArrays.begin(); iter != this->InputArrays.end(); iter ++)
    {
    array->InsertNextValue(iter->first.c_str());
    }
  return array;
}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::ClearInputArrays()
{
  this->InputArrays.clear();
}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::SetInitialParameter(const char* name, double value)
{
  this->InitialParameters[name] = value;
}


//----------------------------------------------------------------------------
double vtkMRMLPharmacokineticsCurveAnalysisNode::GetInitialParameter(const char* name)
{
  ValueMapType::iterator iter;
  iter = this->InitialParameters.find(name);
  if (iter != this->InitialParameters.end())
    {
    return iter->second;
    }
  else
    {
    return 0.0;
    }
}


//----------------------------------------------------------------------------
vtkStringArray* vtkMRMLPharmacokineticsCurveAnalysisNode::GetInitialParameterNameArray()
{
  ValueMapType::iterator iter;
  vtkStringArray* array = vtkStringArray::New();

  for (iter = this->InitialParameters.begin(); iter != this->InitialParameters.end(); iter ++)
    {
    array->InsertNextValue(iter->first.c_str());
    }

  return array;
}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::ClearInitialParameters()
{
  this->InitialParameters.clear();
}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::SetConstant(const char *name, const double value)
{
  this->Constants[name] = value;
}



//----------------------------------------------------------------------------
double vtkMRMLPharmacokineticsCurveAnalysisNode::GetConstant(const char* name)
{
  ValueMapType::iterator iter;
  iter = this->Constants.find(name);
  if (iter != this->Constants.end())
    {
    return this->Constants[name]; 
    }
  else
    {
    return 0.0;
    }
}


//----------------------------------------------------------------------------
vtkStringArray* vtkMRMLPharmacokineticsCurveAnalysisNode::GetConstantNameArray()
{
  ValueMapType::iterator iter;
  vtkStringArray* array = vtkStringArray::New();
  
  for (iter = this->Constants.begin(); iter != this->Constants.end(); iter ++)
    {
    array->InsertNextValue(iter->first.c_str());
    }

  return array;
}


//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::ClearConstants()
{
  this->Constants.clear();
}



//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::SetOutputValue(const char *name, const double value)
{
  this->OutputValues[name] = value;
}


//----------------------------------------------------------------------------
double vtkMRMLPharmacokineticsCurveAnalysisNode::GetOutputValue(const char* name)
{
  ValueMapType::iterator iter;
  iter = this->OutputValues.find(name);
  if (iter != this->OutputValues.end())
    {
    return this->OutputValues[name]; 
    }
  else
    {
    return 0.0;
    }
}


//----------------------------------------------------------------------------
vtkStringArray* vtkMRMLPharmacokineticsCurveAnalysisNode::GetOutputValueNameArray()
{
  ValueMapType::iterator iter;
  vtkStringArray* array = vtkStringArray::New();

  for (iter = this->OutputValues.begin(); iter != this->OutputValues.end(); iter ++)
    {
    array->InsertNextValue(iter->first.c_str());
    }

  return array;
}



//----------------------------------------------------------------------------
void vtkMRMLPharmacokineticsCurveAnalysisNode::ClearOutputValues()
{
  this->OutputValues.clear();
}



