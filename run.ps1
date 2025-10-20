# Script to run Redis project with proper UTF-8 encoding
# Set console to UTF-8 for proper Vietnamese character display
chcp 65001 > $null

# Run the program
.\build\Release\redis.exe
