#include <arpa/inet.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "include/common.h"
#include "include/file.h"
#include "include/parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <db_file>\n", argv[0]);
  printf("\t-n - create new db file\n");
  printf("\t-f - (required) path to db file\n");
  printf("\t-a - add employee to db\n");
  printf("\t-l - list all employees\n");
  printf("\t-i - find employee by name\n");
  return;
}

int main(int argc, char *argv[]) {
  int c;
  bool new_file = false;
  bool list = false;
  char *file_path = NULL;
  char *add_str = NULL;
  char *find = NULL;
  int dbfd = -1;
  dbheader_t *db_header = NULL;
  employee_t *employees = NULL;

  while ((c = getopt(argc, argv, "nf:a:li:")) != -1) {
    switch (c) {
    case 'n':
      new_file = true;
      break;
    case 'f':
      file_path = optarg;
      break;
    case 'a':
      add_str = optarg;
      break;
    case 'l':
      list = true;
      break;
    case 'i':
      find = optarg;
      break;
      break;
    case '?':
      printf("Unknown option -%c\n", c);
      break;
    default:
      return -1;
    }
  }

  if (file_path == NULL) {
    printf("db_file is a required argument!\n");
    print_usage(argv);
    return -1;
  }

  if (new_file) {
    dbfd = create_db_file(file_path);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to create the db file!\n");
      close(dbfd);
      return -1;
    }
    if (create_db_header(dbfd, &db_header) == STATUS_ERROR) {
      printf("Failed to create db header\n");
      return -1;
    }
  } else {
    dbfd = open_db_file(file_path);
    if (dbfd == STATUS_ERROR) {
      printf("Unable to open the db file!\n");
      close(dbfd);
      return -1;
    }
    if (validate_db_header(dbfd, &db_header) == STATUS_ERROR) {
      printf("Failed to validate database header\n");
      close(dbfd);
      return -1;
    }
  }

  if (read_employees(dbfd, db_header, &employees) != STATUS_SUCCESS) {
    printf("Failed to read employees\n");
    return -1;
  }

  if (add_str) {
    db_header->count++;
    employees = realloc(employees, db_header->count * sizeof(employee_t));
    if (employees == NULL) {
      perror("realloc");
      printf("Failed to realloc memory for new employee");
      return -1;
    }
    add_employee(db_header, employees, add_str);
  }

  if (list) {
    list_employees(db_header, employees);
  }

  if (find) {
    employee_t *employee = find_employee(db_header, employees, find);
    if (employee == NULL) {
      printf("No employee found\n");
    } else {
      printf("Employee found\n");
      printf("\tName: %s\n", employee->name);
      printf("\tAddress: %s\n", employee->address);
      printf("\tHours: %d\n", employee->hours);
      free(employee);
    }
  }

  if (output_file(dbfd, db_header, employees) == STATUS_ERROR) {
    printf("Failed to write to database\n");
    close(dbfd);
    free(db_header);
    return -1;
  }

  free(db_header);

  return 0;
}
