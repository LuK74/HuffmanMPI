#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>

struct simpl_entry {
  char key;
  int occurences;
  float frequency;
  char * encoding;
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

/*
 * Entries function
 */
s_entry create_entry(char key, int occurences, int frequency);
s_entry get_entry(s_table table, char key);
int get_entry_index(s_table table, char key);

/*
 * More specific functions
 */
int increment_entry(s_table table, char key);
int increment_n_entry(s_table table, char key, int n);
void build_frequency(s_table table, int total_occurences);
int set_encoding(s_table table, char key, char * encoding);

/*
 * Import & Export Table
 */
s_table read_table(FILE * input, int * padding, int * size);
void write_table(FILE * output, s_table table, int padding, int total_occ, int total_size);
void write_table_mpi(MPI_File * output, s_table table, int padding, int total_occ, int total_size);
