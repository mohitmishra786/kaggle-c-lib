#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <sys/stat.h>
#include <errno.h>

// Create directory if it doesn't exist
int create_directory(const char* path) {
    struct stat st = {0};
    if (stat(path, &st) == -1) {
#ifdef _WIN32
        return _mkdir(path);
#else
        return mkdir(path, 0700);
#endif
    }
    return 0;
}

// Callback function for writing directly to file
size_t write_file_callback(void* ptr, size_t size, size_t nmemb, void* stream) {
    FILE* file = (FILE*)stream;
    size_t written = fwrite(ptr, size, nmemb, file);
    return written;
}

// Base64 encode function
char* base64_encode(const char* input) {
    static const char base64_chars[] = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    size_t input_len = strlen(input);
    size_t output_len = 4 * ((input_len + 2) / 3);
    char* encoded = (char*)malloc(output_len + 1);

    if (encoded == NULL) return NULL;

    size_t i = 0, j = 0;
    unsigned char char_array_3[3];
    unsigned char char_array_4[4];

    while (input_len--) {
        char_array_3[i++] = *(input++);
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for(i = 0; i < 4; i++)
                encoded[j++] = base64_chars[char_array_4[i]];
            i = 0;
        }
    }

    if (i) {
        size_t k;
        for(k = i; k < 3; k++)
            char_array_3[k] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

        for (k = 0; k < i + 1; k++)
            encoded[j++] = base64_chars[char_array_4[k]];

        while(i++ < 3)
            encoded[j++] = '=';
    }

    encoded[j] = '\0';
    return encoded;
}