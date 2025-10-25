# =========================================
# run.ps1 - Build Hiredis static + Redis Project
# =========================================

# Đảm bảo PowerShell dùng UTF-8
chcp 65001 > $null

# Đường dẫn project
$projectRoot = "C:\udpt-v2\redis"
$hiredisDir = Join-Path $projectRoot "hiredis"
$hiredisBuildDir = Join-Path $hiredisDir "build"
$buildDir = Join-Path $projectRoot "build"

# =========================================
# 1️⃣ Build Hiredis static
# =========================================
Write-Host "`n====================="
Write-Host "🔹 Build Hiredis static..."
Write-Host "=====================`n"

# Xoá build cũ nếu có
if (Test-Path $hiredisBuildDir) { Remove-Item $hiredisBuildDir -Recurse -Force }
mkdir $hiredisBuildDir

Push-Location $hiredisBuildDir

# Generate MinGW Makefiles với static lib
cmake -G "MinGW Makefiles" -DBUILD_SHARED_LIBS=OFF ..
if ($LASTEXITCODE -ne 0) { Write-Host "❌ CMake generate Hiredis thất bại"; exit 1 }

mingw32-make
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Build Hiredis thất bại"; exit 1 }

Pop-Location

# Kiểm tra libhiredis.a
$libHiredis = Join-Path $hiredisBuildDir "libhiredis.a"
if (-not (Test-Path $libHiredis)) {
    Write-Host "`n❌ Không tìm thấy libhiredis.a trong $hiredisBuildDir"
    exit 1
}

# =========================================
# 2️⃣ Build Redis Project
# =========================================
Write-Host "`n====================="
Write-Host "🔹 Build Redis Project..."
Write-Host "=====================`n"

if (Test-Path $buildDir) { Remove-Item $buildDir -Recurse -Force }
mkdir $buildDir

Push-Location $buildDir

# Generate project, link Hiredis static
cmake -G "MinGW Makefiles" `
      -DHiredis_LIBRARY="$libHiredis" `
      -DHiredis_INCLUDE_DIR="$hiredisDir" ..
if ($LASTEXITCODE -ne 0) { Write-Host "❌ CMake generate Redis Project thất bại"; exit 1 }

# Biên dịch
mingw32-make
if ($LASTEXITCODE -ne 0) { Write-Host "❌ Build Redis Project thất bại"; exit 1 }

Pop-Location

# =========================================
# 3️⃣ Chạy redis.exe
# =========================================
$exe = Get-ChildItem -Path $buildDir -Recurse -Filter "redis.exe" | Select-Object -First 1
if ($null -eq $exe) {
    Write-Host "`n❌ Không tìm thấy redis.exe trong thư mục build!"
    exit 1
}

Write-Host "`n====================="
Write-Host "🚀 Chạy chương trình:"
Write-Host "=====================`n"

# Chạy exe trực tiếp, vì link static nên không cần DLL
& $exe.FullName
