#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <uuid/uuid.h>
#include "rate_limiter.h"

// Lấy timestamp hiện tại (giây với độ chính xác microsecond)
double get_current_timestamp() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec / 1000000.0;
}

// Tạo unique request ID
const char* generate_request_id() {
    static char uuid_str[37];
    uuid_t uuid;
    uuid_generate(uuid);
    uuid_unparse(uuid, uuid_str);
    return uuid_str;
}

// Kiểm tra rate limit cho một identifier (sử dụng Sliding Window Counter)
RateLimitResult check_rate_limit(redisContext* c, const char* identifier, RateLimitConfig config) {
    RateLimitResult result = {0, 0, 0.0};
    
    if (!c || !identifier) {
        printf("❌ Lỗi: Redis context hoặc identifier không hợp lệ\n");
        return result;
    }

    // Tạo Redis key cho identifier này
    char redis_key[256];
    snprintf(redis_key, sizeof(redis_key), "rate_limit:%s", identifier);

    // 1. Lấy timestamp hiện tại
    double now = get_current_timestamp();
    double window_start = now - config.window_seconds;

    // 2. Xóa các request cũ hơn window (Redis ZREMRANGEBYSCORE)
    redisReply* reply = redisCommand(c, 
        "ZREMRANGEBYSCORE %s -inf %.6f", 
        redis_key, window_start);
    
    if (!reply) {
        printf("❌ Lỗi: Không thể xóa request cũ từ Redis\n");
        return result;
    }
    freeReplyObject(reply);

    // 3. Đếm số request hiện tại trong window (Redis ZCARD)
    reply = redisCommand(c, "ZCARD %s", redis_key);
    if (!reply) {
        printf("❌ Lỗi: Không thể đếm request từ Redis\n");
        return result;
    }
    
    long long current_count = reply->integer;
    freeReplyObject(reply);

    // 4. Kiểm tra xem có vượt limit không
    if (current_count >= config.max_requests) {
        // Bị từ chối - tìm thời gian cần đợi
        reply = redisCommand(c, "ZRANGE %s 0 0 WITHSCORES", redis_key);
        if (reply && reply->type == REDIS_REPLY_ARRAY && reply->elements >= 2) {
            double oldest_timestamp = atof(reply->element[1]->str);
            result.retry_after = (oldest_timestamp + config.window_seconds) - now;
            if (result.retry_after < 0) result.retry_after = 0;
        }
        freeReplyObject(reply);
        
        result.allowed = 0;
        result.remaining = 0;
        return result;
    }

    // 5. Request được chấp nhận - thêm vào Redis (Redis ZADD)
    const char* request_id = generate_request_id();
    reply = redisCommand(c, 
        "ZADD %s %.6f %s", 
        redis_key, now, request_id);
    
    if (!reply) {
        printf("❌ Lỗi: Không thể thêm request vào Redis\n");
        return result;
    }
    freeReplyObject(reply);

    // 6. Set TTL cho key để tự động xóa (Redis EXPIRE)
    reply = redisCommand(c, "EXPIRE %s %d", redis_key, config.window_seconds + 10);
    if (reply) {
        freeReplyObject(reply);
    }

    // Trả về kết quả thành công
    result.allowed = 1;
    result.remaining = config.max_requests - current_count - 1;
    result.retry_after = 0.0;
    
    return result;
}

// Thực hiện demo các tính năng rate limiter
void perform_rate_limit_demo(redisContext* c) {
    if (!c) {
        printf("❌ Lỗi: Không có kết nối Redis\n");
        return;
    }

    printf("\n==========================================\n");
    printf("    DEMO RATE LIMITING PHÂN TÁN\n");
    printf("==========================================\n\n");

    // Cấu hình: 10 requests trong 60 giây
    RateLimitConfig config = {
        .max_requests = 10,
        .window_seconds = 60
    };

    const char* user_id = "user_123";

    printf("Cấu hình:\n");
    printf("- Giới hạn: %d requests trong %d giây\n", config.max_requests, config.window_seconds);
    printf("- Client ID: %s\n", user_id);
    printf("- Thuật toán: Sliding Window Counter\n\n");

    printf("------------------------------------------\n");
    printf("TEST: Gửi 15 requests liên tiếp\n");
    printf("------------------------------------------\n\n");

    int accepted = 0;
    int rejected = 0;

    for (int i = 1; i <= 15; i++) {
        RateLimitResult result = check_rate_limit(c, user_id, config);
        
        if (result.allowed) {
            printf("[Request %2d] ✓ ACCEPTED | Remaining: %d/%d\n", 
                   i, result.remaining, config.max_requests);
            accepted++;
        } else {
            printf("[Request %2d] ✗ REJECTED | Rate limit exceeded | Retry after: %.0f seconds\n", 
                   i, result.retry_after);
            rejected++;
        }
    }

    printf("\n------------------------------------------\n");
    printf("Kết quả: %d accepted, %d rejected\n", accepted, rejected);
    printf("==========================================\n");
}
