#ifndef KAGGLE_DOWNLOADER_H
#define KAGGLE_DOWNLOADER_H

#include <stddef.h>

// Error codes
#define KAGGLE_SUCCESS 0
#define KAGGLE_ERROR_AUTH -1
#define KAGGLE_ERROR_NETWORK -2
#define KAGGLE_ERROR_INVALID_DATASET -3

// Main configuration structure
typedef struct {
    char* username;
    char* api_key;
    char* download_path;
} KaggleConfig;

// Initialize the Kaggle downloader with credentials
int kaggle_init(const char* username, const char* api_key);

// Set download directory
int kaggle_set_download_path(const char* path);

// Download a dataset
int kaggle_download_dataset(const char* dataset_path, char* output_path, size_t output_path_size);

// Cleanup resources
void kaggle_cleanup(void);

#endif