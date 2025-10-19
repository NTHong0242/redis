#pragma once
#include <hiredis/hiredis.h>

// Chức năng: thống kê thời gian thực
void perform_real_time_analytics(redisContext* c);
