#include <stdio.h>
#include "redis_utils.h"

redisContext* connect_redis() {
    redisContext* c = redisConnect("127.0.0.1", 6379);
    if (c == NULL || c->err) {
        printf("❌ Không thể kết nối Redis: %s\n", c ? c->errstr : "Không xác định");
        return NULL;
    }
    printf("✅ Kết nối Redis thành công!\n");
    return c;
}
