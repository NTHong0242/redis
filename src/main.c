#include <stdio.h>
#include "redis_utils.h"
#include "analytics.h"
#include "temp_storage.h"
int main() {
    redisContext* c = connect_redis();
    if (!c) return -1;

    // printf("\n--- Chức năng 1: Thống kê thời gian thực ---\n");
    // perform_real_time_analytics(c);


    printf("\n--- Chức năng 1: Rate Limiting Phân Tán ---\n");
    perform_rate_limit_demo(c);

    printf("\n--- Chức năng 2: Lưu trữ tạm thời ---\n");
    perform_temporary_storage(c);

    redisFree(c);
    return 0;
}
