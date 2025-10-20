#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "rate_limiter.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <windows.h>

// Lấy timestamp hiện tại (giây với độ chính xác microsecond)
double get_current_timestamp() {
    FILETIME ft;
    ULARGE_INTEGER ui;
    GetSystemTimeAsFileTime(&ft);
    ui.LowPart = ft.dwLowDateTime;
    ui.HighPart = ft.dwHighDateTime;
    
    // Convert to Unix timestamp (seconds since 1970-01-01)
    // Windows FILETIME is 100-nanosecond intervals since 1601-01-01
    // Unix epoch starts 11644473600 seconds later
    return (double)(ui.QuadPart / 10000000ULL - 11644473600ULL) + 
           (double)(ui.QuadPart % 10000000ULL) / 10000000.0;
}

// Tạo unique request ID
const char* generate_request_id() {
    static char uuid_str[37];
    // Simple UUID replacement for Windows using random numbers and timestamp
    srand((unsigned int)time(NULL));
    snprintf(uuid_str, sizeof(uuid_str), 
             "%08x-%04x-%04x-%04x-%08x%04x",
             rand(), rand() & 0xffff, rand() & 0xffff,
             rand() & 0xffff, rand(), rand() & 0xffff);
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

// Mô phỏng nhiều client đồng thời
void simulate_multiple_clients(redisContext* c, RateLimitConfig config) {
    if (!c) return;

    const char* clients[] = {"user_A", "user_B", "user_C"};
    int num_clients = 3;
    int stats_accepted[3] = {0, 0, 0};
    int stats_rejected[3] = {0, 0, 0};

    printf("\n------------------------------------------\n");
    printf("TEST 2: Mô phỏng nhiều client đồng thời\n");
    printf("------------------------------------------\n\n");

    // Mỗi client gửi 15 requests, xen kẽ nhau
    for (int round = 1; round <= 15; round++) {
        for (int client_idx = 0; client_idx < num_clients; client_idx++) {
            RateLimitResult result = check_rate_limit(c, clients[client_idx], config);
            
            if (result.allowed) {
                printf("[Client %s - Request %2d] ✓ ACCEPTED | Remaining: %d/%d\n",
                       clients[client_idx], round, result.remaining, config.max_requests);
                stats_accepted[client_idx]++;
            } else {
                printf("[Client %s - Request %2d] ✗ REJECTED | Rate limit exceeded\n",
                       clients[client_idx], round);
                stats_rejected[client_idx]++;
            }
            
            Sleep(500); // 0.5 giây delay giữa mỗi request
        }
        printf("\n");
    }

    printf("------------------------------------------\n");
    printf("Statistics:\n");
    for (int i = 0; i < num_clients; i++) {
        printf("- Client %s: %d accepted, %d rejected\n",
               clients[i], stats_accepted[i], stats_rejected[i]);
    }
    printf("------------------------------------------\n");
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

        Sleep(1000); // Windows Sleep takes milliseconds
    }

    printf("\n------------------------------------------\n");
    printf("Kết quả: %d accepted, %d rejected\n", accepted, rejected);
    printf("------------------------------------------\n");

    // Test 2: Multi-client scenario
    simulate_multiple_clients(c, config);

    printf("\n==========================================\n");
}
