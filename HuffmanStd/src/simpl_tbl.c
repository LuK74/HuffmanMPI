#include "simpl_tbl.h"

/*
 * Init table
 * init_tab_size correspond to the initial size of the array entries
 */
s_table init_table(int n_entries, int init_tab_size, s_entry * entries) {
  s_table table = malloc(sizeof(s_table));
  table->n_entries = n_entries;
  table->tab_size = init_tab_size;
  table->entries = entries;
  return table;
}

void add_entry(s_table table, s_entry new_entry) {
  for (int i = 0; i < table->n_entries; i++) {
    if ((table->entries[i])->key == new_entry->key) {
      (table->entries[i])->value = new_entry->value;
      break;
    }
  }

  if (table->tab_size <= table->n_entries) {
    s_entry * old_entries = table->entries;
    table->tab_size = table->tab_size * 2;
    
    table->entries = malloc(sizeof(table)*(table->tab_size));
    for (int i = 0; i < table->n_entries; i++) {
      (table->entries)[i] = old_entries[i];
    }

    free(old_entries);

    table->entries[table->n_entries] = new_entry;
    table->n_entries = table->n_entries + 1;

  }

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
    
    table->entries = malloc(sizeof(table)*(table->tab_size));
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

/*
 * Create an entry with the given fields
 */
s_entry create_entry(char key, int occurences, int frequency) {
  s_entry new_entry = malloc(sizeof(struct simpl_entry));
  new_entry->key = key;
  new_entry->occurences = occurences;
  new_entry->frequency = frequency;

  return new_entry;
}

/*
 * Increment the value of an entry IF it does exist
 * Return 0 if the entry exist and it has been incremented
 * Return -1 if the entry doesn't exist 
 */
int increment_entry(s_table table, char key) {
  s_entry entry = get_entry(table, key);

  if (entry == NULL) {
    return -1;
  }

  entry->occurences = entry->occurences + 1;
  
  return 0;
}

/*
 * Given a total number of occurences, will build the frequency
 * value in each entry
 */
void build_frequency(s_table table, int total_occurences) {
  for (int i = 0; i < table->n_entries; i++) {
    ((table->entries)[i])->frequency = (((table->entries)[i])->occurences) / total_occurences;
  }
}
