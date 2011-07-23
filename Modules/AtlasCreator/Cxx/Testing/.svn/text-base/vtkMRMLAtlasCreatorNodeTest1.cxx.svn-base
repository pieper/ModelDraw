#include "vtkMRMLAtlasCreatorNode.h"

#include <stdlib.h>
#include <iostream>

#include "TestingMacros.h"

int vtkMRMLAtlasCreatorNodeTest1(int , char * [] )
{
  vtkSmartPointer< vtkMRMLAtlasCreatorNode > node1 = vtkSmartPointer< vtkMRMLAtlasCreatorNode >::New();
 EXERCISE_BASIC_OBJECT_METHODS( node1 );

  node1->UpdateReferences();

  vtkSmartPointer< vtkMRMLAtlasCreatorNode > node2 = vtkSmartPointer< vtkMRMLAtlasCreatorNode >::New();

  node2->Copy( node1 );

  node2->Reset();

  node2->InitializeByDefault();

  node2->StartModify();

  std::string nodeTagName = node1->GetNodeTagName();

  std::cout << "Node Tag Name = " << nodeTagName << std::endl;

  TEST_SET_GET_BOOLEAN( node1, HideFromEditors );
  TEST_SET_GET_BOOLEAN( node1, Selectable );

  TEST_SET_GET_STRING( node1, Description );
  TEST_SET_GET_STRING( node1, SceneRootDir );
  TEST_SET_GET_STRING( node1, Name );
  TEST_SET_GET_STRING( node1, SingletonTag );

  TEST_SET_GET_BOOLEAN( node1, ModifiedSinceRead );
  TEST_SET_GET_BOOLEAN( node1, SaveWithScene );
  TEST_SET_GET_BOOLEAN( node1, AddToScene );
  TEST_SET_GET_BOOLEAN( node1, Selected );


  TEST_SET_GET_STRING( node1, OriginalImagesFilePathList );
  TEST_SET_GET_STRING( node1, SegmentationsFilePathList );
  TEST_SET_GET_STRING( node1, OutputDirectory );
  TEST_SET_GET_STRING( node1, Toolkit );
  TEST_SET_GET_STRING( node1, TemplateType );
  TEST_SET_GET_STRING( node1, FixedTemplateDefaultCaseFilePath );
  TEST_SET_GET_STRING( node1, LabelsList );
  TEST_SET_GET_STRING( node1, RegistrationType );
  TEST_SET_GET_STRING( node1, OutputCast );
  TEST_SET_GET_STRING( node1, SchedulerCommand );
  TEST_SET_GET_STRING( node1, ExistingTemplate );
  TEST_SET_GET_STRING( node1, TransformsDirectory );

  TEST_SET_GET_INT( node1, DynamicTemplateIterations, 10 );
  TEST_SET_GET_INT( node1, DeleteAlignedImages, 0 );
  TEST_SET_GET_INT( node1, DeleteAlignedImages, 1 );
  TEST_SET_GET_INT( node1, DeleteAlignedSegmentations, 0 );
  TEST_SET_GET_INT( node1, DeleteAlignedSegmentations, 1 );
  TEST_SET_GET_INT( node1, NormalizeAtlases, 0 );
  TEST_SET_GET_INT( node1, NormalizeAtlases, 1 );
  TEST_SET_GET_INT( node1, NormalizeTo, -1 );
  TEST_SET_GET_INT( node1, NormalizeTo, 100 );
  TEST_SET_GET_INT( node1, PCAAnalysis, 0 );
  TEST_SET_GET_INT( node1, PCAAnalysis, 1 );
  TEST_SET_GET_INT( node1, PCAMaxEigenVectors, 10 );
  TEST_SET_GET_INT( node1, PCACombine, 0 );
  TEST_SET_GET_INT( node1, PCACombine, 1 );
  TEST_SET_GET_INT( node1, UseCluster, 0 );
  TEST_SET_GET_INT( node1, UseCluster, 1 );
  TEST_SET_GET_INT( node1, SkipRegistration, 0 );
  TEST_SET_GET_INT( node1, SkipRegistration, 1 );
  TEST_SET_GET_INT( node1, NumberOfThreads, -1);
  TEST_SET_GET_INT( node1, NumberOfThreads, 5);
  TEST_SET_GET_INT( node1, DebugMode, 0 );
  TEST_SET_GET_INT( node1, DebugMode, 1 );
  TEST_SET_GET_INT( node1, DryrunMode, 0 );
  TEST_SET_GET_INT( node1, DryrunMode, 1 );
  TEST_SET_GET_INT( node1, SaveTransforms, 0 );
  TEST_SET_GET_INT( node1, SaveTransforms, 1 );


  node1->Modified();
  vtkMRMLScene * scene = node1->GetScene();

  if( scene != NULL )
    {
    std::cerr << "Error in GetScene() " << std::endl;
    return EXIT_FAILURE;
    }

  std::string stringToEncode = "Thou Shall Test !";
  std::string stringURLEncoded = node1->URLEncodeString( stringToEncode.c_str() );

  std::string stringDecoded = node1->URLDecodeString( stringURLEncoded.c_str() );

  if( stringDecoded != stringToEncode )
    {
    std::cerr << "Error in URLEncodeString/URLDecodeString() " << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
