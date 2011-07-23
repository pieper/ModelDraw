/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkITKArchetypeImageSeriesScalarReader.h"

#include <stdlib.h>
#include <iostream>

#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>

#include "vtkImageData.h"

int main(int argc, char * argv[])
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

  std::cout << std::endl;
  itk::SerieUIDContainer seriesUIDs = names->GetSeriesUIDs();
  std::cout << "Series name: " << seriesUIDs[0] << std::endl;

  const ReaderType::FileNamesContainer &filenames =
    names->GetFileNames(seriesUIDs[0]);
  std::cout << "Number of filenames: " << filenames.size() << std::endl;
  
  vtkITKArchetypeImageSeriesScalarReader *reader = 
    vtkITKArchetypeImageSeriesScalarReader::New();

  reader->SetSingleFile(0);
 
  // add the file names in-order 
  ReaderType::FileNamesContainer::const_iterator fileIter;
  fileIter = filenames.begin();
  reader->SetArchetype( fileIter->c_str() );
  for(ReaderType::FileNamesContainer::const_iterator fileIter = filenames.begin();
    fileIter!=filenames.end();++fileIter){
    reader->AddFileName(fileIter->c_str());    
  }

  reader->Update();
  double origin1[3];
  reader->GetOutput()->GetOrigin(origin1);
  
  std::cout << std::endl;
  std::cout << "========== reading files in order: " << std::endl;
  std::cout << "Origin: " << origin1[0] << "," << origin1[1] << "," 
    << origin1[2] << std::endl;
  std::cout << std::endl;

  reader->Delete();



  //
  // create a fresh reader to reset the internal state
  //
  reader = vtkITKArchetypeImageSeriesScalarReader::New();
  reader->SetSingleFile(0);

  // add the file names in backwards order for storage node 2
  fileIter = filenames.end()-1;
  reader->SetArchetype( fileIter->c_str() );
  for(; fileIter!=filenames.begin();--fileIter){
    reader->AddFileName(fileIter->c_str());    
  }
  reader->AddFileName(filenames.begin()->c_str());

  reader->Modified();
  reader->UpdateInformation();
  reader->Update();
  double origin2[3];
  reader->GetOutput()->GetOrigin(origin2);

  std::cout << std::endl;
  std::cout << "========== reading files in reverse order: " << std::endl;
  std::cout << "Origin: " << origin2[0] << "," << origin2[1] << "," 
    << origin2[2] << std::endl;

  bool testFailed = false;
  if( origin1[0]!=origin2[0] || 
      origin1[1]!=origin2[1] ||
      origin1[2]!=origin2[2] )
    {
    testFailed = true;
    }

  reader->Delete();

  std::cout << std::endl;
  if(testFailed == true)
    {
    std::cout << "##%%&^*^$% test ##%%&^*^$% failed" << std::endl;
    return EXIT_FAILURE;
    }

  std::cout << ":D test passed" << std::endl;
  return EXIT_SUCCESS;
}
