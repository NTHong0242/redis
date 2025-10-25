#pragma once
#include <hiredis.h>

// Kết nối tới Redis server
redisContext* connect_redis();
