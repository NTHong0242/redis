cmake_minimum_required(VERSION 3.10)
project(redis C)

set(CMAKE_C_STANDARD 11)

# Include headers từ project
include_directories(${PROJECT_SOURCE_DIR}/include)

# Windows - using locally built hiredis
include_directories("${PROJECT_SOURCE_DIR}/hiredis")
link_directories("${PROJECT_SOURCE_DIR}/hiredis/build/Release")
set(HIREDIS_LIBRARY hiredis)

# File nguồn
add_executable(redis
    src/main.c
    src/redis_utils.c
    src/analytics.c
    src/temp_storage.c
    src/rate_limiter.c
)

# Link libraries - Windows socket library
target_link_libraries(redis ${HIREDIS_LIBRARY} ws2_32)
