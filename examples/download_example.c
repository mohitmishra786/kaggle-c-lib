#include <stdio.h>
#include <stdlib.h>
#include "kaggle_downloader.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <dataset_path>\n", program_name);
    printf("Example: %s jocelyndumlao/impulse-buying-factors-on-tiktok-shop\n", program_name);
    printf("         %s chetankv/dogs-cats-images\n", program_name);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* dataset_path = argv[1];
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

    printf("Starting download of dataset: %s\n", dataset_path);
    char output_path[512];
    int result = kaggle_download_dataset(dataset_path, output_path, sizeof(output_path));

    if (result == KAGGLE_SUCCESS) {
        printf("Dataset downloaded successfully to: %s\n", output_path);
    } else {
        fprintf(stderr, "Failed to download dataset: error code %d\n", result);
    }

    kaggle_cleanup();
    return (result == KAGGLE_SUCCESS) ? 0 : 1;
}