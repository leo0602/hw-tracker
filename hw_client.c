/* hw_client.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw.h"

#define BUFLEN 200

void print_classes() {
  t_classes *classes = get_classes();
  int i;
  char **c_list = classes->classes;
  for (i = 0; i < classes->num; i++) {
    printf("%d: %s\n", i, *c_list++);
  }
}

int print_vals(void *data, int argc, char **argv, char **col_name) {
  printf("--------------------------------\n");
  while (argc-- > 0) {
    if (strcmp(*col_name, "COMPLETED") == 0) {
      if (**argv == '0') {
        printf("%s: %s\n", *col_name++, "FALSE");
      } else if (**argv == '1') {
        printf("%s: %s\n", *col_name++, "TRUE");
      } else {
        printf("Error on column %s with value %s\n", *col_name++, *argv);
      }
    } else {
      printf("%s: %s\n", *col_name++, *argv ? *argv : "NULL");
    }
    argv++;
  }
  return 0;
}

void eval(char *buf1, char *buf2, char flag) {
    printf("%s\n", buf1);
    if (flag != 1) {
      printf("%s\n", buf2);
    }
}

void repl() {
  char buf1[BUFLEN];
  char buf2[BUFLEN];
  char c, flag;
  int i;
  while (1) {
    printf("> ");
    i = 0;
    flag = 0;
    while ((c = getchar()) != ' ' && i < BUFLEN - 1) {
      if (c == '\n') {
        flag = 1;
        break;
      }
      buf1[i++] = c;
    }
    buf1[i] = '\0';
    i = 0;
    while (flag != 1 && (c = getchar()) != '\n' && i < BUFLEN - 1) {
      buf2[i++] = c;
    }
    buf2[i] = '\0';
    eval(buf1, buf2, flag);

    if (strcmp(buf1, "exit") == 0) {
      break;
    }
  }
}

int main(int argc, char *argv[]) {
  repl();
  exit(0);
  initialize();
  print_classes();
  add_hw(0, "HW1", 9, 4, 9, 0);
  mark_completed(0, "HW1");
  cleanup();
  get_assignments(0, &print_vals);
  finish();
  exit(0);
  return 0;
}
