#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <SDL3/SDL.h>
#include "bmp.h"

/*
    TODO: Find a way to make it for multiple header sizes
        - Add compression
        - add color table
*/

bmp_image* read_bmp(const char *filename) {

    FILE *image_pointer = fopen(filename, "rb");

    if (image_pointer == NULL) {
        perror("Error");
        return NULL;
    }

    bitmap_file_header *file_header = malloc(sizeof(bitmap_file_header));
    bitmap_info_header *info_header = malloc(sizeof(bitmap_info_header));


    // Read the File Header of the Bitmap
    fread(file_header, BMP_FILE_HEADER_SIZE_IN_BYTES, 1, image_pointer);

    // Read the info header of the bitmap
    fread(info_header, BMP_INFO_HEADER_SIZE_IN_BYTES, 1, image_pointer);

    // Set the image pointer to where the BMP pixel array starts
    fseek(image_pointer, file_header->pixel_data_offset, SEEK_SET);

    const uint32_t row_size = ((info_header->width * info_header->bits_per_pixel + 31) / 32) * 4;

    const uint32_t width_in_bytes = (info_header->width * info_header->bits_per_pixel) / 8;

    const uint32_t padding_size = row_size - width_in_bytes;

    const uint32_t bytes_per_pixel = info_header->bits_per_pixel / 8;

    const size_t pixel_data_size = (size_t)row_size * info_header->height;

    unsigned char *pixels = malloc(pixel_data_size);

    if (pixels == NULL)
    {
        fprintf(stderr, "Failed to allocate pixel buffer\n");
        fclose(image_pointer);
        return NULL;
    }

    const size_t bytes_read =
        fread(pixels, 1, pixel_data_size, image_pointer);

    if (bytes_read != pixel_data_size)
    {
        fprintf(stderr,
                "Expected %zu bytes of pixel data but read %zu\n",
                pixel_data_size,
                bytes_read);

        free(pixels);
        fclose(image_pointer);
        return NULL;
    }



    const size_t total_pixels = (size_t)info_header->width * (size_t)info_header->height;

    uint32_t *bitmap_pixels = malloc(total_pixels * sizeof(uint32_t));

    if (bitmap_pixels == NULL)
    {
        fprintf(stderr, "Failed to allocate bitmap pixel buffer\n");
        free(pixels);
        fclose(image_pointer);
        return NULL;
    }

    /* TODO:
        - Build the compatibility with other color levels (bits per pixel), like 16 bit, 8 bit, etc.
        */

    switch (info_header->bits_per_pixel) {
        case 8:
            for (uint32_t row = 0; row < info_header->height; row++)
            {
                for (uint32_t col = 0; col < info_header->width; col++)
                {
                    // Find this pixel inside the BMP pixel buffer
                    const unsigned char *pixel = pixels + row * row_size + col * bytes_per_pixel;

                    // BMP stores 24-bit pixels as B G R
                    const uint32_t blue  = pixel[0];
                    const uint32_t green = pixel[0];
                    const uint32_t red   = pixel[0];

                    const uint32_t result = (blue << 16) | (green << 8) | red;

                    // Standard BMP pixel rows are stored bottom-to-top
                    const uint32_t out_row = info_header->height - 1 - row;

                    const uint32_t out_index = out_row * info_header->width + col;

                    bitmap_pixels[out_index] = result;
                }
            }
            break;
        case 16:
            break;
        case 24:
            for (uint32_t row = 0; row < info_header->height; row++)
            {
                for (uint32_t col = 0; col < info_header->width; col++)
                {
                    // Find this pixel inside the BMP pixel buffer
                    const unsigned char *pixel = pixels + row * row_size + col * bytes_per_pixel;

                    // BMP stores 24-bit pixels as B G R
                    const uint32_t blue  = pixel[0];
                    const uint32_t green = pixel[1];
                    const uint32_t red   = pixel[2];

                    const uint32_t result = (blue << 16) | (green << 8) | red;

                    // Standard BMP pixel rows are stored bottom-to-top
                    const uint32_t out_row = info_header->height - 1 - row;

                    const uint32_t out_index = out_row * info_header->width + col;

                    bitmap_pixels[out_index] = result;
                }
            }
            break;
        case 32:
            for (uint32_t row = 0; row < info_header->height; row++)
            {
                for (uint32_t col = 0; col < info_header->width; col++)
                {
                    // Find this pixel inside the BMP pixel buffer
                    const unsigned char *pixel = pixels + row * row_size + col * bytes_per_pixel;

                    // BMP stores 24-bit pixels as B G R
                    const uint32_t blue  = pixel[0];
                    const uint32_t green = pixel[1];
                    const uint32_t red   = pixel[2];

                    const uint32_t result = (blue << 16) | (green << 8) | red;

                    // Standard BMP pixel rows are stored bottom-to-top
                    const uint32_t out_row = info_header->height - 1 - row;

                    const uint32_t out_index = out_row * info_header->width + col;

                    bitmap_pixels[out_index] = result;
                }
            }
            break;
        default: ;
    }

    bmp_image *image = malloc(sizeof(bmp_image));

    image->file_header = file_header;
    image->info_header = info_header;



    image->pixels = pixels;
    image->row_size = row_size;
    image->width_in_bytes = width_in_bytes;
    image->padding_size_in_bytes = padding_size;
    image->bytes_per_pixel = bytes_per_pixel;
    image->bitmap_pixels = bitmap_pixels;

    printf("Row Size: %d\n", image->row_size);

    printf("Width In bytes: %d\n", image->width_in_bytes);
    printf("Padding Size: %d\n", image->padding_size_in_bytes);
    printf("Bytes Per Pixel: %d\n", image->bytes_per_pixel);

    // display file header
    printf("%x ", image->file_header->signature);
    printf("%d ", image->file_header->file_size);
    printf("%d ", image->file_header->reserved1);
    printf("%d ", image->file_header->reserved2);
    printf("%d ", image->file_header->reserved2);
    printf("%04x \n", image->file_header->pixel_data_offset);

    // display info header
    printf("header_size: %d \n", image->info_header->header_size);
    printf("width: %d \n", image->info_header->width);
    printf("height: %d \n", image->info_header->height);
    printf("planes: %d \n", image->info_header->planes);
    printf("bits_per_pixel: %d \n", image->info_header->bits_per_pixel);
    printf("compression: %d \n", image->info_header->compression);
    printf("image_size: %d \n", image->info_header->image_size);
    printf("x_pixels_per_m: %d \n", image->info_header->x_pixels_per_m);
    printf("y_pixels_per_m: %d \n", image->info_header->y_pixels_per_m);
    printf("colors_in_color_table: %d \n", image->info_header->colors_in_color_table);
    printf("important_colors: %d \n\n", image->info_header->important_colors);


    fclose(image_pointer);
    return image;


}

int write_bmp(const char *filename, const bmp_image *bmp_image) {

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Event event;

    window = SDL_CreateWindow(filename,bmp_image->info_header->width,bmp_image->info_header->height,0);

    if (window == NULL)
    {
        SDL_Log("Failed to create window: %s", SDL_GetError());
        return EXIT_FAILURE;
    }

    renderer = SDL_CreateRenderer(window, NULL);

    if (renderer == NULL)
    {
        SDL_Log("Failed to create renderer: %s", SDL_GetError());

        SDL_DestroyWindow(window);

        return EXIT_FAILURE;
    }

    texture = SDL_CreateTexture(renderer,SDL_PIXELFORMAT_XBGR8888,SDL_TEXTUREACCESS_STREAMING,bmp_image->info_header->width,bmp_image->info_header->height
    );

    if (texture == NULL)
    {
        SDL_Log("Failed to create texture: %s", SDL_GetError());

        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);

        return EXIT_FAILURE;
    }


    /*
        Get the ID of THIS BMP window.

        We use this to know when the user clicks
        the X specifically on this window.
    */
    SDL_WindowID window_id = SDL_GetWindowID(window);


    uint8_t is_running = 1;

    while (is_running)
    {
        /*
            Poll events
        */
        while (SDL_PollEvent(&event))
        {
            /*
                User clicked X on this image window.
            */
            if (
                event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == window_id
            )
            {
                is_running = 0;
            }
        }


        /*
            SAME DISPLAY CODE YOU ALREADY HAD
        */

        SDL_UpdateTexture(texture, NULL, bmp_image->bitmap_pixels, bmp_image->info_header->width * sizeof(uint32_t)
        );


        /*
            Display the window and renderer
        */

        // SDL_RenderClear(renderer);

        SDL_RenderTexture(renderer, texture,NULL,NULL);

        SDL_RenderPresent(renderer);
    }


    /*
        Window has been closed.
    */

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);


    return EXIT_SUCCESS;

    // SDL_Window *window;
    // SDL_Renderer *renderer;
    // SDL_Texture *texture;
    // SDL_Event event;
    //
    // SDL_Init(SDL_INIT_VIDEO);
    //
    // window = SDL_CreateWindow("Image Processor", bmp_image->info_header->width, bmp_image->info_header->height, 0);
    //
    // renderer = SDL_CreateRenderer(window, NULL);
    //
    // texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_XBGR8888, SDL_TEXTUREACCESS_STREAMING, bmp_image->info_header->width, bmp_image->info_header->height);
    //
    // uint8_t is_running = 1;
    // while (is_running)
    // {
    //
    //     /*Poll the events*/
    //     while (SDL_PollEvent(&event))
    //     {
    //         if (event.type == SDL_EVENT_QUIT)
    //         {
    //             // TODO: stop running
    //             is_running = 0;
    //         }
    //     }
    //
    //     SDL_UpdateTexture(texture, NULL, bmp_image->bitmap_pixels, bmp_image->info_header->width * sizeof(uint32_t));
    //
    //     /* Display the window and the renderer*/
    //     // SDL_RenderClear(renderer);
    //     SDL_RenderTexture(renderer, texture, NULL, NULL);
    //     SDL_RenderPresent(renderer);
    // }
    //
    // SDL_DestroyTexture(texture);
    // SDL_DestroyRenderer(renderer);
    // SDL_DestroyWindow(window);
    // SDL_Quit();
    //
    // return EXIT_SUCCESS;

}

void free_bmp(bmp_image *image) {
    free(image->bitmap_pixels);
    free(image->pixels);
    free(image->file_header);
    free(image->info_header);
    free(image);

}

