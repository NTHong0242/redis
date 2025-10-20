# Redis Rate Limiting Demo

Hệ thống demo Rate Limiting phân tán sử dụng Redis với C/C++.

**Tính năng:**
- Giới hạn tốc độ truy cập phân tán (Distributed Rate Limiting)
- Sliding Window Counter algorithm
- Demo multi-client concurrent requests
- Hỗ trợ Windows

## Yêu cầu hệ thống

- **Windows 10/11**
- **Visual Studio 2022 Build Tools** (hoặc Visual Studio 2022)
- **CMake 3.10+**
- **Git**
- **Redis Server**

---

## 📦 Cài đặt Dependencies

### 1. Cài đặt Visual Studio 2022 Build Tools
### 2. Cài đặt CMake
**Kiểm tra:**
```powershell
cmake --version
# Output: cmake version 3.x.x

### 4. Cài đặt Redis Server

Đã có trong docker compose

Chạy docker compose up -d

redis-cli ping
# Output: PONG
```

---

## 🔧 Cài đặt Hiredis Library

Hiredis là C client library cho Redis. Project này sẽ build hiredis từ source.

### Bước 1: Clone Hiredis

```powershell
# Đảm bảo bạn đang ở thư mục project
cd C:\workspace\redis

# Clone hiredis repository
git clone https://github.com/redis/hiredis.git
```

### Bước 2: Build Hiredis

```powershell
# Tạo build directory
cd hiredis
mkdir build
cd build

# Generate build files với Visual Studio 2022
cmake .. -G "Visual Studio 17 2022"

# Build ở Release mode
cmake --build . --config Release

# Quay về thư mục project
cd ..\..
```

### Bước 3: Tạo thư mục header

```powershell
# Tạo thư mục hiredis/hiredis và copy headers
mkdir hiredis\hiredis
copy hiredis\*.h hiredis\hiredis\
```

**Kết quả:** sẽ có:
- `hiredis/build/Release/hiredis.dll` - DLL file
- `hiredis/build/Release/hiredis.lib` - Library file
- `hiredis/hiredis/*.h` - Header files

---

## 🚀 Build và Chạy Project

### Lần đầu tiên - Build toàn bộ

```powershell
# 1. Đảm bảo đang ở thư mục root của project
cd C:\workspace\redis

# 2. Tạo build directory (nếu chưa có)
mkdir build
cd build

# 3. Generate build files
cmake .. -G "Visual Studio 17 2022"

# 4. Build project
cmake --build . --config Release

# 5. Copy hiredis.dll vào thư mục executable
cd ..
copy hiredis\build\Release\hiredis.dll build\Release\

# 6. Chạy chương trình
.\run.ps1
```

### Khi sửa code - Rebuild nhanh

```powershell
# Build lại (chỉ compile file đã thay đổi)
cd build
cmake --build . --config Release
cd ..

# Chạy chương trình
.\run.ps1
```

### Khi thay đổi CMakeLists.txt - Rebuild từ đầu

```powershell
# Xóa build directory
cd build
Remove-Item -Recurse -Force *

# Generate lại và build
cmake .. -G "Visual Studio 17 2022"
cmake --build . --config Release

# Copy DLL
cd ..
copy hiredis\build\Release\hiredis.dll build\Release\

# Chạy
.\run.ps1
```

---

## 📝 Chạy chương trình

###  Sử dụng script run.ps1 (Khuyến nghị)

Script này tự động set UTF-8 encoding để hiển thị tiếng Việt đúng.

```powershell
.\run.ps1

