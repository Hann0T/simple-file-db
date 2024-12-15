#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "include/common.h"
#include "include/parse.h"

int output_file(int fd, dbheader_t *header, employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad fd from the user\n");
    return STATUS_ERROR;
  }

  int employee_count = header->count;
  header->magic = htonl(header->magic);
  header->filesize =
      htonl(sizeof(dbheader_t) + (sizeof(employee_t) * employee_count));
  header->count = htons(header->count);
  header->version = htons(header->version);

  lseek(fd, 0, SEEK_SET);

  write(fd, header, sizeof(dbheader_t));

  for (int i = 0; i < employee_count; i++) {
    employees[i].hours = htonl(employees[i].hours);
    write(fd, &employees[i], sizeof(employee_t));
  }

  return STATUS_SUCCESS;
}

int create_db_header(int fd, dbheader_t **header_out) {
  dbheader_t *header = calloc(1, sizeof(dbheader_t));
  if (header == NULL) {
    printf("Malloc failed to allocate memory for db header\n");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(dbheader_t);

  *header_out = header;

  return STATUS_SUCCESS;
}

int validate_db_header(int fd, dbheader_t **header_out) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return STATUS_ERROR;
  }

  dbheader_t *header = calloc(1, sizeof(dbheader_t));
  if (header == NULL) {
    printf("Malloc failed to allocate memory for db header\n");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(dbheader_t)) != sizeof(dbheader_t)) {
    perror("read");
    free(header);
    return STATUS_ERROR;
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if (header->magic != HEADER_MAGIC) {
    printf("Improper header magic\n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->version != HEADER_VERSION) {
    printf("Improper header version\n");
    free(header);
    return STATUS_ERROR;
  }

  struct stat db_stat = {0};
  fstat(fd, &db_stat);

  if (header->filesize != db_stat.st_size) {
    printf("Corrupted database\n");
    free(header);
    return STATUS_ERROR;
  }

  *header_out = header;
  return STATUS_SUCCESS;
}

int read_employees(int fd, dbheader_t *header, employee_t **employees_out) {
  if (fd < 0) {
    printf("Got a bad FD from the user\n");
    return STATUS_ERROR;
  }
  int count = header->count;
  employee_t *employees = calloc(count, sizeof(employee_t));
  if (employees == NULL) {
    printf("Calloc failed\n");
    return STATUS_ERROR;
  }
  read(fd, employees, count * sizeof(employee_t));

  for (int i = 0; i < count; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employees_out = employees;
  return STATUS_SUCCESS;
}

int add_employee(dbheader_t *header, employee_t *employees, char *add_str) {
  printf("Adding a new employee %s\n", add_str);
  char *name = strtok(add_str, ",");
  char *addr = strtok(NULL, ",");
  char *hours = strtok(NULL, ",");

  printf("%s %s %s\n", name, addr, hours);

  int index = header->count - 1;

  strncpy(employees[index].name, name, sizeof(employees[index].name));
  strncpy(employees[index].address, addr, sizeof(employees[index].address));
  employees[index].hours = atoi(hours);

  return STATUS_SUCCESS;
}

void list_employees(dbheader_t *header, employee_t *employees) {
  for (int i = 0; i < header->count; i++) {
    printf("Employee: %d\n", i);
    printf("\tName: %s\n", employees[i].name);
    printf("\tName: %s\n", employees[i].address);
    printf("\tName: %d\n", employees[i].hours);
  }
}

employee_t *find_employee(dbheader_t *header, employee_t *employees,
                          char *needle) {
  employee_t *filter_employee = malloc(sizeof(employee_t));

  int employees_len = header->count;
  for (int i = 0; i < employees_len; i++) {
    if (strncmp(employees[i].name, needle, sizeof(employees[i].name)) == 0) {
      strncpy(filter_employee->name, employees[i].name,
              sizeof(filter_employee->name));
      strncpy(filter_employee->address, employees[i].address,
              sizeof(filter_employee->address));
      filter_employee->hours = employees[i].hours;
      return filter_employee;
    }
  }

  return NULL;
}

//
//
//
//
//
//
//
//
//
//
// why it cant be a pointer and not a pointer pointer
int create_db_header_bad(int fd, dbheader_t *header_out) {
  dbheader_t *header = calloc(1, sizeof(dbheader_t));
  if (header == NULL) {
    printf("Malloc failed to allocate memory for db header\n");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(dbheader_t);

  // the problem starts here

  // BAD
  header_out = header;
  // if we set the value of the header_out to the *header
  // then we are doing nothing, because the *header_out
  // is a copy

  // BAD
  *header_out = *header;
  // if we dealocate we get the dbheader_t value from *header_out
  // then we dealocate the *header
  // so at the end the only thing that was done here is a copy
  // a copy of dbheader_t
  // so for this solution I doesn't make any sense to allocate memory on the
  // heap

  return 0;
}
