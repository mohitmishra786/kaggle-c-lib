#include <stdio.h>
#include <stdlib.h>
#include "kaggle_downloader.h"

void print_usage(const char* program_name) {
    printf("Usage: %s <dataset_path1> [dataset_path2 ...]\n", program_name);
    printf("Example: %s jocelyndumlao/impulse-buying-factors-on-tiktok-shop chetankv/dogs-cats-images\n", 
           program_name);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    const char* username = getenv("KAGGLE_USERNAME");
    const char* api_key = getenv("KAGGLE_KEY");

    if (!username || !api_key) {
        fprintf(stderr, "Please set KAGGLE_USERNAME and KAGGLE_KEY environment variables\n");
        return 1;
    }

    if (kaggle_init(username, api_key) != KAGGLE_SUCCESS) {
        fprintf(stderr, "Failed to initialize\n");
        return 1;
    }

    if (kaggle_set_download_path("./downloads") != KAGGLE_SUCCESS) {
        fprintf(stderr, "Failed to set download path\n");
        kaggle_cleanup();
        return 1;
    }

    int success_count = 0;
    for (int i = 1; i < argc; i++) {
        printf("\nTesting dataset: %s\n", argv[i]);
        char output_path[512];
        int result = kaggle_download_dataset(argv[i], output_path, sizeof(output_path));

        if (result == KAGGLE_SUCCESS) {
            printf("Success: Downloaded to %s\n", output_path);
            success_count++;
        } else {
            fprintf(stderr, "Failed: Error code %d\n", result);
        }
    }

    kaggle_cleanup();
    printf("\nTest summary: %d/%d successful downloads\n", success_count, argc - 1);
    return (success_count == argc - 1) ? 0 : 1;
}