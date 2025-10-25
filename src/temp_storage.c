#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // sleep()
#include "temp_storage.h"

void perform_temporary_storage(redisContext* c) {
    if (!c) {
        printf("❌ Lỗi: Không có kết nối Redis\n");
        return;
    }

    // --------- Định nghĩa key và giá trị ----------
    const char* key10 = "temp_10s";
    const char* value10 = "Dữ liệu 10 giây";

    const char* key30 = "temp_30s";
    const char* value30 = "Dữ liệu 30 giây";

    // --------- Lưu dữ liệu tạm thời ----------
    redisReply* reply = redisCommand(c, "SET %s %s EX 10", key10, value10);
    if (!reply) { printf("❌ Lỗi SET %s\n", key10); return; }
    freeReplyObject(reply);

    reply = redisCommand(c, "SET %s %s EX 30", key30, value30);
    if (!reply) { printf("❌ Lỗi SET %s\n", key30); return; }
    freeReplyObject(reply);

    printf("✅ Đã lưu dữ liệu tạm thời: %s (10s), %s (30s)\n", key10, key30);

    // --------- Kiểm tra sau 10s ----------
    sleep(10);
    printf("\n⏳ Kiểm tra sau 10 giây:\n");

    reply = redisCommand(c, "GET %s", key10);
    if (reply->type == REDIS_REPLY_NIL) printf("✅ %s đã hết hạn\n", key10);
    else printf("⚠️ %s vẫn tồn tại: %s\n", key10, reply->str);
    freeReplyObject(reply);

    reply = redisCommand(c, "GET %s", key30);
    if (reply->type == REDIS_REPLY_NIL) printf("⚠️ %s đã hết hạn sớm?\n", key30);
    else printf("✅ %s vẫn tồn tại: %s\n", key30, reply->str);
    freeReplyObject(reply);

    // --------- Kiểm tra sau 30s ----------
    sleep(20); // thêm 20s nữa để tổng = 30s
    printf("\n⏳ Kiểm tra sau 30 giây:\n");

    reply = redisCommand(c, "GET %s", key10);
    if (reply->type == REDIS_REPLY_NIL) printf("✅ %s vẫn hết hạn\n", key10);
    else printf("⚠️ %s vẫn tồn tại: %s\n", key10, reply->str);
    freeReplyObject(reply);

    reply = redisCommand(c, "GET %s", key30);
    if (reply->type == REDIS_REPLY_NIL) printf("✅ %s đã hết hạn\n", key30);
    else printf("⚠️ %s vẫn tồn tại: %s\n", key30, reply->str);
    freeReplyObject(reply);
}
