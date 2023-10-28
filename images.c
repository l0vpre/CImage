#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define MAX(a, b) a > b ? a : b

// Usage:
// ./images <input file> <output file> <operation flag> [kwargs]
// Example:
// ./images sample.png sample-brighter.png -B 1.5

// operations
// -i - negative
// -A - change alpha [float amount]
// -B - change brightness [float amount]
// -r - extract red
// -g - extract green
// -b - extract blue
// -a - extract alpha
// -G - greyscale

__uint32_t mult_channel(__uint32_t channel, float amount);


void image_inverse(int argc, char* argv[]);
void image_change_aplha(int argc, char* argv[]);
void image_change_brightness(int argc, char* argv[]);
void image_extract_red(int argc, char* argv[]);
void image_extract_green(int argc, char* argv[]);
void image_extract_blue(int argc, char* argv[]);
void image_extract_alpha(int argc, char* argv[]);
void image_greyscale(int argc, char* argv[]);

int width;
int height;
int channels = 4;
__uint32_t *image;

int main(int argc, char* argv[]) {
    if(argc < 4) {
        fprintf(stderr, "Arguments count must be 4 or greater\n");
        exit(1);
    }

    FILE* file = fopen(argv[1], "r");
    if(file == NULL) {
        fprintf(stderr, "Error opening file \"%s\"\n", argv[1]);
        exit(1);
    }

    image = (__uint32_t*)stbi_load_from_file(file, &width, &height, &channels, 4);
    if(image == NULL) {
        fprintf(stderr, "Error loading image\n");
        exit(1);
    }

    fclose(file);

    const char* flag = argv[3];

    if(strcmp(flag, "-i") == 0) {
        image_inverse(argc, argv);
    } else if(strcmp(flag, "-A") == 0) {
        image_change_aplha(argc, argv);
    } else if(strcmp(flag, "-B") == 0) {
        image_change_brightness(argc, argv);
    } else if (strcmp(flag, "-r") ==0) {
        image_extract_red(argc, argv);
    } else if (strcmp(flag, "-g") ==0) {
        image_extract_green(argc, argv);
    } else if (strcmp(flag, "-b") ==0) {
        image_extract_blue(argc, argv);
    } else if (strcmp(flag, "-a") ==0) {
        image_extract_alpha(argc, argv);
    } else if(strcmp(flag, "-G") == 0){
        image_greyscale(argc, argv);
    } else {
        fprintf(stderr, "Unknown flag \"%s\"", flag);
        exit(1);
    }

    int res = stbi_write_png(argv[2], width, height, channels, image, width * channels);

    if(res == 0) {
        fprintf(stderr, "Error writing image to \"%s\"", argv[2]);
        exit(1);
    }

    stbi_image_free(image);
    
    return 0;
}

__uint32_t mult_channel(__uint32_t channel, float amount) {
    channel = (__uint32_t) (channel * amount);
    if(channel > 0xff) {
        channel = 0xff;
    }
    
    return channel;
}


void image_inverse(int argc, char* argv[]) {
    for(__uint32_t index = 0; index < width * height; index++) {
        __uint32_t pixel = image[index];       
        __uint32_t color = pixel & 0x00ffffff;
        __uint32_t alpha = pixel & 0xff000000; 
        color = (~color) & 0x00ffffff;        
        image[index] = color | alpha;          
    }
}

void image_change_aplha(int argc, char* argv[]) {  
    if(argc < 5){
        fprintf(stderr, "Arguments count must be not less than 5\n");
        exit(1);
    }

    float change = atof(argv[4]);
    if(change < 0) {
        fprintf(stderr, "Amount must be positive\n");
        exit(1);
    }

    for(__uint32_t index = 0; index < width * height; index++) {
        __uint32_t pixel = image[index];
        __uint32_t alpha = (pixel & 0xff000000) >> 24;
        
        alpha = mult_channel(alpha, change);

        image[index] = (pixel & 0x00ffffff) | alpha << 24;
    }
}

void image_change_brightness(int argc, char* argv[]){
    if(argc < 5){
        fprintf(stderr, "Arguments count must be not less than 5\n");
        exit(1);
    }

    float change = atof(argv[4]);
    if(change < 0) {
        fprintf(stderr, "Amount must be positive\n");
        exit(1);
    }

    for(__uint32_t index = 0; index < width * height; index++) {
        __uint32_t pixel = image[index];
        __uint32_t alpha = pixel & 0xff000000;
        __uint32_t blue  = (pixel & 0x00ff0000) >> 16;
        __uint32_t green = (pixel & 0x0000ff00) >> 8;
        __uint32_t red   = (pixel & 0x000000ff);
        
        red = mult_channel(red, change);
        green = mult_channel(green, change);
        blue = mult_channel(blue, change);

        image[index] = alpha | (blue << 16) | (green << 8) | red;
    }
}

void image_extract_red(int argc, char* argv[]){
    for(__uint32_t index = 0; index < width * height; index++) {
        image[index] = image[index] & 0x000000ff | 0xff000000;
    }
}

void image_extract_green(int argc, char* argv[]){
    for(__uint32_t index = 0; index < width * height; index++) {
        image[index] = image[index] & 0x0000ff00 | 0xff000000;
    }
}

void image_extract_blue(int argc, char* argv[]){
    for(__uint32_t index = 0; index < width * height; index++) {
        image[index] = image[index] & 0x00ff0000 | 0xff000000;
    }
}

void image_extract_alpha(int argc, char* argv[]){
    for(__uint32_t index = 0; index < width * height; index++) {
        __uint32_t pixel = image[index];
        __uint32_t alpha = (pixel & 0xff000000) >> 24;
        image[index] = 0xff000000 | (alpha << 16) | (alpha << 8) | alpha;
    }
}

void image_greyscale(int argc, char* argv[]) {
    for(__uint32_t index = 0; index < width * height; index++) {
        __uint32_t pixel = image[index];
        __uint32_t alpha = pixel & 0xff000000;
        __uint32_t blue  = (pixel & 0x00ff0000) >> 16;
        __uint32_t green = (pixel & 0x0000ff00) >> 8;
        __uint32_t red   = (pixel & 0x000000ff);
        __uint32_t max_color = MAX(red, green);
        max_color = MAX(max_color, blue);
        image[index] = alpha | (max_color << 16) | (max_color << 8) | max_color;
    }

}
