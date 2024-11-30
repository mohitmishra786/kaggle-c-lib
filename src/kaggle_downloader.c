#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include <sys/stat.h>
#include "../include/kaggle_downloader.h"

// External functions from curl_utils.c
extern int create_directory(const char* path);
extern size_t write_file_callback(void* ptr, size_t size, size_t nmemb, void* stream);
extern char* base64_encode(const char* input);

// Progress callback data structure
struct ProgressData {
    curl_off_t last_print_percent;
    FILE* output_file;
};

static KaggleConfig config = {0};

// Progress callback function
static int progress_callback(void* clientp, curl_off_t dltotal, curl_off_t dlnow, curl_off_t ultotal, curl_off_t ulnow) {
    struct ProgressData* prog = (struct ProgressData*)clientp;

    if (dltotal <= 0) return 0;

    curl_off_t percent = (dlnow * 100) / dltotal;

    // Update progress every 1%
    if (percent > prog->last_print_percent) {
        printf("\rDownload progress: %3ld%%", percent);
        fflush(stdout);
        prog->last_print_percent = percent;

        if (percent == 100) {
            printf("\nDownload completed!\n");
        }
    }

    return 0;
}

static char* sanitize_dataset_path(const char* dataset_path) {
    if (!dataset_path) return NULL;

    // Remove leading/trailing whitespace
    while (*dataset_path == ' ') dataset_path++;

    // Copy and clean the path
    char* cleaned = strdup(dataset_path);
    size_t len = strlen(cleaned);

    // Remove trailing whitespace and slashes
    while (len > 0 && (cleaned[len-1] == ' ' || cleaned[len-1] == '/')) {
        cleaned[--len] = '\0';
    }

    // Ensure path has owner/dataset format
    if (!strchr(cleaned, '/')) {
        free(cleaned);
        return NULL;
    }

    return cleaned;
}

int kaggle_init(const char* username, const char* api_key) {
    if (!username || !api_key) {
        fprintf(stderr, "Error: Username or API key is NULL\n");
        return KAGGLE_ERROR_AUTH;
    }

    config.username = strdup(username);
    config.api_key = strdup(api_key);
    config.download_path = strdup("./downloads");

    curl_global_init(CURL_GLOBAL_DEFAULT);

    printf("Kaggle downloader initialized successfully\n");
    return KAGGLE_SUCCESS;
}

int kaggle_set_download_path(const char* path) {
    if (!path) {
        fprintf(stderr, "Error: Download path is NULL\n");
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    free(config.download_path);
    config.download_path = strdup(path);

    if (create_directory(config.download_path) != 0) {
        fprintf(stderr, "Error: Failed to create download directory: %s\n", path);
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    printf("Download path set to: %s\n", path);
    return KAGGLE_SUCCESS;
}

int kaggle_download_dataset(const char* dataset_path, char* output_path, size_t output_path_size) {
    char* cleaned_path = sanitize_dataset_path(dataset_path);
    if (!cleaned_path) {
        fprintf(stderr, "Error: Invalid dataset path format. Must be 'owner/dataset-name'\n");
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    if (!config.username || !config.api_key) {
        fprintf(stderr, "Error: Library not initialized. Call kaggle_init first\n");
        free(cleaned_path);
        return KAGGLE_ERROR_AUTH;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Error: Failed to initialize CURL\n");
        free(cleaned_path);
        return KAGGLE_ERROR_NETWORK;
    }

    // Create auth string
    char auth_str[512];
    snprintf(auth_str, sizeof(auth_str), "%s:%s", config.username, config.api_key);
    char* auth_encoded = base64_encode(auth_str);
    if (!auth_encoded) {
        fprintf(stderr, "Error: Failed to encode authentication\n");
        free(cleaned_path);
        curl_easy_cleanup(curl);
        return KAGGLE_ERROR_AUTH;
    }

    // Construct auth header
    char auth_header[1024];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Basic %s", auth_encoded);

    // Create output file path
    char output_file[512];
    const char* dataset_name = strrchr(cleaned_path, '/') ? strrchr(cleaned_path, '/') + 1 : cleaned_path;
    snprintf(output_file, sizeof(output_file), "%s/%s.zip", config.download_path, dataset_name);

    FILE* fp = fopen(output_file, "wb");
    if (!fp) {
        fprintf(stderr, "Error: Failed to create output file: %s\n", output_file);
        free(cleaned_path);
        free(auth_encoded);
        curl_easy_cleanup(curl);
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    // Initialize progress data
    struct ProgressData prog = {0};
    prog.output_file = fp;

    // Construct API URL
    char url[512];
    snprintf(url, sizeof(url), 
             "https://www.kaggle.com/api/v1/datasets/download/%s",
             cleaned_path);

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, auth_header);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);
    curl_easy_setopt(curl, CURLOPT_XFERINFOFUNCTION, progress_callback);
    curl_easy_setopt(curl, CURLOPT_XFERINFODATA, &prog);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    printf("Starting download of dataset: %s\n", cleaned_path);
    CURLcode res = curl_easy_perform(curl);

    fclose(fp);
    curl_slist_free_all(headers);
    free(auth_encoded);
    free(cleaned_path);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Error: Download failed: %s\n", curl_easy_strerror(res));
        return KAGGLE_ERROR_NETWORK;
    }

    if (http_code != 200) {
        fprintf(stderr, "Error: HTTP error %ld\n", http_code);
        return KAGGLE_ERROR_NETWORK;
    }

    strncpy(output_path, output_file, output_path_size);
    printf("Dataset downloaded successfully to: %s\n", output_path);
    return KAGGLE_SUCCESS;
}

void kaggle_cleanup(void) {
    free(config.username);
    free(config.api_key);
    free(config.download_path);
    curl_global_cleanup();
    printf("Kaggle downloader cleaned up successfully\n");
}