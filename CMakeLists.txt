cmake_minimum_required(VERSION 3.10)
project(redis C)

set(CMAKE_C_STANDARD 11)

# Include headers
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    "G:/hiredis"
    "G:/hiredis/hiredis"
)

# Link thư viện hiredis đã build
link_directories("G:/hiredis/build")

# File nguồn
add_executable(redis
    src/main.c
    src/redis_utils.c
    src/analytics.c
    src/temp_storage.c
)

# Liên kết hiredis + ws2_32 (socket lib)
target_link_libraries(redis hiredis ws2_32)
