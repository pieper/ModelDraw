#
# Install the testing data and include the Testing/ sub-directory.
#

# Install the testing data
install(DIRECTORY
    Testing/TestData
    DESTINATION ${EM_SHARE_DIR}/${PROJECT_NAME}/Testing COMPONENT Development
    PATTERN ".svn" EXCLUDE
    )

# include subdirectory
if(BUILD_TESTING)
    ADD_SUBDIRECTORY(Testing)
endif(BUILD_TESTING)
