/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include <stdlib.h>
#include <iostream>

#include "vtkSmartPointer.h"
#include "vtkImageEllipsoidSource.h"
#include "vtksys/SystemTools.hxx"

#include "vtkImageStash.h"

int main(int argc, char **argv)
{
  std::cout << "testing vtkImageStash\n";

  vtkSmartPointer<vtkImageEllipsoidSource> e = vtkSmartPointer<vtkImageEllipsoidSource>::New();
  vtkSmartPointer<vtkImageStash> stash = vtkSmartPointer<vtkImageStash>::New();

  std::cout << "** set up a big image in memory\n";

  e->SetWholeExtent(0, 1024, 0, 1024, 0, 256);
  e->Update();
  unsigned long actualSize = e->GetOutput()->GetActualMemorySize();

  std::cout << "** first, test non-threaded stashing\n";
  for (int i = 0; i < 2; ++i)
    {
    std::cout << "Ellipsoid image size pre-stash: " 
        << e->GetOutput()->GetActualMemorySize() << "\n";
    stash->SetStashImage(e->GetOutput());
    stash->Stash();
    std::cout << "Ellipsoid image size post-stash: " 
        << e->GetOutput()->GetActualMemorySize() << "\n";
    stash->Unstash();
    std::cout << "Ellipsoid image size post-unstash: " 
        << e->GetOutput()->GetActualMemorySize() << "\n";
    }

  if ( e->GetOutput()->GetActualMemorySize() != actualSize )
    {
    std::cerr << "bad size after stash/unstash!\n";
    return EXIT_FAILURE;
    }

  stash->Print(std::cout);

  std::cout << "** second, test threaded stashing\n";
  for (int i = 0; i < 2; ++i)
    {
    std::cout << "Ellipsoid image size pre-threaded-stash: " 
        << e->GetOutput()->GetActualMemorySize() << "\n";
    stash->SetStashImage(e->GetOutput());
    stash->ThreadedStash();
    std::cout << "Expect an error here (trying to unstash while thread is running): \n" ;
    stash->Unstash();
    while (stash->GetStashing())
      {
      std::cout << "Stash in process...\n";
      vtksys::SystemTools::Delay(300);
      }
    std::cout << "Ellipsoid image size post-threaded-stash: " 
        << e->GetOutput()->GetActualMemorySize() << "\n";
    stash->Unstash();
    std::cout << "Ellipsoid image size post-unstash: " 
        << e->GetOutput()->GetActualMemorySize() << "\n";
    }

  if ( e->GetOutput()->GetActualMemorySize() != actualSize )
    {
    std::cerr << "bad size after threaded stash/unstash!\n";
    return EXIT_FAILURE;
    }

  std::cout << "All tests passed!\n"; 

  return EXIT_SUCCESS;
}
