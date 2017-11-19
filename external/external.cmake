# Install external libraries

# Catch
message(STATUS "Installing Catch submodule")
execute_process(COMMAND git submodule update --init -- external/Catch2 WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
