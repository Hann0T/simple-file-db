#ifndef PARSE_H
#define PARSE_H

#define HEADER_MAGIC 0x4c4c4144
#define HEADER_VERSION 1

typedef struct DBHeader {
  unsigned int magic;
  unsigned short version;
  unsigned short count;
  unsigned int filesize;
} dbheader_t;

typedef struct Employee {
  char name[256];
  char address[256];
  unsigned int hours;
} employee_t;

int create_db_header(int fd, dbheader_t **header_out);
int validate_db_header(int fd, dbheader_t **header_out);
int read_employees(int fd, dbheader_t **header_out, employee_t **employees_out);
int output_file(int fd, dbheader_t *header);

#endif
