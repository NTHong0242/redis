cmake_minimum_required(VERSION 3.10)
project(redis_project C)

set(CMAKE_C_STANDARD 11)

# -----------------------------------------
# Đường dẫn Hiredis static
# -----------------------------------------
set(Hiredis_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/hiredis")
set(Hiredis_LIBRARY "${CMAKE_SOURCE_DIR}/hiredis/build/libhiredis.a")

# -----------------------------------------
# Include headers
# -----------------------------------------
include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${Hiredis_INCLUDE_DIR}
)

# -----------------------------------------
# Thêm file source
# -----------------------------------------
add_executable(redis
    src/main.c
    src/rate_limiter.c
    src/analytics.c
    src/temp_storage.c
    src/redis_utils.c
)

# -----------------------------------------
# Link Hiredis static + Winsock (Windows)
# -----------------------------------------
target_link_libraries(redis
    ${Hiredis_LIBRARY}
    ws2_32
)
