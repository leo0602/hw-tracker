#ifndef HW_H_
#define HW_H_
typedef struct t_classes {
  char **classes;
  int num;
} t_classes;
void initialize();
void finish();
void add_hw(int class_id, char *assignment, int due_month, int due_day, int due_hour, int due_min);
void cleanup();
void clear();
void find_hw(int num_days, int (*callback)(void *, int, char **, char **));
void get_assignments(int class_id, int (*callback)(void *, int, char **, char **));
struct t_classes *get_classes();
void mark_completed(int class_id, char *assignment);
void add_class(char *name);
#endif
