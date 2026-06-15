# Loads machine-local dependency paths from cmake/local_paths.cmake.
set(_imguitry_local_paths "${CMAKE_CURRENT_LIST_DIR}/local_paths.cmake")
if(EXISTS "${_imguitry_local_paths}")
    include("${_imguitry_local_paths}")
else()
    message(FATAL_ERROR
        "Missing cmake/local_paths.cmake.\n"
        "Copy cmake/local_paths.cmake.example to cmake/local_paths.cmake and edit paths for this machine.")
endif()
unset(_imguitry_local_paths)
