#include <stdio.h>
#include "analytics.h"

void perform_real_time_analytics(redisContext* c) {
    if (!c) return;

    redisReply* reply = redisCommand(c, "INCR page_views");
    if (reply) {
        printf("Lượt truy cập hiện tại: %lld\n", reply->integer);
        freeReplyObject(reply);
    } else {
        printf(" Không thể cập nhật thống kê.\n");
    }
}
