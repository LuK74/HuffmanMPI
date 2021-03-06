#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "tree.h"


s_table generate_table(FILE * file, int * f_size) {
  s_table table = init_table(0,0,NULL);
  int total_occ = 0;
  char c = fgetc(file);
  
  while(c != EOF) {
    total_occ++;
    increment_entry(table, c);
    c = fgetc(file);
  }

  build_frequency(table, total_occ);
  *f_size = total_occ;

  return table;
}

int compress(FILE * input, FILE * output, s_table table, tree root, int f_size) {
  rewind(input);
  char c = fgetc(input);

  int curr_size = 0;
  char to_print = 0;
  s_entry entry = NULL;

  int buf_size = f_size;
  char * buf = (char*) malloc(sizeof(char)*buf_size);
  int total_size = 0;
  int total_occ = 0;
  
  while(c != EOF) {
    total_occ++;
    entry = get_entry(table, c);
    if (entry == NULL) return -1;
    for (int i = 0; entry->encoding[i] != '\0'; i++) {
      if (curr_size != 0) to_print = to_print<<1;
      
      to_print = to_print | ((entry->encoding[i]) - '0');
      curr_size++;

      if (curr_size == 8) {
	buf[total_size] = to_print;
	to_print = 0;
	curr_size = 0;

	total_size++;
	if (total_size >= buf_size) {
	  char * old_buf = buf;
	  buf = (char *) malloc(sizeof(char)*(buf_size*2));
	  for (int i = 0; i < buf_size; i++) { buf[i] = old_buf[i] ; }
	  buf_size *= 2;
	}
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
    buf[total_size] = to_print;
    total_size++;
  }

  write_table(output, table, padding, total_occ, total_size);
  
  //char * buffer = malloc(sizeof(char)*total_size);
  //read(tube[0], buffer, total_size);

  for (int i = 0; i < total_size; i++) {
     fputc(buf[i], output);
  }

  free(buf);
  
  return 0;
}

int decompress(FILE * input, FILE * output) {
  rewind(input);

  int padding;
  int size;
  s_table table = read_table(input, &padding, &size);
  tree root = convert_from_tbl(table);

  tree current_node = root;
    
  char c = fgetc(input);
  int direction = 0;
  
  while (size > 0) {
    int limit = 0;
    if (size == 1) {
      limit = padding;
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

    c = fgetc(input);
    size--;
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
  int f_size;
  s_table table = generate_table(input_file, &f_size);
  tree root = convert_from_tbl(table);
  fill_encoding(root, table);

  FILE * output = fopen("test_compressed_output.txt", "w");
  compress(input_file, output, table, root, f_size);
  fclose(output);

  char tree_filename[] = "test_tree.txt";
  export_tree(root, tree_filename);
  
  FILE * new_output = fopen("test_decompressed_output.txt", "w");
  FILE * new_input = fopen("test_compressed_output.txt", "r");
  decompress(new_input, new_output);
  fclose(new_output);
  fclose(new_input);
  
  return 0;
}

void print_help() {
  printf("Usage : ./huffman option [input_filename] [output_filename]\n");
  printf("\nOption :\n");
  printf("-c/--compression = Compress [input_filename] and outputs it as [output_filename] if furnished, if not as result_decompression.txt\n");
  printf("\n-d/--decompression = Decompress [input_filename] and outputs it as [output_filename] if furnished, if not as result_decompression.txt\n");
  printf("\n-t/--test = Will compress and decompress the file named [input_filename], and output a file representing the tree used for the compression\n");
}

void process_compression(char * input, char * output) {
  FILE * input_file = fopen(input, "r");
  int f_size;
  s_table table = generate_table(input_file, &f_size);
  tree root = convert_from_tbl(table);
  fill_encoding(root, table);

  char tree_filename[] = "test_tree.txt";
  export_tree(root, tree_filename);
  
  FILE * output_file = fopen(output, "w");
  compress(input_file, output_file, table, root, f_size);
  fclose(output_file);
  fclose(input_file); 
}

void process_decompression(char * input, char * output) {
  FILE * input_compressed = fopen(input, "r");
  FILE * output_file = fopen(output, "w");
  decompress(input_compressed, output_file);
  fclose(input_compressed);
  fclose(output_file);
}

void process_args(int argc, char * argv[]) {
  if (argc <= 1) {
    printf("No arguments or options passed, try --help\n");
  } else if (argc == 2) {
    if (strcmp(argv[1], "--help")==0) {
      print_help();
    } else {
      printf("Unknown option, try --help");
    }
  } else if (argc == 3) {
    if (strcmp(argv[1], "--compress")==0 || strcmp(argv[1], "-c")==0) {
      char result_file[] = "result_compression.txt";
      process_compression(argv[2], result_file);
    } else if (strcmp(argv[1], "--decompress")==0 || strcmp(argv[1], "-d")==0) {
      char result_file[] = "result_decompression.txt";
      process_decompression(argv[2], result_file);
    } else if (strcmp(argv[1], "--test")==0 || strcmp(argv[1], "-t")==0) {
      test(argv[2]);
    } else {
      printf("Unknown option, try --help");
    }
  } else if (argc == 4) {
    if (strcmp(argv[1], "--compress")==0 || strcmp(argv[1], "-c")==0) {
      process_compression(argv[2], argv[3]);
    } else if (strcmp(argv[1], "--decompress")==0 || strcmp(argv[1], "-d")==0) {
      process_decompression(argv[2], argv[3]);
    } else {
      printf("Unknown option, try --help");
    }
  }
}

int main(int argc, char * argv[]) {
  process_args(argc, argv);
  return 0;
}
