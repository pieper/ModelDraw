/*=auto=========================================================================

  Portions (c) Copyright 2005 Brigham and Women's Hospital (BWH) 
  All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   3D Slicer

=========================================================================auto=*/

#include "vtkMRMLViewNode.h"

#include <stdlib.h>
#include <iostream>

#include "TestingMacros.h"

int vtkMRMLViewNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLViewNode > node1 = vtkSmartPointer< vtkMRMLViewNode >::New();

  EXERCISE_BASIC_OBJECT_METHODS( node1 );

  node1->UpdateReferences();

  vtkSmartPointer< vtkMRMLViewNode > node2 = vtkSmartPointer< vtkMRMLViewNode >::New();

  node2->Copy( node1 );

  return EXIT_SUCCESS;
}
