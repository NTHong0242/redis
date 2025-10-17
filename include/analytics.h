#pragma once
#include <hiredis.h>

// Chức năng: thống kê thời gian thực
void perform_real_time_analytics(redisContext* c);
