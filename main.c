#include <stdio.h>
#include <math.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "lib/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "lib/stb_image_write.h"

typedef struct {
  unsigned char * bytes;
  const char * path;
  int width;
  int height;
  int channels;
} Image;

void cv_load_image(Image* image) {
  image->bytes = (unsigned char *)stbi_load(image->path, &(image->width), &(image->height), &(image->channels), 0);
  if(image->bytes == NULL) {
    printf("Error in loading the image\n");
    exit(1);
  }
}

void cv_write_image(Image* image, const char * name) {
  if (stbi_write_png(name, image->width, image->height, image->channels, image->bytes, 0) == 0) {
    printf("Error writing PNG file.\n");
    exit(1);
  }
}

void cv_compute_gaussian_kernel(float *** kernel, int sigma, int size) {
  float sum = 0.0;

  *kernel = (float**)malloc(sizeof(float*) * size);
  for (int i = 0; i < size; i++) {
    (*kernel)[i] = (float*)malloc(sizeof(float) * size);
    for (int j = 0; j < size; j++) {
        int y =  i - size / 2, x = j - size / 2;
        (*kernel)[i][j] = (1/(2 * M_PI * pow(sigma, 2))) * exp(-(x * x + y * y)/(2*pow(sigma, 2)));
        sum += (*kernel)[i][j];
    }
  }

  for (int i = 0; i < size; i++) {
    for (int j = 0; j < size; j++) {
        (*kernel)[i][j] /= sum;
    }
  }
}


void cv_apply_gaussian_blur(Image *image, float sigma) {
  int SIZE = 9;
  float **kernel;

  cv_compute_gaussian_kernel(&kernel, sigma, SIZE);

  unsigned char *imageBytes = image->bytes;

  unsigned char *newImageBytes = malloc(image->height * image->width * image->channels * sizeof(unsigned char));

  for (int i = 0; i < image->height; i++) {
    for (int j = 0; j < image->width; j++) {
      for (int c = 0; c < image->channels; c++) { // Iterate over the channels
        float sum = 0.0, sumWeight = 0.0;

        for (int k = 0; k < SIZE; k++) {
          for (int l = 0; l < SIZE; l++) {
            float weight = kernel[k][l];

            int yIndex = i + k - SIZE / 2;
            int xIndex = j + l - SIZE / 2;

            if (yIndex >= 0 && yIndex < image->height && xIndex >= 0 && xIndex < image->width) {
              int pixelIndex = (yIndex * image->width + xIndex) * image->channels + c;
              unsigned char pixelValue = imageBytes[pixelIndex];

              sum += pixelValue * weight;
              sumWeight += weight;
            }
          }
        }

        int pixelIndex = (i * image->width + j) * image->channels + c;
        newImageBytes[pixelIndex] = (unsigned char)(sum / sumWeight);
      }
    }
  }

  memcpy(image->bytes, newImageBytes, image->height * image->width * image->channels * sizeof(unsigned char));

  for (int i = 0; i < SIZE; i++) {
    free(kernel[i]);
  }
  free(kernel);
  free(newImageBytes);
}


void cv_free_image(Image * image) {
  free(image->bytes);
}

int main(void) {
  Image img = { .path = "another.png" };
  cv_load_image(&img);

  cv_apply_gaussian_blur(&img, 5);


  cv_write_image(&img, "output.png");

  cv_free_image(&img);
  return 0;
}
