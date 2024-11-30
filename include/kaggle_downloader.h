#ifndef KAGGLE_DOWNLOADER_H
#define KAGGLE_DOWNLOADER_H

#ifdef __cplusplus
extern "C" {
#endif

// Error codes
#define KAGGLE_SUCCESS             0
#define KAGGLE_ERROR_AUTH         -1
#define KAGGLE_ERROR_NETWORK      -2
#define KAGGLE_ERROR_INVALID_DATASET -3

// Configuration structure
typedef struct {
    char* username;
    char* api_key;
    char* download_path;
} KaggleConfig;

// Main API functions
int kaggle_init(const char* username, const char* api_key);
int kaggle_set_download_path(const char* path);
int kaggle_download_dataset(const char* dataset_path, char* output_path, size_t output_path_size);
void kaggle_cleanup(void);

#ifdef __cplusplus
}
#endif

#endif // KAGGLE_DOWNLOADER_H