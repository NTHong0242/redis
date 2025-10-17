#include <stdio.h>
#include "temp_storage.h"

void perform_temporary_storage(redisContext* c) {
    if (!c) return;

    redisReply* reply = redisCommand(c, "SET temp_data 'Dữ liệu tạm thời' EX 10");
    if (reply) {
        printf("Đã lưu dữ liệu tạm thời (10 giây).\n");
        freeReplyObject(reply);
    } else {
        printf("Lỗi khi lưu dữ liệu tạm.\n");
    }
}
