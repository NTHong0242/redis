cmake_minimum_required(VERSION 3.10)
project(redis C)

set(CMAKE_C_STANDARD 11)

# Include headers từ project
include_directories(${PROJECT_SOURCE_DIR}/include)

# Detect OS và set paths cho hiredis
if(APPLE)
    # macOS - Homebrew paths
    find_path(HIREDIS_INCLUDE_DIR 
        NAMES hiredis/hiredis.h
        PATHS /opt/homebrew/include /usr/local/include
        NO_DEFAULT_PATH
    )
    find_library(HIREDIS_LIBRARY
        NAMES hiredis
        PATHS /opt/homebrew/lib /usr/local/lib
        NO_DEFAULT_PATH
    )
    
    if(NOT HIREDIS_INCLUDE_DIR OR NOT HIREDIS_LIBRARY)
        message(FATAL_ERROR "hiredis not found! Install with: brew install hiredis")
    endif()
    
    include_directories(${HIREDIS_INCLUDE_DIR})
    message(STATUS "Found hiredis include: ${HIREDIS_INCLUDE_DIR}")
    message(STATUS "Found hiredis library: ${HIREDIS_LIBRARY}")
    
elseif(WIN32)
    # Windows - hardcoded paths
    include_directories("G:/hiredis" "G:/hiredis/hiredis")
    link_directories("G:/hiredis/build")
    set(HIREDIS_LIBRARY hiredis)
    
else()
    # Linux - system paths
    find_path(HIREDIS_INCLUDE_DIR hiredis/hiredis.h)
    find_library(HIREDIS_LIBRARY hiredis)
    
    if(NOT HIREDIS_INCLUDE_DIR OR NOT HIREDIS_LIBRARY)
        message(FATAL_ERROR "hiredis not found! Install with: apt-get install libhiredis-dev")
    endif()
    
    include_directories(${HIREDIS_INCLUDE_DIR})
endif()

# File nguồn
add_executable(redis
    src/main.c
    src/redis_utils.c
    src/analytics.c
    src/temp_storage.c
)

# Link libraries theo OS
if(APPLE OR UNIX)
    # macOS/Linux - chỉ cần hiredis
    target_link_libraries(redis ${HIREDIS_LIBRARY})
elseif(WIN32)
    # Windows - cần thêm ws2_32 (Windows socket library)
    target_link_libraries(redis ${HIREDIS_LIBRARY} ws2_32)
endif()
