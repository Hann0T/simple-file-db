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
  return;
}

int main(int argc, char *argv[]) {
  int c;
  bool new_file = false;
  char *file_path = NULL;
  int dbfd = -1;
  dbheader_t *db_header = NULL;

  while ((c = getopt(argc, argv, "nf:")) != -1) {
    switch (c) {
    case 'n':
      new_file = true;
      break;
    case 'f':
      file_path = optarg;
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

  if (output_file(dbfd, db_header) == STATUS_ERROR) {
    printf("Failed to write to database\n");
    close(dbfd);
    free(db_header);
    return -1;
  }

  printf("Header written to database\n");
  free(db_header);

  return 0;
}
