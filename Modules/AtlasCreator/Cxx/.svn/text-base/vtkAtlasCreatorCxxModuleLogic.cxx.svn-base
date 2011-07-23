/*=auto=========================================================================

 Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

 See Doc/copyright/copyright.txt
 or http://www.slicer.org/copyright/copyright.txt for details.

 Program:   3D Slicer
 Module:    $RCSfile: vtkAtlasCreatorCxxModuleLogic.cxx,v $
 Date:      $Date: 2006/03/17 15:10:10 $
 Version:   $Revision: 1.2 $

 =========================================================================auto=*/

#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>

#include "vtkObjectFactory.h"

#include "vtkAtlasCreatorCxxModuleLogic.h"
#include "vtkAtlasCreatorCxxModule.h"

#include "vtkMRMLAtlasCreatorNode.h"

#include "vtkMRMLScene.h"

#include "vtkImageData.h"

#include "vtkSlicerApplication.h"

//----------------------------------------------------------------------------
// A helper class to compare two maps
template<class T>
class MapCompare
{
public:
  static bool map_value_comparer(typename std::map<T, unsigned int>::value_type &i1, typename std::map<
      T, unsigned int>::value_type &i2)
  {
    return i1.second < i2.second;
  }
};

vtkAtlasCreatorCxxModuleLogic* vtkAtlasCreatorCxxModuleLogic::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance(
      "vtkAtlasCreatorCxxModuleLogic");
  if (ret)
    {
    return (vtkAtlasCreatorCxxModuleLogic*) ret;
    }

  // If the factory was unable to create the object, then create it here.
  return new vtkAtlasCreatorCxxModuleLogic;
}
//----------------------------------------------------------------------------


//----------------------------------------------------------------------------
vtkAtlasCreatorCxxModuleLogic::vtkAtlasCreatorCxxModuleLogic()
{

}

//----------------------------------------------------------------------------
vtkAtlasCreatorCxxModuleLogic::~vtkAtlasCreatorCxxModuleLogic()
{

}

//----------------------------------------------------------------------------
void vtkAtlasCreatorCxxModuleLogic::RegisterNodes()
{

  vtkMRMLScene* scene = this->GetMRMLScene();

  if (scene)
    {
    vtkMRMLAtlasCreatorNode* atlasCreatorNode = vtkMRMLAtlasCreatorNode::New();
    scene->RegisterNodeClass(atlasCreatorNode);
    atlasCreatorNode->Delete();
    }

}

//----------------------------------------------------------------------------
void vtkAtlasCreatorCxxModuleLogic::PrintSelf(ostream& os, vtkIndent indent)
{

}

//----------------------------------------------------------------------------
// loops through the faces of the image bounding box and counts all the different image values and stores them in a map
// T represents the image data type
template<class T>
T vtkAtlasCreatorCxxModuleLogic::GuessRegistrationBackgroundLevel(vtkImageData* imageData)
{
  int borderWidth = 5;
  T inLevel;
  typedef std::map<T, unsigned int> MapType;
  MapType m;
  long totalVoxelsCounted = 0;

  T* inData = static_cast<T*> (imageData->GetScalarPointer());
  int dim[3];
  imageData->GetDimensions(dim);

  vtkIdType inc[3];
  vtkIdType iInc, jInc, kInc;
  imageData->GetIncrements(inc);

  // k first slice
  for (int k = 0; k < borderWidth; ++k)
    {
    kInc = k * inc[2];
    for (int j = 0; j < dim[1]; ++j)
      {
      jInc = j * inc[1];
      for (int i = 0; i < dim[0]; ++i)
        {
        iInc = i * inc[0];
        inLevel = inData[iInc + jInc + kInc];
        if (m.count(inLevel))
          {
          ++m[inLevel];
          }
        else
          {
          m[inLevel] = 1;
          }
        ++totalVoxelsCounted;
        }
      }
    }

  // k last slice
  for (int k = dim[2] - borderWidth; k < dim[2]; ++k)
    {
    kInc = k * inc[2];
    for (int j = 0; j < dim[1]; ++j)
      {
      jInc = j * inc[1];
      for (int i = 0; i < dim[0]; ++i)
        {
        iInc = i * inc[0];
        inLevel = inData[iInc + jInc + kInc];
        if (m.count(inLevel))
          {
          ++m[inLevel];
          }
        else
          {
          m[inLevel] = 1;
          }
        ++totalVoxelsCounted;
        }
      }
    }

  // j first slice
  for (int j = 0; j < borderWidth; ++j)
    {
    jInc = j * inc[1];
    for (int k = 0; k < dim[2]; ++k)
      {
      kInc = k * inc[2];
      for (int i = 0; i < dim[0]; ++i)
        {
        iInc = i * inc[0];
        inLevel = inData[iInc + jInc + kInc];
        if (m.count(inLevel))
          {
          ++m[inLevel];
          }
        else
          {
          m[inLevel] = 1;
          }
        ++totalVoxelsCounted;
        }
      }
    }

  // j last slice
  for (int j = dim[1] - borderWidth; j < dim[1]; ++j)
    {
    jInc = j * inc[1];
    for (int k = 0; k < dim[2]; ++k)
      {
      kInc = k * inc[2];
      for (int i = 0; i < dim[0]; ++i)
        {
        iInc = i * inc[0];
        inLevel = inData[iInc + jInc + kInc];
        if (m.count(inLevel))
          {
          ++m[inLevel];
          }
        else
          {
          m[inLevel] = 1;
          }
        ++totalVoxelsCounted;
        }
      }
    }

  // i first slice
  for (int i = 0; i < borderWidth; ++i)
    {
    iInc = i * inc[0];
    for (int k = 0; k < dim[2]; ++k)
      {
      kInc = k * inc[2];
      for (int j = 0; j < dim[1]; ++j)
        {
        jInc = j * inc[1];
        inLevel = inData[iInc + jInc + kInc];
        if (m.count(inLevel))
          {
          ++m[inLevel];
          }
        else
          {
          m[inLevel] = 1;
          }
        ++totalVoxelsCounted;
        }
      }
    }

  // i last slice
  for (int i = dim[0] - borderWidth; i < dim[0]; ++i)
    {
    iInc = i * inc[0];
    for (int k = 0; k < dim[2]; ++k)
      {
      kInc = k * inc[2];
      for (int j = 0; j < dim[1]; ++j)
        {
        jInc = j * inc[1];
        inLevel = inData[iInc + jInc + kInc];
        if (m.count(inLevel))
          {
          ++m[inLevel];
          }
        else
          {
          m[inLevel] = 1;
          }
        ++totalVoxelsCounted;
        }
      }
    }

  // all the information is stored in map m :  std::map<T, unsigned int>

  if (m.empty())
    {
    // no image data provided?
    return 0;
    }
  else if (m.size() == 1)
    {
    // Homogeneous background
    return m.begin()->first;
    }
  else
    {
    // search for the largest element
    typename MapType::iterator itor = std::max_element(m.begin(), m.end(),
        MapCompare<T>::map_value_comparer);

    // the iterator is pointing to the element with the largest value in the range [m.begin(), m.end()]
    T backgroundLevel = itor->first;

    // how many counts?
    double percentageOfVoxels = 100.0 * static_cast<double> (itor->second)
        / totalVoxelsCounted;

    std::cout << "   Background level guess : " << std::endl
        << "   first place: " << static_cast<int> (backgroundLevel) << " ("
        << percentageOfVoxels << "%) " << std::endl;

    // erase largest element
    m.erase(itor);

    // again, search for the largest element (second place)
    typename MapType::iterator itor2 = std::max_element(m.begin(), m.end(),
        MapCompare<T>::map_value_comparer);

    T backgroundLevel_second_place = itor2->first;

    double percentageOfVoxels_secondplace = 100.0
        * static_cast<double> (itor2->second) / totalVoxelsCounted;

    std::cout << "   second place: "
        << static_cast<int> (backgroundLevel_second_place) << " ("
        << percentageOfVoxels_secondplace << "%)" << std::endl;

    return backgroundLevel;
    }
}

//----------------------------------------------------------------------------
double vtkAtlasCreatorCxxModuleLogic::GuessRegistrationBackgroundLevel(vtkMRMLVolumeNode* volumeNode)
{
  if (!volumeNode || !volumeNode->GetImageData())
    {
    std::cerr
        << "double vtkAtlasCreatorCxxModuleLogic::GuessRegistrationBackgroundLevel(vtkMRMLVolumeNode* volumeNode) : volumeNode or volumeNode->GetImageData is null"
        << std::endl;
    return -1;
    }

  // guess background level
  double backgroundLevel = 0;
  switch (volumeNode->GetImageData()->GetScalarType())
    {
    vtkTemplateMacro(backgroundLevel = (GuessRegistrationBackgroundLevel<VTK_TT>(volumeNode->GetImageData()));)
      ;
    }
  std::cout << "   Guessed background level: " << backgroundLevel << std::endl;
  return backgroundLevel;
}

//----------------------------------------------------------------------------
vtkMRMLScalarVolumeNode*
vtkAtlasCreatorCxxModuleLogic::AddArchetypeScalarVolume(const char* filename, const char* volname, vtkSlicerApplicationLogic* appLogic, vtkMRMLScene* mrmlScene)
{
  vtkSlicerVolumesLogic* volLogic = vtkSlicerVolumesLogic::New();
  volLogic->SetMRMLScene(mrmlScene);
  volLogic->SetApplicationLogic(appLogic);
  vtkMRMLScalarVolumeNode* volNode = NULL;

  volNode = volLogic->AddArchetypeScalarVolume(filename, volname, 0);

  volLogic->Delete();
  return volNode;
}
