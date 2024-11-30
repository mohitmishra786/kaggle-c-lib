cmake_minimum_required(VERSION 3.10)
project(kaggle_downloader C)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Find required packages
find_package(CURL REQUIRED)
find_package(JSON-C REQUIRED)

# Add library
add_library(kaggle_downloader
    src/kaggle_downloader.c
    src/curl_utils.c
)

# Include directories
target_include_directories(kaggle_downloader
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CURL_INCLUDE_DIRS}
        ${JSON-C_INCLUDE_DIRS}
)

# Link libraries
target_link_libraries(kaggle_downloader
    PUBLIC
        ${CURL_LIBRARIES}
        ${JSON-C_LIBRARIES}
)

# Add example executable
add_executable(download_example examples/download_example.c)
target_link_libraries(download_example kaggle_downloader)

# Add test executable
add_executable(test_downloader tests/test_downloader.c)
target_link_libraries(test_downloader kaggle_downloader)

# Installation rules
install(TARGETS kaggle_downloader
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
install(FILES include/kaggle_downloader.h
    DESTINATION include
)

# Create downloads directory if it doesn't exist
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/downloads)