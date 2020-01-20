#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.

cmake_minimum_required(VERSION 2.8.11)

project(uhttp)

set(UHTTP_VERSION 1.0.1)

option(run_e2e_tests "set run_e2e_tests to ON to run e2e tests (default is OFF)" OFF)
option(run_unittests "set run_unittests to ON to run unittests (default is OFF)" OFF)
option(skip_samples "set skip_samples to ON to skip building samples (default is OFF)[if possible, they are always built]" OFF)
option(use_installed_dependencies "set use_installed_dependencies to ON to use installed packages instead of building dependencies from submodules" OFF)
option(use_custom_heap "use externally defined heap functions instead of the malloc family" OFF)

if(${use_custom_heap})
    add_definitions(-DGB_USE_CUSTOM_HEAP)
endif()

if(${no_logging})
    add_definitions(-DNO_LOGGING)
endif()

# Include the common build rules for the C SDK
include("deps/c-utility/configs/azure_iot_build_rules.cmake")

if(${use_openssl})
    add_definitions(-DUSE_OPENSSL)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_OPENSSL")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DUSE_OPENSSL")
endif()

#Use solution folders.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

enable_testing()

if(${memory_trace})
    add_definitions(-DGB_MEASURE_MEMORY_FOR_THIS -DGB_DEBUG_ALLOC)
endif()

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
endif()

set(UHTTP_C_INC_FOLDER ${CMAKE_CURRENT_LIST_DIR}/inc CACHE INTERNAL "this is what needs to be included if using sharedLib lib" FORCE)

#do not add or build any tests of the dependencies
set(original_run_e2e_tests ${run_e2e_tests})
set(original_run_unittests ${run_unittests})

set(run_e2e_tests OFF)
set(run_unittests OFF)

include("dependencies.cmake")

if (${original_run_unittests})
    set(SHARED_UTIL_REAL_TEST_FOLDER ${CMAKE_CURRENT_LIST_DIR}/deps/c-utility/tests/real_test_files CACHE INTERNAL "this is what needs to be included when doing test sources" FORCE)
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/deps/c-utility/testtools EXCLUDE_FROM_ALL)
endif()

set(use_cppunittest ON)
set_platform_files(${CMAKE_CURRENT_LIST_DIR}/deps/c-utility)

set(run_e2e_tests ${original_run_e2e_tests})
set(run_unittests ${original_run_unittests})

if (MSVC)
    detect_architecture("_M_AMD64" x86_64)
    detect_architecture("_M_IX86" x86)
    detect_architecture("_M_ARM" ARM)
else()
    detect_architecture("__x86_64__" x86_64)
    detect_architecture("__i386__" x86)
    detect_architecture("__arm__" ARM)
endif()
if (NOT DEFINED ARCHITECTURE OR ARCHITECTURE STREQUAL "")
    set(ARCHITECTURE "GENERIC")
endif()
message(STATUS "target architecture: ${ARCHITECTURE}")

set(uhttp_c_files
    ./src/uhttp.c
)

set(uhttp_h_files
    ./inc/azure_uhttp_c/uhttp.h
)

include_directories(./inc)
include_directories(${SHARED_UTIL_INC_FOLDER})

include("configs/azure_uhttpFunctions.cmake")

add_library(uhttp ${uhttp_c_files} ${uhttp_h_files})
setTargetBuildProperties(uhttp)
target_link_libraries(uhttp aziotsharedutil)

set_platform_files(${CMAKE_CURRENT_LIST_DIR}/deps/c-utility)

if (${run_unittests})
    include("dependencies-test.cmake")
    add_subdirectory(tests)
endif()

if (NOT ${skip_samples})
    add_subdirectory(samples)
endif()

if(${use_installed_dependencies})
    # Install uhttp
    set(package_location "cmake")

    if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
        set(CMAKE_INSTALL_LIBDIR "lib")
    endif()

    install(TARGETS uhttp EXPORT uhttpTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBDIR}/../bin
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azureiot
    )
    install(FILES ${uhttp_h_files} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azureiot/azure_uhttp_c)

    include(CMakePackageConfigHelpers)

    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${UHTTP_VERSION}
        COMPATIBILITY SameMajorVersion
    )

    install(
        FILES
            "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION
            ${package_location}
    )
else()
    set(install_staticlibs
        uhttp
    )
    install(FILES ${uhttp_h_files}
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/azure_uhttp_c)
    install(TARGETS ${install_staticlibs}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})
endif()

compileTargetAsC99(uhttp)
