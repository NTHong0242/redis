#pragma once
#include "hiredis.h"

// ===========================================
// 📦 TEMPORARY STORAGE MODULE (Redis-based)
// -------------------------------------------
// Chức năng: Lưu trữ tạm thời dữ liệu trong Redis,
// tự động xóa sau một khoảng thời gian (TTL).
// Dùng để mô phỏng cache, session, hay dữ liệu tạm.
// ===========================================

// Cấu hình cho bộ lưu trữ tạm thời
typedef struct {
    const char* key;          // Tên key trong Redis
    const char* value;        // Giá trị muốn lưu tạm
    int ttl_seconds;          // Thời gian tồn tại (TTL, giây)
} TempStorageConfig;

// Kết quả khi thao tác với Redis
typedef struct {
    int success;              // 1 nếu lưu thành công, 0 nếu lỗi
    const char* message;      // Mô tả trạng thái hoặc lỗi
} TempStorageResult;

// -------------------------------------------
// 🧩 Các hàm của module temp_storage
// -------------------------------------------

// Lưu dữ liệu tạm thời vào Redis (SET key value EX ttl)
TempStorageResult store_temporary_data(redisContext* c, TempStorageConfig config);

// Đọc dữ liệu tạm thời từ Redis (GET key)
TempStorageResult read_temporary_data(redisContext* c, const char* key);

// Demo lưu và đọc dữ liệu tạm thời, hiển thị trạng thái TTL
void perform_temporary_storage(redisContext* c);
