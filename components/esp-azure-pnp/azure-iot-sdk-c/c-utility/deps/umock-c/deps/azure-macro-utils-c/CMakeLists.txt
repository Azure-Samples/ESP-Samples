#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

if(TARGET azure_macro_utils_c)
    return()
endif()
    
project(azure_macro_utils_c)

set(AZURE_MACRO_UTILS_C_VERSION 1.1.0)

option(run_int_tests "set run_int_tests to ON to run int tests (default is OFF)" OFF)

#these are the include folders
#the following "set" statement exports across the project a global variable called MACRO_UTILS_INC_FOLDER that expands to whatever needs to included when using macro_utils
set(MACRO_UTILS_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using macro utils" FORCE)

include(CTest)
include(GNUInstallDirs)

install(FILES "inc/azure_macro_utils/macro_utils.h" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/azure_macro_utils")

if("${CMAKE_VERSION}" VERSION_GREATER 3.0.0)
    add_library(azure_macro_utils_c INTERFACE)
    install(TARGETS azure_macro_utils_c EXPORT azure_macro_utils_cTargets
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    #Install azure_iot_sdks
    set(package_location "cmake")

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${AZURE_MACRO_UTILS_C_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    configure_file("configs/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake"
        COPYONLY
    )

    install(EXPORT azure_macro_utils_cTargets
        FILE
            "${PROJECT_NAME}Targets.cmake"
        DESTINATION
            ${package_location}
    )

    install(
        FILES
            "configs/${PROJECT_NAME}Config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
            ${package_location}
    )
else()
    message(STATUS "This version of CMake does not support interface targets. Only the Azure Macro Utils header file will be installed. To use Azure Macro Utils, simply add \"MACRO_UTILS_INC_FOLDER\" to your include directories as specified in the instructions on the GitHub README.")
endif()

if(${run_int_tests})
    add_subdirectory(tests) 
endif()
