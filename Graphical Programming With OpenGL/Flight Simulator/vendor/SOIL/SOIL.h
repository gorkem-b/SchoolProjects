#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#define SOIL_LOAD_AUTO 0

unsigned char* SOIL_load_image(
    const char* filename,
    int* width,
    int* height,
    int* channels,
    int force_channels
);

void SOIL_free_image_data(unsigned char* img_data);

const char* SOIL_last_result(void);

#ifdef __cplusplus
}
#endif