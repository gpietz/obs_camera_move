set(OBS_SEARCH_PATHS
    "${CMAKE_SOURCE_DIR}/obs-studio"
    "D:/Development/Samples/Cpp/obs-studio/libobs"              # Include directory (development)
    "D:/Development/Samples/Cpp/obs-studio/build/libobs/Debug"  # Debug build path (development)
    "D:/Development/Samples/Cpp/obs-studio/UI/obs-frontend-api"
    "D:/Development/Samples/Cpp/obs-studio/build/UI/obs-frontend-api/Debug/"
    "/usr/local/include/obs"                                    # Default path for Linux/macos
    "C:/Program Files/obs-studio/include"                       # Default path for Windows
)

# Find header files
find_path(OBS_INCLUDE_DIR
    NAMES obs.h
    PATHS ${OBS_SEARCH_PATHS}
)
find_path(OBS_FRONTEND_INCLUDE_DIR
          names obs-frontend-api.h
          PATHS ${OBS_SEARCH_PATHS})

# Find library
find_library(OBS_LIBRARY
    NAMES obs
    PATHS ${OBS_SEARCH_PATHS}
)
find_library(OBS_FRONTEND_LIBRARY
    NAMES obs-frontend-api.lib
    PATHS ${OBS_SEARCH_PATHS}
)

message(STATUS "OBS_INCLUDE_DIR: ${OBS_INCLUDE_DIR}")
message(STATUS "OBS_LIBRARY: ${OBS_LIBRARY}")
message(STATUS "OBS_FRONTEND_INCLUDE_DIR: ${OBS_FRONTEND_INCLUDE_DIR}")
message(STATUS "OBS_FRONTEND_LIBRARY: ${OBS_FRONTEND_LIBRARY}")

# Check dependencies
if (NOT OBS_INCLUDE_DIR OR NOT OBS_LIBRARY)
    message(FATAL_ERROR "OBS SDK could not be found. Please make sure that the SDK is installed correctly.")
endif()
if (NOT OBS_FRONTEND_INCLUDE_DIR OR NOT OBS_FRONTEND_LIBRARY)
    message(FATAL_ERROR "OBS SDK Frontend API could not be found. Please make sure that the SDK is installed correctly.")
endif()

# Set results
set(OBS_FOUND TRUE)
set(OBS_INCLUDE_DIRS ${OBS_INCLUDE_DIR})
set(OBS_LIBRARIES ${OBS_LIBRARY})

# Include directive for CMake
mark_as_advanced(OBS_INCLUDE_DIR OBS_LIBRARY)
