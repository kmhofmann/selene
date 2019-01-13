# Install external libraries

find_package(Git QUIET)

if (NOT GIT_FOUND AND SELENE_BUILD_TESTS)
    message(FATAL_ERROR "Git not found. Git is required for initializing/updating submodules.")
endif()

# Catch2
if (SELENE_BUILD_TESTS)
    execute_process(COMMAND ${GIT_EXECUTABLE} submodule update --init -- external/Catch2
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE GIT_SUBMODULE_RESULT_CATCH)
    if (NOT GIT_SUBMODULE_RESULT_CATCH EQUAL "0")
        message(FATAL_ERROR "Catch2 Git submodule could not be initialized or updated.")
    endif()
endif()
