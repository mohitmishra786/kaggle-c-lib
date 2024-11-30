# Kaggle Dataset Downloader (libkaggle)

A C library for downloading datasets from Kaggle programmatically. This library provides a simple and efficient interface to download any public dataset from Kaggle using their API.

## Features

- ✨ Download any public dataset from Kaggle via command line
- 🚀 Simple and intuitive C API
- 📊 Real-time progress tracking
- ⚡ Efficient error handling
- 💻 Cross-platform support (Linux, macOS, Windows)

## 🔮 Roadmap

Future plans include:
- [ ] Dataset search functionality
- [ ] Competition dataset support
- [ ] Async download support
- [ ] Dataset metadata retrieval
- [ ] Custom SSL certificate support

## Prerequisites

Before using the library, ensure you have:

1. A Kaggle account and API credentials
2. C compiler (gcc, clang, or MSVC)
3. CMake (version 3.10 or higher)
4. Required development libraries:
   - libcurl
   - json-c
   - OpenSSL

### Installing Dependencies

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install build-essential cmake libcurl4-openssl-dev libjson-c-dev libssl-dev
```

#### macOS
```bash
brew install cmake curl json-c openssl
```

## Installation

### From Source

```bash
# Clone the repository
git clone https://github.com/yourusername/kaggle-c-lib.git
cd kaggle-c-lib

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build
make

# Install (optional)
sudo make install
```

### Using as a Library in Your Project

#### Method 1: CMake FetchContent
```cmake
include(FetchContent)

FetchContent_Declare(
    kaggle_downloader
    GIT_REPOSITORY https://github.com/yourusername/kaggle-c-lib.git
    GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(kaggle_downloader)

# Link against the library
target_link_libraries(your_project PRIVATE kaggle_downloader)
```

#### Method 2: System-wide Installation
After installing the library system-wide, in your CMakeLists.txt:
```cmake
find_package(kaggle_downloader REQUIRED)
target_link_libraries(your_project PRIVATE kaggle_downloader)
```

## Usage

### 1. Setting up Kaggle Credentials

Before using the library, set up your Kaggle API credentials:

1. Go to your [Kaggle account settings](https://www.kaggle.com/account)
2. Scroll to "API" section and click "Create New API Token"
3. This will download a `kaggle.json` file
4. Set environment variables:
```bash
export KAGGLE_USERNAME="your-username"
export KAGGLE_KEY="your-api-key"
```

### 2. Command Line Usage

```bash
# Download a single dataset
./download_example "jocelyndumlao/impulse-buying-factors-on-tiktok-shop"

# Download multiple datasets
./test_downloader "dataset1/name" "dataset2/name" "dataset3/name"
```

### 3. API Usage Example

```c
#include <stdio.h>
#include <stdlib.h>
#include <kaggle_downloader.h>

int main() {
    // Initialize with Kaggle credentials
    if (kaggle_init(getenv("KAGGLE_USERNAME"), 
                    getenv("KAGGLE_KEY")) != KAGGLE_SUCCESS) {
        return 1;
    }

    // Set download directory
    kaggle_set_download_path("./downloads");

    // Download a dataset
    char output_path[512];
    int result = kaggle_download_dataset(
        "jocelyndumlao/impulse-buying-factors-on-tiktok-shop", 
        output_path, 
        sizeof(output_path)
    );

    if (result == KAGGLE_SUCCESS) {
        printf("Dataset downloaded to: %s\n", output_path);
    }

    kaggle_cleanup();
    return result == KAGGLE_SUCCESS ? 0 : 1;
}
```

### 4. Compiling Your Program

```bash
# With pkg-config
gcc -o my_program my_program.c $(pkg-config --cflags --libs kaggle_downloader)

# Manual linking
gcc -o my_program my_program.c -lkaggle_downloader -lcurl -ljson-c
```

## API Reference

### Functions

#### `int kaggle_init(const char* username, const char* api_key)`
Initialize the library with Kaggle credentials.
- Returns: `KAGGLE_SUCCESS` on success, error code otherwise

#### `int kaggle_set_download_path(const char* path)`
Set the directory where datasets will be downloaded.
- Returns: `KAGGLE_SUCCESS` on success, error code otherwise

#### `int kaggle_download_dataset(const char* dataset_path, char* output_path, size_t output_path_size)`
Download a dataset from Kaggle.
- Parameters:
  - `dataset_path`: Format "owner/dataset-name"
  - `output_path`: Buffer to store the downloaded file path
  - `output_path_size`: Size of the output buffer
- Returns: `KAGGLE_SUCCESS` on success, error code otherwise

#### `void kaggle_cleanup()`
Clean up resources used by the library.

### Error Codes
```c
#define KAGGLE_SUCCESS             0
#define KAGGLE_ERROR_AUTH         -1
#define KAGGLE_ERROR_NETWORK      -2
#define KAGGLE_ERROR_INVALID_DATASET -3
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## Versioning

We use [SemVer](http://semver.org/) for versioning. For available versions, see the [tags on this repository](https://github.com/yourusername/kaggle-c-lib/tags).

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments

- Kaggle for providing the API
- libcurl developers
- json-c developers


To make the library installable system-wide, add these to your **CMakeLists.txt**:

```cmake
# Add installation targets
install(TARGETS kaggle_downloader
    EXPORT kaggle_downloader-targets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(FILES include/kaggle_downloader.h
    DESTINATION include
)

# Generate and install package configuration files
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/kaggle_downloader-config-version.cmake"
    VERSION 1.0.0
    COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/kaggle_downloader-config.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/kaggle_downloader-config.cmake"
    INSTALL_DESTINATION lib/cmake/kaggle_downloader
)

install(EXPORT kaggle_downloader-targets
    FILE kaggle_downloader-targets.cmake
    NAMESPACE kaggle::
    DESTINATION lib/cmake/kaggle_downloader
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/kaggle_downloader-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/kaggle_downloader-config-version.cmake"
    DESTINATION lib/cmake/kaggle_downloader
)
```

Create a new file **cmake/kaggle_downloader-config.cmake.in**:
```cmake
@PACKAGE_INIT@

include("${CMAKE_CURRENT_LIST_DIR}/kaggle_downloader-targets.cmake")
check_required_components(kaggle_downloader)
```

To create a release:
1. Update version numbers in CMakeLists.txt
2. Commit all changes
3. Tag the release
4. Create a GitHub release
5. Build binary packages:
```bash
# In build directory
cpack -G DEB  # For Debian/Ubuntu
cpack -G RPM  # For Red Hat/Fedora
```

This will create installable packages for different platforms.

Remember to:
1. Update CHANGELOG.md for each release
2. Keep documentation up to date
3. Test on different platforms before releasing
4. Include example programs in the release
5. Provide clear installation instructions for each platform