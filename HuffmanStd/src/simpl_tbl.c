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
    
    table->entries = malloc(sizeof(table)*(table->tab_size));
    for (int i = 0; i < table->n_entries; i++) {
      (table->entries)[i] = old_entries[i];
    }

    free(old_entries);
    
  }
  
}

void * get_value(s_table table, char key) {
  for (int i = 0; i < table->n_entries; i++) {
    if (((table->entries)[i])->key == key) {
      return ((table->entries)[i])->value;
    }
  }
  return NULL;
}
