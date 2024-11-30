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

static KaggleConfig config = {0};

int kaggle_init(const char* username, const char* api_key) {
    if (!username || !api_key) {
        fprintf(stderr, "Username or API key is NULL\n");
        return KAGGLE_ERROR_AUTH;
    }

    config.username = strdup(username);
    config.api_key = strdup(api_key);
    config.download_path = strdup("./downloads");

    curl_global_init(CURL_GLOBAL_DEFAULT);
    return KAGGLE_SUCCESS;
}

int kaggle_set_download_path(const char* path) {
    if (!path) {
        fprintf(stderr, "Download path is NULL\n");
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    free(config.download_path);
    config.download_path = strdup(path);

    if (create_directory(config.download_path) != 0) {
        fprintf(stderr, "Failed to create download directory\n");
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    return KAGGLE_SUCCESS;
}

int kaggle_download_dataset(const char* dataset_path, char* output_path, size_t output_path_size) {
    if (!dataset_path || !config.username || !config.api_key) {
        fprintf(stderr, "Invalid parameters for download\n");
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    if (create_directory(config.download_path) != 0) {
        fprintf(stderr, "Failed to create download directory\n");
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    CURL* curl = curl_easy_init();
    if (!curl) {
        fprintf(stderr, "Failed to initialize CURL\n");
        return KAGGLE_ERROR_NETWORK;
    }

    // Create auth string
    char auth_str[512];
    snprintf(auth_str, sizeof(auth_str), "%s:%s", config.username, config.api_key);
    char* auth_encoded = base64_encode(auth_str);
    if (!auth_encoded) {
        curl_easy_cleanup(curl);
        fprintf(stderr, "Failed to encode authentication\n");
        return KAGGLE_ERROR_AUTH;
    }

    // Construct auth header
    char auth_header[1024];
    snprintf(auth_header, sizeof(auth_header), "Authorization: Basic %s", auth_encoded);

    // Create output file path
    char output_file[512];
    const char* dataset_name = strrchr(dataset_path, '/') ? strrchr(dataset_path, '/') + 1 : dataset_path;
    snprintf(output_file, sizeof(output_file), "%s/%s.zip", config.download_path, dataset_name);

    FILE* fp = fopen(output_file, "wb");
    if (!fp) {
        fprintf(stderr, "Failed to create output file: %s\n", output_file);
        free(auth_encoded);
        curl_easy_cleanup(curl);
        return KAGGLE_ERROR_INVALID_DATASET;
    }

    // Construct API URL
    char url[512];
    snprintf(url, sizeof(url), 
             "https://www.kaggle.com/api/v1/datasets/download/%s",
             dataset_path);

    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, auth_header);
    headers = curl_slist_append(headers, "Content-Type: application/json");

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_file_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    CURLcode res = curl_easy_perform(curl);

    fclose(fp);
    curl_slist_free_all(headers);
    free(auth_encoded);

    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    curl_easy_cleanup(curl);

    if (res != CURLE_OK) {
        fprintf(stderr, "Curl failed: %s\n", curl_easy_strerror(res));
        return KAGGLE_ERROR_NETWORK;
    }

    if (http_code != 200) {
        fprintf(stderr, "HTTP error: %ld\n", http_code);
        return KAGGLE_ERROR_NETWORK;
    }

    strncpy(output_path, output_file, output_path_size);
    return KAGGLE_SUCCESS;
}

void kaggle_cleanup(void) {
    free(config.username);
    free(config.api_key);
    free(config.download_path);
    curl_global_cleanup();
}