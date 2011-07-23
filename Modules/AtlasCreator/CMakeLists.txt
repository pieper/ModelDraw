project(AtlasCreator)

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

# --------------------------------------------------------------------------
# Find Slicer3

if(NOT Slicer3_SOURCE_DIR)
  find_package(Slicer3 REQUIRED)
  include(${Slicer3_USE_FILE})
  slicer3_set_default_install_prefix_for_external_projects()
endif(NOT Slicer3_SOURCE_DIR)

# --------------------------------------------------------------------------
# Include the Cxx/ directory
subDirs(
  Cxx
)

# --------------------------------------------------------------------------
# Include the ACITKTools directory
subDirs(
  ACITKTools
)

# --------------------------------------------------------------------------
# Include the TestData directory
ADD_CUSTOM_TARGET(copydata ALL
  ${CMAKE_COMMAND}
  -DSRC=${CMAKE_CURRENT_SOURCE_DIR}/TestData
  -DDST=${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_SHARE_DIR}/${PROJECT_NAME}/
  -P ${CMAKE_CURRENT_SOURCE_DIR}/copydata.cmake
)

install(
  DIRECTORY TestData
  DESTINATION ${Slicer3_INSTALL_MODULES_SHARE_DIR}/${PROJECT_NAME}/
  FILES_MATCHING PATTERN "*"
)


# copy UPenn_logo.png
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/UPenn_logo.png
  ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_SHARE_DIR}/${PROJECT_NAME}/UPenn_logo.png
  COPYONLY)
  
install(
  FILES ${CMAKE_CURRENT_SOURCE_DIR}/UPenn_logo.png
  DESTINATION ${Slicer3_INSTALL_MODULES_SHARE_DIR}/${PROJECT_NAME}
  )


# --------------------------------------------------------------------------
# Copy over the Python files

if(Slicer3_USE_PYTHON)

  file(GLOB PYTHONFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.py")
  file(GLOB TCLFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.tcl")

  foreach(file ${PYTHONFILES})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${file}
      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/${file}
      COPYONLY)
  endforeach(file)
  
  foreach(file ${TCLFILES})
    configure_file(
      ${CMAKE_CURRENT_SOURCE_DIR}/${file}
      ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/${file}
      COPYONLY)
  endforeach(file)  

  # copy template.sh
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/template.sh
    ${CMAKE_BINARY_DIR}/${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}/template.sh
    COPYONLY)
  
  install(
    FILES ${PYTHONFILES} ${TCLFILES} ${CMAKE_CURRENT_SOURCE_DIR}/template.sh
    DESTINATION ${Slicer3_INSTALL_MODULES_LIB_DIR}/${PROJECT_NAME}
    )

endif(Slicer3_USE_PYTHON)

