/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLVolumeArchetypeStorageNode.h"
#include "vtkMRMLScalarVolumeNode.h"

#include <stdlib.h>
#include <iostream>

#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>

#include "TestingMacros.h"

int vtkMRMLVolumeArchetypeStorageNodeTest2(int argc, char * argv[])
{
  if(argc<2){
    std::cerr << "Expected to see a path on input!" << std::endl;
    return EXIT_FAILURE;
  }

  // glob for filenames from input path
  typedef itk::Image<char,3> ImageType;
  typedef itk::ImageSeriesReader<ImageType> ReaderType;
  ReaderType::FileNamesContainer fnames;

  typedef itk::GDCMSeriesFileNames InputNamesGenerator;
  InputNamesGenerator::Pointer names = InputNamesGenerator::New();
  names->SetDirectory(argv[1]);
  names->SetUseSeriesDetails(true);

  itk::SerieUIDContainer seriesUIDs = names->GetSeriesUIDs();
  std::cout << "Series name: " << seriesUIDs[0] << std::endl;

  const ReaderType::FileNamesContainer &filenames =
    names->GetFileNames(seriesUIDs[0]);
  std::cout << "Number of filenames: " << filenames.size() << std::endl;
  
  vtkMRMLVolumeArchetypeStorageNode *storageNode1 = 
    vtkMRMLVolumeArchetypeStorageNode::New();
  storageNode1->SetFileName(filenames[0].c_str());
  storageNode1->ResetFileNameList();
  storageNode1->SetUseOrientationFromFile(1);
 
  // add the file names in-order for storage node 1
  for(ReaderType::FileNamesContainer::const_iterator fI = filenames.begin();
    fI!=filenames.end();++fI){
    storageNode1->AddFileName(fI->c_str());    
  }
  
  vtkMRMLScalarVolumeNode *volumeNode1 = vtkMRMLScalarVolumeNode::New();
  storageNode1->SetSingleFile(0);
  storageNode1->ReadData(volumeNode1);
  storageNode1->SetUseOrientationFromFile(1);
  double* origin1 = volumeNode1->GetOrigin();
  
  std::cout << "========== storageNode1: " << std::endl;
  storageNode1->Print(std::cout);
  std::cout << "Origin: " << origin1[0] << "," << origin1[1] << "," 
    << origin1[2] << std::endl;


  std::cout << std::endl;

  vtkMRMLVolumeArchetypeStorageNode *storageNode2 = 
    vtkMRMLVolumeArchetypeStorageNode::New();
  storageNode2->SetFileName(filenames[filenames.size()-1].c_str());
  storageNode2->ResetFileNameList();

  // add the file names in backwards order for storage node 2
  for(ReaderType::FileNamesContainer::const_iterator fI = filenames.end()-1;
    fI!=filenames.begin();--fI){
    storageNode2->AddFileName(fI->c_str());    
  }
  storageNode2->AddFileName(filenames.begin()->c_str());

  vtkMRMLScalarVolumeNode *volumeNode2 = vtkMRMLScalarVolumeNode::New();
  storageNode2->SetSingleFile(0);
  storageNode2->ReadData(volumeNode2);
  storageNode2->SetUseOrientationFromFile(1);

  std::cout << "========== storageNode2: " << std::endl;
  storageNode2->Print(std::cout);

  double* origin2 = volumeNode2->GetOrigin();
  std::cout << "Origin: " << origin2[0] << "," << origin2[1] << "," 
    << origin2[2] << std::endl;

  bool testFailed = false;
  if(origin1[0]!=origin2[0] || 
     origin1[1]!=origin2[1] ||
     origin1[2]!=origin2[2])
    testFailed = true;

  volumeNode1->Delete();
  storageNode1->Delete();
  volumeNode2->Delete();
  storageNode2->Delete();

  if(testFailed)
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
