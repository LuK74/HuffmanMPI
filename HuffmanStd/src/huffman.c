#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tree.h"


s_table generate_table(FILE * file) {
  s_table table = init_table(0,0,NULL);
  int total_occ = 0;
  char c = fgetc(file);
  
  while(c != EOF) {
    total_occ++;
    increment_entry(table, c);
    c = fgetc(file);
  }

  build_frequency(table, total_occ);

  return table;
}

int compress(FILE * input, FILE * output, s_table table, tree root) {
  rewind(input);
  char c = fgetc(input);

  int curr_size = 0;
  char to_print = 0;
  s_entry entry = NULL;

  int tube[2];
  pipe(tube);
  int total_size = 0;
  int total_occ = 0;
  
  while(c != EOF) {
    total_occ++;
    entry = get_entry(table, c);
    if (entry == NULL) return -1;
    for (int i = 0; entry->encoding[i] != '\n'; i++) {
      if (curr_size != 0) to_print = to_print<<1;
      
      to_print = to_print | ((entry->encoding[i]) - '0');
      curr_size++;

      if (curr_size == 8) {
	write(tube[1], &to_print, 1);
	to_print = 0;
	curr_size = 0;

	total_size++;
      } //else {
	//to_print = to_print<<1;
      //}
      
    }
    c = fgetc(input);
  }

  int padding = 0;
  if (curr_size != 0) {
    padding = 8-curr_size;
    to_print = to_print<<padding;
    write(tube[1], &to_print, 1); 
    total_size++;
  }

  write_table(output, table, padding, total_occ);
  
  //char * buffer = malloc(sizeof(char)*total_size);
  //read(tube[0], buffer, total_size);

  char c_to_print;
  
  while(total_size > 0) {
    read(tube[0], &c_to_print, 1);
    fputc(c_to_print, output);
    total_size--;
  }

  close(tube[0]);
  close(tube[1]);
  return 0;
}

int decompress(FILE * input, FILE * output) {
  rewind(input);

  int * padding = malloc(sizeof(int));
  s_table table = read_table(input, padding);
  tree root = convert_from_tbl(table);

  tree current_node = root;
    
  char c = fgetc(input);
  char next = fgetc(input);
  int direction = 0;
  
  while (c != EOF) {
    int limit = 0;
    if (next == EOF) {
      limit = *padding;
    }
    
    for (int i = 7; i >= limit; i--) {
      if (current_node != NULL) {
	if (current_node->flag == -1) {
	  fputc(current_node->value, output);
	  current_node = root;
	}
      } else {
	return -1;
      }
      
      direction = c&(1<<i);
      if (direction != 0) {
	current_node = current_node->right;
      } else {
	current_node = current_node->left;
      }
    }

    c = next;
    next = fgetc(input);
  }  

  if (current_node != NULL) {
    if (current_node->flag == -1) {
      fputc(current_node->value, output);
      current_node = root;
    }
  }
  
  return 0;
}

int test(char * input) {
  FILE * input_file = fopen(input, "r");
  s_table table = generate_table(input_file);
  tree root = convert_from_tbl(table);
  fill_encoding(root, table);

  FILE * output = fopen("test_compressed_output.txt", "w");
  compress(input_file, output, table, root);
  fclose(output);
  
  export_tree(root, "test_tree.txt");

  FILE * new_output = fopen("test_decompressed_output.txt", "w");
  FILE * new_input = fopen("test_compressed_output.txt", "r");
  decompress(new_input, new_output);
  fclose(new_output);
  fclose(new_input);

  return 0;
}

int main(int argc, char * argv[]) {
  if (argc == 3 && strcmp(argv[1],"--test")==0) {
    test(argv[2]);
  } else {
    FILE * input = fopen(argv[1], "r");
    s_table table = generate_table(input);
    tree root = convert_from_tbl(table);
    fill_encoding(root, table);

    FILE * output = fopen(argv[2], "w");

    compress(input, output, table, root);
  
    export_tree(root, "tree.txt");
    fclose(input);
    fclose(output);
  }
  return 0;
}
