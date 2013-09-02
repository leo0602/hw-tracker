/* hw.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>

#define MAX_CLASSES 10
#define CLASS_LEN 50

typedef struct ClassID {
  int id;
  char *name;
} ClassID;

char classes[MAX_CLASSES][CLASS_LEN];
int num_classes = 0;

static int print_vals(void *data, int argc, char **argv, char **col_name) {
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
  printf("\n");
  return 0;
}

static void do_sql(sqlite3 *db, char *sql_cmd) {
  int err_no;
  char *err_msg = NULL;
  err_no = sqlite3_exec(db, sql_cmd, &print_vals, NULL, &err_msg);
  if (err_no != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
  }
}

static void initialize() {
  FILE *fp = fopen("classes", "r");
  int i;
  if (fp == NULL) {
    fprintf(stderr, "Initialization failed: couldn't read classes");
    exit(1);
  }
  for (i = 0; i < MAX_CLASSES && fgets(classes[i], CLASS_LEN, fp) != NULL; i++) {
    num_classes++;
    printf("%d: %s", i, classes[i]);
  }
}

int main(int argc, char *argv[]) {
  initialize();
  sqlite3 *db;
  int err_no;
  char *sql_cmd;
  err_no = sqlite3_open("hw.db", &db);
  if (err_no) {
    fprintf(stderr, "Open failed: %s", sqlite3_errmsg(db));
  }
  sql_cmd = "CREATE TABLE IF NOT EXISTS HW"\
            "(CLASS_ID INT, CLASS TEXT, ASSIGNMENT TEXT,"\
            "DUE DATETIME, COMPLETED INT);";
  do_sql(db, sql_cmd);
  sql_cmd = "INSERT INTO HW VALUES (0, 'CS 70', 'HW0', '2013-09-03', 0);";
  do_sql(db, sql_cmd);
  sql_cmd = "SELECT * FROM HW WHERE DUE > '2013-09-02';";
  do_sql(db, sql_cmd);
  sqlite3_close(db);
  return 0;
}
