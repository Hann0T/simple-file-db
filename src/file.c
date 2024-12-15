#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "include/common.h"
#include "include/file.h"

int create_db_file(char *file_name) {
  int fd = open(file_name, O_RDONLY);
  if (fd != -1) {
    printf("File already exists!\n");
    close(fd);
    return STATUS_ERROR;
  }

  fd = open(file_name, O_RDWR | O_CREAT, 0644);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }

  return fd;
}

int open_db_file(char *file_name) {
  int fd = open(file_name, O_RDWR);
  if (fd == -1) {
    perror("open");
    return STATUS_ERROR;
  }

  return fd;
}
