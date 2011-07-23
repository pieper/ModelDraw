SET(KIT AtlasCreatorLib)
SET(CMAKE_TESTDRIVER_BEFORE_TESTMAIN "DEBUG_LEAKS_ENABLE_EXIT_ERROR();" )
CREATE_TEST_SOURCELIST(Tests ${KIT}CxxTests.cxx
  vtkMRMLAtlasCreatorNodeTest1.cxx
  EXTRA_INCLUDE TestingMacros.h
  )
SET (TestsToRun ${Tests})
REMOVE (TestsToRun ${KIT}CxxTests.cxx)

SET(LIBRARY_NAME ${PROJECT_NAME})

ADD_EXECUTABLE(${KIT}CxxTests ${Tests})
TARGET_LINK_LIBRARIES(${KIT}CxxTests ${lib_name})

SET( ${KIT}_TESTS ${CXX_TEST_PATH}/${KIT}CxxTests)

IF(WIN32)
  SET(${KIT}_TESTS ${CXX_TEST_PATH}/${CMAKE_BUILD_TYPE}/${KIT}CxxTests)
ENDIF(WIN32)

MACRO( SIMPLE_TEST  TESTNAME )
  ADD_TEST( ${TESTNAME} ${LAUNCH_EXE} ${${KIT}_TESTS} ${TESTNAME} )
ENDMACRO( SIMPLE_TEST  )

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest1.py
  ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest1.py
  COPYONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest2.py
  ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest2.py
  COPYONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest3.py
  ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest3.py
  COPYONLY)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest3.py
  ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest4.py
  COPYONLY)

install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest1.py ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest2.py ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest3.py ${CMAKE_CURRENT_SOURCE_DIR}/vtkMRMLAtlasCreatorNodeLaunchTest4.py
  DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}
  )

SIMPLE_TEST( vtkMRMLAtlasCreatorNodeTest1 )

# add a tcl test 
# ADD_TEST(MeasurementsTest1 ${Slicer3_BINARY_DIR}/Slicer3 --test-mode --script ${Slicer3_SOURCE_DIR}/Modules/Measurements/Testing/Tcl/MeasurementsTest1.tcl)

IF(NOT WIN32)
# add python tests but only if we are not in windows
ADD_TEST(AtlasCreatorLaunchFixedTest ${Slicer3_BINARY_DIR}/Slicer3 --test-mode -p ${Slicer3_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest1.py)
ADD_TEST(AtlasCreatorLaunchFixedFailProofTest ${Slicer3_BINARY_DIR}/Slicer3 --test-mode -p ${Slicer3_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest2.py)
ADD_TEST(AtlasCreatorLaunchDynamicTest ${Slicer3_BINARY_DIR}/Slicer3 --test-mode -p ${Slicer3_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest3.py)
ADD_TEST(AtlasCreatorLaunchAffine12DOFTest ${Slicer3_BINARY_DIR}/Slicer3 --test-mode -p ${Slicer3_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/vtkMRMLAtlasCreatorNodeLaunchTest4.py)
ENDIF()
