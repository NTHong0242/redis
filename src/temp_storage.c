#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      // sleep()
#include "temp_storage.h"

void perform_temporary_storage(redisContext* c) {
    if (!c) {
        printf(" Lỗi: Không có kết nối Redis\n");
        return;
    }

    // --------- Định nghĩa key và giá trị ----------
    const char* key10 = "temp_10s";
    const char* value10 = "Dữ liệu 10 giây";

    const char* key20 = "temp_30s";
    const char* value20 = "Dữ liệu 30 giây";

    // --------- Lưu dữ liệu tạm thời ----------
    redisReply* reply = redisCommand(c, "SET %s %s EX 10", key10, value10);
    if (!reply) { printf(" Lỗi SET %s\n", key10); return; }
    freeReplyObject(reply);

    reply = redisCommand(c, "SET %s %s EX 20", key20, value20);
    if (!reply) { printf(" Lỗi SET %s\n", key20); return; }
    freeReplyObject(reply);

    printf(" Đã lưu dữ liệu tạm thời: %s (10s), %s (20s)\n", key10, key20);

    // --------- Kiểm tra sau 10s ----------
    sleep(10);
    printf("\n Kiểm tra sau 10 giây:\n");

    reply = redisCommand(c, "GET %s", key10);
    if (reply->type == REDIS_REPLY_NIL) printf(" %s đã hết hạn\n", key10);
    else printf(" %s vẫn tồn tại: %s\n", key10, reply->str);
    freeReplyObject(reply);

    reply = redisCommand(c, "GET %s", key20);
    if (reply->type == REDIS_REPLY_NIL) printf(" %s đã hết hạn sớm?\n", key20);
    else printf(" %s vẫn tồn tại: %s\n", key20, reply->str);
    freeReplyObject(reply);

    // --------- Kiểm tra sau 30s ----------
    sleep(10); // thêm 20s nữa để tổng = 30s
    printf("\n Kiểm tra sau 30 giây:\n");

    reply = redisCommand(c, "GET %s", key10);
    if (reply->type == REDIS_REPLY_NIL) printf(" %s vẫn hết hạn\n", key10);
    else printf(" %s vẫn tồn tại: %s\n", key10, reply->str);
    freeReplyObject(reply);

    reply = redisCommand(c, "GET %s", key20);
    if (reply->type == REDIS_REPLY_NIL) printf(" %s đã hết hạn\n", key20);
    else printf(" %s vẫn tồn tại: %s\n", key20, reply->str);
    freeReplyObject(reply);
}
