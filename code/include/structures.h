#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <types.h>

typedef struct convolutionalLayer
{
  int input_channel;
  int filter_size;
  int output_channel;
  int padding;
  int stride;
  float * biases;
  float * filters;
  float ( * activation_function) (float);
  float ( * activation_function_prime) (float);
}
convolutionalLayer;

typedef struct poolingLayer
{
  int filter_size;
  int padding;
  int stride;
  int is_max_pooling;
}
poolingLayer;

typedef struct denseLayer
{
  int input_size;
  int output_size;
  float * biases;
  float * weights;
  float ( * activation_function) (float);
  float ( * activation_function_prime) (float);
}
denseLayer;

typedef struct dataset1D
{
  float * samples;
  int sample_count;
  int column_count;
}
dataset1D;

typedef struct dataset2D
{
  float * samples;
  int sample_count;
  int row_count;
  int column_count;
  int channel_count;
}
dataset2D;

typedef struct LeNet5
{
  struct convolutionalLayer c1;
  struct poolingLayer s2;
  struct convolutionalLayer c3;
  struct poolingLayer s4;
  struct denseLayer c5;
  struct denseLayer f6;
  struct denseLayer output;
}
LeNet5;

struct in_addr
{
  uint32_t s_addr;  // 32-bit IPv4 address in network byte order (big-endian)
};

struct sockaddr
{
  unsigned short sa_family;   // Address family (e.g., AF_INET for IPv4)
  char sa_data[14];           // Address data (implementation-specific)
};

struct sockaddr_in
{
  short            sin_family;   // Address family (AF_INET)
  unsigned short   sin_port;     // Port number (in network byte order)
  struct in_addr   sin_addr;     // IPv4 address
  unsigned char    sin_zero[8];  // Padding to make the structure the same size as sockaddr
};

struct stat {
  unsigned long st_dev;
  unsigned long st_ino;
  unsigned long st_nlink;
  unsigned int st_mode;
  unsigned int st_uid;
  unsigned int st_gid;
  unsigned long st_rdev;
  unsigned long st_size;
  unsigned long st_blksize;
  unsigned long st_blocks;
};

#endif