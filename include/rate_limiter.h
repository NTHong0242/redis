#pragma once
#include <hiredis/hiredis.h>

// Cấu trúc lưu kết quả kiểm tra rate limit
typedef struct {
    int allowed;              // 1 nếu request được chấp nhận, 0 nếu bị từ chối
    int remaining;            // Số request còn lại trong window
    double retry_after;       // Thời gian (giây) cần đợi nếu bị từ chối
} RateLimitResult;

// Cấu hình rate limiter
typedef struct {
    int max_requests;         // Số request tối đa
    int window_seconds;       // Kích thước window (giây)
} RateLimitConfig;

// Kiểm tra rate limit cho một identifier
RateLimitResult check_rate_limit(redisContext* c, const char* identifier, RateLimitConfig config);

// Lấy timestamp hiện tại (milliseconds)
double get_current_timestamp();

// Tạo unique request ID
const char* generate_request_id();

// Demo rate limiter với các test cases
void perform_rate_limit_demo(redisContext* c);
