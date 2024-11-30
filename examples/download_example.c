#include <stdio.h>
#include <stdlib.h>
#include "kaggle_downloader.h"

int main() {
    const char* username = getenv("KAGGLE_USERNAME");
    const char* api_key = getenv("KAGGLE_KEY");

    if (!username || !api_key) {
        fprintf(stderr, "Please set KAGGLE_USERNAME and KAGGLE_KEY environment variables\n");
        return 1;
    }

    printf("Initializing Kaggle downloader...\n");
    if (kaggle_init(username, api_key) != KAGGLE_SUCCESS) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }

    printf("Setting download path...\n");
    if (kaggle_set_download_path("./downloads") != KAGGLE_SUCCESS) {
        fprintf(stderr, "Failed to set download path\n");
        kaggle_cleanup();
        return 1;
    }

    printf("Starting download...\n");
    char output_path[512];
    int result = kaggle_download_dataset("chetankv/dogs-cats-images", 
                                       output_path, 
                                       sizeof(output_path));

    if (result == KAGGLE_SUCCESS) {
        printf("Dataset downloaded successfully to: %s\n", output_path);
    } else {
        fprintf(stderr, "Failed to download dataset: error code %d\n", result);
    }

    kaggle_cleanup();
    return (result == KAGGLE_SUCCESS) ? 0 : 1;
}