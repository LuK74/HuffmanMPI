#include <string.h>
#include "simpl_tbl.h"

/*
 * Init table
 * init_tab_size correspond to the initial size of the array entries
 */
s_table init_table(int n_entries, int init_tab_size, s_entry * entries) {
  s_table table = (s_table) malloc(sizeof(table));
  table->n_entries = n_entries;
  table->tab_size = init_tab_size;
  table->entries = entries;
  return table;
}

void add_entry(s_table table, s_entry new_entry) {
  for (int i = 0; i < table->n_entries; i++) {
    if ((table->entries[i])->key == new_entry->key) {
      (table->entries[i])->occurences = new_entry->occurences;
      (table->entries[i])->frequency = new_entry->frequency;
      break;
    }
  }

  if (table->tab_size <= table->n_entries) {
    s_entry * old_entries = table->entries;
    if (table->tab_size == 0) {
      table->tab_size = 1;
    }
    table->tab_size = table->tab_size * 2;
    
    table->entries = (s_entry*) malloc(sizeof(struct simpl_entry)*(table->tab_size));
    for (int i = 0; i < table->n_entries; i++) {
      (table->entries)[i] = old_entries[i];
    }

    free(old_entries);

  }

  table->entries[table->n_entries] = new_entry;
  table->n_entries = table->n_entries + 1;

}

void remove_entry(s_table table, char key) {
  int found = 0;
  
  for (int i = 0; i < table->n_entries; i++) {
    if ((table->entries[i])->key == key || found) {
      if (!found) {
	found = 1;
	table->n_entries--;
      }
  
      if (i < table->n_entries - 1) {
	(table->entries[i]) = (table->entries[i+1]);
      }
    }
  }

  if (table->tab_size/2 <= table->n_entries) {
    s_entry * old_entries = table->entries;
    table->tab_size = (table->tab_size / 2) + 1;
    
    table->entries = (s_entry*) malloc(sizeof(struct simpl_entry)*(table->tab_size));
    for (int i = 0; i < table->n_entries; i++) {
      (table->entries)[i] = old_entries[i];
    }

    free(old_entries);
    
  }
  
}

s_entry get_entry(s_table table, char key) {
  for (int i = 0; i < table->n_entries; i++) {
    if (((table->entries)[i])->key == key) {
      return ((table->entries)[i]);
    }
  }
  return NULL;
}

int get_entry_index(s_table table, char key) {
  for (int i = 0; i < table->n_entries; i++) {
    if (((table->entries)[i])->key == key) {
      return i;
    }
  }
  return -1;
}

/*
 * Create an entry with the given fields
 */
s_entry create_entry(char key, int occurences, int frequency) {
  s_entry new_entry = (s_entry) malloc(sizeof(struct simpl_entry));
  new_entry->key = key;
  new_entry->occurences = occurences;
  new_entry->frequency = frequency;
  new_entry->encoding = 0;
  
  return new_entry;
}

/*
 * Increment the value of an entry IF it does exist
 * Return 0 if the entry exist and it has been incremented
 * Return -1 if the entry doesn't exist 
 */
int increment_entry(s_table table, char key) {
  int index = get_entry_index(table, key);
  s_entry entry = NULL;
  
  if (index == -1) {
    entry = create_entry(key, 0, 0);
    add_entry(table, entry);
    index = get_entry_index(table, key);
  } else {
    entry = table->entries[index];
  } 

  entry->occurences = entry->occurences + 1;

  for (int i = index; i > 0; i--) {
    if ((table->entries[i])->occurences > (table->entries[i-1])->occurences) {
      s_entry swap_entry = table->entries[i];
      table->entries[i] = table->entries[i-1];
      table->entries[i-1] = swap_entry;
    } else {
      break;
    }
  }
  
  
  return 0;
}

/*
 * Given a total number of occurences, will build the frequency
 * value in each entry
 */
void build_frequency(s_table table, int total_occurences) {
  for (int i = 0; i < table->n_entries; i++) {
    ((table->entries)[i])->frequency = ((float)(((table->entries)[i])->occurences)) / ((float)total_occurences);
  }
}

/*
 * Set encoding for an entry
 * Return -1 if the entry doesn't exist
 * Return 0 if the function executed properly
 */
int set_encoding(s_table table, char key, char * encoding) {
  s_entry entry = get_entry(table, key);

  if (entry != NULL) {
    entry->encoding = encoding;
    return 0;
  }

  return -1;
}

void write_table(FILE * output, s_table table, int padding, int total_occ, int total_size) {
  fprintf(output, "Padding=%d\n", padding);
  fprintf(output, "Size=%d\n", total_occ);
  fprintf(output, "SizeToRead=%d\n", total_size);

  for (int i = 0; i < table->n_entries; i++) {
    fprintf(output, "Key=%c:%d\n", table->entries[i]->key, table->entries[i]->occurences); 
  }

  fprintf(output, "END\n");
}

/*
void write_table_mpi(MPI_File * output, s_table table, int padding, int total_occ, int total_size) {
  //MPI_Status

  fprintf(output, "Padding=%d\n", padding);
  fprintf(output, "Size=%d\n", total_occ);
  fprintf(output, "SizeToRead=%d\n", total_size);

  for (int i = 0; i < table->n_entries; i++) {
    fprintf(output, "Key=%c:%d\n", table->entries[i]->key, table->entries[i]->occurences); 
  }

  fprintf(output, "END\n");
}
*/



/*
 * Read table from huffman compressed file
 */
s_table read_table(FILE * input, int * padding, int * size) {
  s_table table = init_table(0,0, NULL);

  int total_occ = 0;
  
  fscanf(input, "Padding=%d\n", padding);
  fscanf(input, "Size=%d\n", &total_occ);
  fscanf(input, "SizeToRead=%d\n", size);

  char * str = (char *) malloc(sizeof(char)*100);
  char key;
  int occurences = 0;
  int _read = fscanf(input, "Key=%c:%d\n", &key, &occurences);

  while(strcmp(str, "END") != 0) {
    //char key = str[0];
    //int occurences;
    //sscanf(str, "Key=%c:%d\n", &key ,&occurences);

    s_entry entry = create_entry(key, occurences,(float)((float)(occurences)/(float)(total_occ)));
    add_entry(table, entry);

    _read = fscanf(input, "Key=%c:%d\n", &key, &occurences);
    if (_read == 0) {
      fscanf(input, "%s\n", str);
    }
    
  }

  return table;
}
