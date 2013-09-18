/* hw_client.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hw.h"

#define BUFLEN 200
#define TWO_WORD 0
#define ONE_WORD 1

void print_classes() {
  t_classes *classes = get_classes();
  int i;
  char **c_list = classes->classes;
  for (i = 0; i < classes->num; i++) {
    printf("%d: %s\n", i, *c_list++);
  }
}

int class_lookup(char *class) {
  t_classes *classes = get_classes();
  int i;
  char **current = classes->classes;
  for (i = 0; i < classes-> num; i++) {
    if (strcmp(*current, class) == 0) {
      return i;
    }
    current++;
  }
  return -1;
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

int do_add(char *buf) {
  int class_id, due_month, due_day, due_hour, due_min, ret;
  char *assignment = malloc(BUFLEN * sizeof(char));
  char *class_name = malloc(BUFLEN * sizeof(char));
  ret = sscanf(buf, "%s %s %d/%d %d:%d", class_name, assignment, &due_month,
      &due_day, &due_hour, & due_min);
  switch (ret) {
    case 5:
      due_min = 0;
    case 6:
      class_id = class_lookup(class_name);
      if (class_id < 0) {
        fprintf(stderr, "No such class: %s\n", class_name);
        return -1;
      }
      add_hw(class_id, assignment, due_month, due_day, due_hour, due_min);
      break;
    default:
      fprintf(stderr, "Could not parse %s\n", buf);
  }
  free(assignment);
  free(class_name);
  return 0;
}


int eval(char *buf1, char *buf2, char flag) {
  printf("%s\n", buf1);
  if (flag != 1) {
    printf("%s\n", buf2);
  }
  if (strcmp(buf1, "exit") == 0) {
    return -1;
  } else if (strcmp(buf1, "add") == 0) {
    if (flag == ONE_WORD) {
      return 1;
    }
    do_add(buf2);
    return 0;
  } else if (strcmp(buf1, "clear") == 0) {
    clear();
    return 0;
  } else if (strcmp(buf1, "list") == 0) {
    find_hw(7, &print_vals);
    return 0;
  } else {
    fprintf(stderr, "Unknown command %s\n", buf1);
    return 1;
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
    flag = TWO_WORD;
    while ((c = getchar()) != ' ' && i < BUFLEN - 1) {
      if (c == '\n') {
        flag = ONE_WORD;
        break;
      }
      buf1[i++] = c;
    }
    buf1[i] = '\0';
    i = 0;
    while (flag != ONE_WORD && (c = getchar()) != '\n' && i < BUFLEN - 1) {
      buf2[i++] = c;
    }
    buf2[i] = '\0';
    if (eval(buf1, buf2, flag) < 0) {
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
