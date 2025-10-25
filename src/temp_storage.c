#include <stdio.h>
#include <stdlib.h>
#include "temp_storage.h"

void perform_temporary_storage(redisContext* c) {
    if (!c) {
        printf("❌ Lỗi: Không có kết nối Redis\n");
        return;
    }

    // Ghi dữ liệu tạm vào Redis (tự xóa sau 10 giây)
    redisReply* reply = redisCommand(c, "SET temp_data 'Dữ liệu tạm thời' EX 10");
    if (!reply) {
        printf("❌ Lỗi: Không thể gửi lệnh SET tới Redis\n");
        return;
    }

    if (reply->type == REDIS_REPLY_STATUS && strcmp(reply->str, "OK") == 0) {
        printf("✅ Đã lưu dữ liệu tạm thời vào Redis (tồn tại trong 10 giây).\n");
    }
    else {
        printf("⚠️ Redis phản hồi khác thường (type=%d): %s\n",
            reply->type, reply->str ? reply->str : "(null)");
    }

    freeReplyObject(reply);
}
