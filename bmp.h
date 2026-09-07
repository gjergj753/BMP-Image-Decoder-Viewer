#ifndef BMP_H
#define BMP_H

#include <stdint.h>

#define BMP_FILE_HEADER_SIZE_IN_BYTES 14
#define BMP_INFO_HEADER_SIZE_IN_BYTES 40

#pragma pack(push, 1) // removes struct padding so we can read bmp files correcly
typedef struct
{

    uint16_t signature;         // used to identify the BMP and DIB file is 0x42 0x4D in hexadecimal
    uint32_t file_size;         // The size of the BMP file in bytes
    uint16_t reserved1;         // Reserved; actual value depends on the application that creates the image, if created manually can be 0
    uint16_t reserved2;         // Reserved; actual value depends on the application that creates the image, if created manually can be 0
    uint32_t pixel_data_offset; // The offset, i.e. starting address, of the byte where the bitmap image data (pixel array) can be found.

} bitmap_file_header;

typedef struct
{
    
    uint32_t header_size;
    uint32_t width;  // image width in pixels
    uint32_t height; // image height in pixels
    uint16_t planes;
    uint16_t bits_per_pixel;
    uint32_t compression;
    uint32_t image_size; // image size in bytes
    uint32_t x_pixels_per_m;
    uint32_t y_pixels_per_m;
    uint32_t colors_in_color_table;
    uint32_t important_colors;

} bitmap_info_header;
#pragma pack(pop)// re-enables padding

typedef struct
{

    bitmap_file_header *file_header;
    bitmap_info_header *info_header;
    // colortable
    unsigned char *pixels;
    uint32_t row_size;
    uint32_t width_in_bytes;
    uint32_t padding_size_in_bytes; // in bytes
    uint32_t bytes_per_pixel;
    uint32_t *bitmap_pixels;

} bmp_image;

bmp_image* read_bmp(const char *filename);

int write_bmp(const char *filename, const bmp_image *bmp_image);

void free_bmp(bmp_image *image);

#endif //BMP_H
