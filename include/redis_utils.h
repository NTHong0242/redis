#pragma once
#include <hiredis/hiredis.h>

// Kết nối tới Redis server
redisContext* connect_redis();
