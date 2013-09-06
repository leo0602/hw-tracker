/* hw.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <time.h>
#include "hw.h"

#define MAX_CLASSES 10
#define CLASS_LEN 50
#define CMD_SIZE 200
#define DATE_LEN 30


t_classes all_classes;
char classes[MAX_CLASSES][CLASS_LEN];
int num_classes = 0;
sqlite3 *db;
char *sql_cmd;

static void do_sql(char *sql, int (*callback)(void *, int, char **, char **)) {
  int err_no;
  char *err_msg = NULL;
  err_no = sqlite3_exec(db, sql, callback, NULL, &err_msg);
  if (err_no != SQLITE_OK) {
    fprintf(stderr, "SQL error: %s\n", err_msg);
  }
}

void initialize_classes() {
  num_classes = 0;
  FILE *fp = fopen("classes", "r");
  int i;
  if (fp == NULL) {
    fprintf(stderr, "Initialization failed: couldn't read classes");
    exit(1);
  }
  for (i = 0; i < MAX_CLASSES - 1 && fgets(classes[i], CLASS_LEN, fp) != NULL; i++) {
    num_classes++;
    int len = strlen(classes[i]);
    if (classes[i][len - 1] == '\n') {
      classes[i][len - 1] = '\0';
    }
  }

  all_classes.classes = malloc(num_classes * sizeof(char *));
  for (i = 0; i < num_classes; i++) {
    *(all_classes.classes + i) = (char *) classes[i];
  }
  all_classes.num = num_classes;
  fclose(fp);
}

void initialize() {
  sql_cmd = malloc(CMD_SIZE * sizeof(char));
  int err_no;

  initialize_classes();

  err_no = sqlite3_open("hw.db", &db);
  if (err_no) {
    fprintf(stderr, "Failed to open db: %s", sqlite3_errmsg(db));
  }

  strcpy(sql_cmd,"CREATE TABLE IF NOT EXISTS HW"\
            "(CLASS_ID INT, CLASS TEXT, ASSIGNMENT TEXT, "\
            "DUE DATETIME, COMPLETED INT);");
  do_sql(sql_cmd, NULL);
}

void finish() {
  sqlite3_close(db);
}

void add_hw(int class_id, char *assignment, int due_month, int due_day, int due_hour, int due_min) {
  if (class_id >= num_classes) {
    fprintf(stderr, "Invalid class id: %d\n", class_id);
    return;
  }
  char *class = classes[class_id];
  char *date = malloc(DATE_LEN * sizeof(char));
  sprintf(date, "2013-%02d-%02d %02d:%02d:00", due_month, due_day, due_hour, due_min);
  sprintf(sql_cmd, "INSERT INTO HW VALUES (%d, '%s', '%s', '%s', 0)", class_id, class, assignment, date);
  do_sql(sql_cmd, NULL);
}

void find_hw(int num_days, int (*callback)(void *, int, char **, char **)) {
  time_t timer;
  char buffer[25];
  struct tm *tm_info;
  timer = time(0) + 60 * 60 * 24 * num_days;
  tm_info = localtime(&timer);
  strftime(buffer, 25, "%Y-%m-%d %H:%M:%S", tm_info);
  sprintf(sql_cmd, "SELECT * FROM HW WHERE DUE < '%s';\n", buffer);
  do_sql(sql_cmd, callback);
}

void get_assignments(int class_id, int (*callback)(void *, int, char **, char **)) {
  if (class_id >= num_classes || class_id < 0) {
    fprintf(stderr, "Invalid class id: %d:\n", class_id);
    return;
  }
  time_t timer;
  char buffer[25];
  struct tm *tm_info;
  time(&timer);
  tm_info = localtime(&timer);
  strftime(buffer, 25, "%Y-%m-%d %H:%M:%S", tm_info);
  sprintf(sql_cmd, "SELECT * FROM HW WHERE CLASS_ID = %d AND DUE > '%s';\n", class_id, buffer);
  do_sql(sql_cmd, callback);
}

void mark_completed(int class_id, char *assignment) {
  sprintf(sql_cmd, "UPDATE HW SET COMPLETED=1 "\
      "WHERE CLASS_ID = %d AND ASSIGNMENT = '%s';\n", class_id, assignment);
  do_sql(sql_cmd, NULL);
}

void add_class(char *name) {
  FILE *fp = fopen("classes", "a");
  fprintf(fp, "%s\n", name);
  fclose(fp);
  initialize_classes();
}

void cleanup() {
  do_sql("DELETE FROM HW WHERE ROWID NOT IN ("\
         "SELECT MIN(ROWID) FROM HW "\
         "GROUP BY CLASS_ID, CLASS, ASSIGNMENT, DUE, COMPLETED);", NULL);
}

t_classes *get_classes() {
  return &all_classes;
}
