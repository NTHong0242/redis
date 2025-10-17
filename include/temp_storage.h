#pragma once
#include <hiredis/hiredis.h>

// Chức năng: lưu trữ tạm thời (dữ liệu tự xóa sau vài giây)
void perform_temporary_storage(redisContext* c);
