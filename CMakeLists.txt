cmake_minimum_required(VERSION 3.10)
project(kaggle_downloader C)

# Find required packages
find_package(CURL REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONC REQUIRED json-c)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${CURL_INCLUDE_DIRS})
include_directories(${JSONC_INCLUDE_DIRS})

# Create library
add_library(kaggle_downloader SHARED
    src/kaggle_downloader.c
    src/curl_utils.c
)

# Link libraries
target_link_libraries(kaggle_downloader
    ${CURL_LIBRARIES}
    ${JSONC_LIBRARIES}
    ${OPENSSL_LIBRARIES}
)

# Create example executable
add_executable(download_example examples/download_example.c)
target_link_libraries(download_example kaggle_downloader)

# Create test executable
add_executable(test_downloader tests/test_downloader.c)
target_link_libraries(test_downloader kaggle_downloader)