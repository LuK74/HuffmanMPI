#include <stdlib.h>

struct simpl_entry {
  char key;
  int occurences;
  int frequency;
};

typedef struct simpl_entry * s_entry;

struct simpl_table {
  int n_entries;
  s_entry * entries;
  int tab_size;
};

typedef struct simpl_table * s_table;

/*
 * Primary functions
 */
s_table init_table(int n_entries, int init_tab_size, s_entry * entries);
void add_entry(s_table table, s_entry new_entry);
void remove_entry(s_table table, char key);
void * get_value(s_table table, char key);

/*
 * Entries function
 */
s_entry create_entry(char key, int occurences, int frequency);

/*
 * More specific functions
 */
int increment_entry(s_table table, char key);
void build_frequency(s_table table, int total_occurences);
