#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/kaggle_downloader.h"

void test_initialization() {
    printf("Testing initialization...\n");

    // Test with NULL parameters
    assert(kaggle_init(NULL, NULL) == KAGGLE_ERROR_AUTH);

    // Test with valid parameters
    assert(kaggle_init("test_user", "test_key") == KAGGLE_SUCCESS);

    kaggle_cleanup();
    printf("Initialization tests passed!\n");
}

void test_download_path() {
    printf("Testing download path...\n");

    // Initialize first
    assert(kaggle_init("test_user", "test_key") == KAGGLE_SUCCESS);

    // Test with NULL path
    assert(kaggle_set_download_path(NULL) == KAGGLE_ERROR_INVALID_DATASET);

    // Test with valid path
    assert(kaggle_set_download_path("./test_downloads") == KAGGLE_SUCCESS);

    kaggle_cleanup();
    printf("Download path tests passed!\n");
}

void test_dataset_download() {
    printf("Testing dataset download...\n");

    const char* username = getenv("KAGGLE_USERNAME");
    const char* api_key = getenv("KAGGLE_KEY");

    if (!username || !api_key) {
        printf("Skipping download test - no credentials\n");
        return;
    }

    assert(kaggle_init(username, api_key) == KAGGLE_SUCCESS);
    assert(kaggle_set_download_path("./test_downloads") == KAGGLE_SUCCESS);

    char output_path[512];
    int result = kaggle_download_dataset("chetankv/dogs-cats-images", 
                                       output_path, 
                                       sizeof(output_path));

    assert(result == KAGGLE_SUCCESS);

    kaggle_cleanup();
    printf("Download tests passed!\n");
}

int main() {
    printf("Starting tests...\n");

    test_initialization();
    test_download_path();
    test_dataset_download();

    printf("All tests completed successfully!\n");
    return 0;
}