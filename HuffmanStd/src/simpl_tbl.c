#include "simpl_tbl.h"

/*
 * Init table
 * init_tab_size correspond to the initial size of the array entries
 */
s_table init_table(int n_entries, int init_tab_size, s_entry * entries) {
  s_table table = malloc(sizeof(table));
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
    
    table->entries = malloc(sizeof(struct simpl_entry)*(table->tab_size));
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
    
    table->entries = malloc(sizeof(struct simpl_entry)*(table->tab_size));
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
  int index = get_entry_index(table, key);
  s_entry entry = NULL;
  
  if (index == -1) {
    entry = create_entry(key, 0, 0);
    add_entry(table, entry);
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
    ((table->entries)[i])->frequency = (((table->entries)[i])->occurences) / total_occurences;
  }
}
