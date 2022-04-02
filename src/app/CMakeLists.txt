# Main application 
if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/${APP_NAME}" AND IS_DIRECTORY "${CMAKE_CURRENT_LIST_DIR}/${APP_NAME}")
    add_subdirectory(${APP_NAME})
    message(STATUS "Building firmware for app \"${APP_NAME}\" with board \"${BOARD}\" <--------------")
else()
    message(STATUS "Cannot find the app folder: ${APP_NAME}")
endif()
