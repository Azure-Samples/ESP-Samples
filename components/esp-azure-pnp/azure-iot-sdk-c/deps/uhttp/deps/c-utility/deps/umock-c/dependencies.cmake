#Copyright (c) Microsoft. All rights reserved.
#Licensed under the MIT license. See LICENSE file in the project root for full license information.


set(original_run_e2e_tests ${run_e2e_tests})
set(original_run_int_tests ${run_int_tests})
set(original_run_unittests ${run_unittests})

set(run_e2e_tests OFF)
set(run_int_tests OFF)
set(run_unittests OFF)

if(${use_installed_dependencies})
    find_package(azure_macro_utils_c REQUIRED CONFIG)
else()
    if (NOT TARGET azure_macro_utils_c) 
	if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/deps/azure-macro-utils-c/CMakeLists.txt)
            add_subdirectory(deps/azure-macro-utils-c)
        else()
            message(FATAL_ERROR "Could not find azure_macro_utils_c")
        endif()
    endif()
endif()

if(${original_run_int_tests} OR ${original_run_unittests})
    if(${use_installed_dependencies})
        find_package(ctest REQUIRED CONFIG)
        find_package(testrunnerswitcher REQUIRED CONFIG)
    else()
        if (NOT TARGET testrunnerswitcher)
            add_subdirectory(deps/testrunner)
        endif()
        if (NOT TARGET ctest)
            add_subdirectory(deps/ctest)
        endif()
    endif()
endif()

set(run_e2e_tests ${original_run_e2e_tests})
set(run_int_tests ${original_run_int_tests})
set(run_unittests ${original_run_unittests})

