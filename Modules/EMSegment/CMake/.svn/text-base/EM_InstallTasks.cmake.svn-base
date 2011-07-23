#
# Install (copy) the task related .tcl and .mrml files to the appropriate places
#

ADD_CUSTOM_TARGET(copytasks ALL
  ${CMAKE_COMMAND}
  -DSRC=${CMAKE_CURRENT_SOURCE_DIR}/Tasks
  -DDST=${CMAKE_BINARY_DIR}/${EM_SHARE_DIR}/${PROJECT_NAME}/
  -P ${CMAKE_CURRENT_SOURCE_DIR}/CMake/EM_InstallTasksSubScript.cmake
)

install(
  DIRECTORY Tasks
  DESTINATION ${EM_SHARE_DIR}/${PROJECT_NAME}/
  FILES_MATCHING PATTERN "*"
)


file(GLOB TCLFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Tcl/*.tcl" )
# MESSAGE(STATUS  "${TCLFILES} ${CMAKE_CURRENT_SOURCE_DIR}")
foreach(file ${TCLFILES})
 configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    ${CMAKE_BINARY_DIR}/${EM_SHARE_DIR}/${PROJECT_NAME}/${file}
    COPYONLY)
endforeach(file)
# MESSAGE(STATUS  "${TCLFILES} ${CMAKE_CURRENT_SOURCE_DIR}")

install(
  FILES ${TCLFILES}
  DESTINATION ${EM_SHARE_DIR}/${PROJECT_NAME}/Tcl
)

### this code will be replaced by the cmake command 'file(COPY ....)' below 
file(GLOB TASKFILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Tasks/*.tcl" "Tasks/*.mrml")
foreach(file ${TASKFILES})
 configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
    ${CMAKE_BINARY_DIR}/${EM_SHARE_DIR}/${PROJECT_NAME}/${file}
    COPYONLY)
endforeach(file)
#
#install(
#  FILES ${TASKFILES}
#  DESTINATION ${EM_SHARE_DIR}/${PROJECT_NAME}/Tasks
#)
###
