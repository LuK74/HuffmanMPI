#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include "tree.h"

int mpi_rank, mpi_size;

/*
 * Generate an hashtable containing all the characters in the file "filename"
 * Will also compute the informations associated to it (occurences and frequency)
 * Return the table
 */
s_table generate_table(char * filename, int * f_size) {
  s_table table = init_table(0,0,NULL);
  s_table final_table = init_table(0,0,NULL);
  int total_occ = 0;
  char c;

  MPI_File fh;
  MPI_Status status;
  MPI_Offset file_size;
  MPI_File_open(MPI_COMM_WORLD, filename, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
  MPI_File_get_size(fh, &file_size);
  
  int to_read = file_size/mpi_size;
  if (mpi_rank + 1 == mpi_size) {
    to_read += (file_size%mpi_size);
  }
  
  char * buf = (char*) malloc(sizeof(char)*to_read);
  MPI_File_read_at(fh, mpi_rank*(file_size/mpi_size) ,buf, to_read, MPI_CHAR, &status);
  MPI_File_close(&fh);
  
  c = buf[0];
  int index = 1;
  while (index <= to_read) {
     total_occ++;
     increment_entry(table, c);
     c = buf[index++];
  }

  int n_entries = 0;
  char key;
  int n_occurences = 0;
  for (int i = 0; i < mpi_size; i++) {
    if (i == mpi_rank) {
      MPI_Bcast(&(table->n_entries), 1, MPI_INT, i, MPI_COMM_WORLD);
      for (int j = 0; j < table->n_entries; j++) {
	MPI_Bcast(&(table->entries[j]->key), 1, MPI_CHAR, i, MPI_COMM_WORLD);
	MPI_Bcast(&(table->entries[j]->occurences), 1, MPI_INT, i, MPI_COMM_WORLD);
	increment_n_entry(final_table, (table->entries[j]->key), (table->entries[j]->occurences));                                
      }
    } else {
      MPI_Bcast(&n_entries, 1, MPI_INT, i, MPI_COMM_WORLD);
      for (int j = 0; j < n_entries; j++) {
	MPI_Bcast(&key, 1, MPI_CHAR, i, MPI_COMM_WORLD);
	MPI_Bcast(&n_occurences, 1, MPI_INT, i, MPI_COMM_WORLD);
	increment_n_entry(final_table, key, n_occurences);
	total_occ += n_occurences;
      }
    }

  }  

  build_frequency(final_table, total_occ);
  *f_size = total_occ;
  
  return final_table;
}

/*
 * Compression function, will translate each character in its corresponding encoding
 * And print it on the output file
 * Parallelization added thanks to MPI
 */
int compress(char * input, char * output, s_table table, int f_size) {
  MPI_File fh_input;
  MPI_Status status;
  MPI_Offset file_size;
  MPI_File_open(MPI_COMM_WORLD, input, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh_input);
  MPI_File_get_size(fh_input, &file_size);
  
  int to_read = file_size/mpi_size;
  if (mpi_rank + 1 == mpi_size) {
    to_read += (file_size%mpi_size);
  }
  
  char * input_buf = (char*) malloc(sizeof(char)*to_read);
  MPI_File_read_at(fh_input, mpi_rank*(file_size/mpi_size) ,input_buf, to_read, MPI_CHAR, &status);
  
  s_entry entry = NULL;

  int buf_size = f_size/mpi_size;
  char * buf = (char *) malloc(sizeof(char)*buf_size);
  
  int total_size = 0;
  int total_occ = 0;

  int index = 1;
  char c = input_buf[0];

  /*
   * Main job done by each process
   * Conversion of its part of the file and save it in a buffer
   */ 
  while (index <= to_read) {
    total_occ++;
    entry = get_entry(table, c);
    if (entry == NULL) return -1;
    for (int i = 0; entry->encoding[i] != '\0'; i++) {
      buf[total_size] = entry->encoding[i];
      total_size++;
      if (total_size >= buf_size) {
	char * old_buf = buf;
	buf = (char *) malloc(sizeof(char)*(buf_size*1.5));
	for (int i = 0; i < buf_size; i++) { buf[i] = old_buf[i] ; }
	buf_size *= 1.5;
	if (old_buf != NULL) free(old_buf);
      }
    }
    c = input_buf[index];
    index++;
  }

  /*
   * One process has to collect every buffer size of each process in order
   * to write our metadata before starting writing into the output file
   */ 
  if (mpi_rank == 0) {
    int final_size = 0; 
    int * buf_sizes = (int*) malloc(sizeof(int)*(mpi_size-1));
    for (int i = 1; i < mpi_size; i++) {
      MPI_Recv(buf_sizes+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      final_size += buf_sizes[i];
    }
    
    final_size += total_size;	
    int padding = (final_size%8 == 0 ? 0 : 8 - final_size%8);
    
    FILE * f_output = fopen(output, "w");
    write_table(f_output, table, padding, file_size, (final_size/8)+(final_size%8 != 0 ? 1 : 0));
    fclose(f_output);

  } else {
    MPI_Send(&total_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
  }
  
  int curr_size = 0;
  char to_print = 0;

  /*
   * Waiting to receive informations about the process previous writing
   */
  if (mpi_rank != 0) {
    MPI_Recv(&curr_size, 1, MPI_INT, mpi_rank - 1, 0, MPI_COMM_WORLD, &status);
    MPI_Recv(&to_print, 1, MPI_CHAR, mpi_rank - 1, 0, MPI_COMM_WORLD, &status);
  }

  /*
   * Open the file, write in it and close it
   * In order for the other process to access it
   */
  FILE * f_output = fopen(output, "a");
  fseek(f_output, 0, SEEK_END);	
  for (int i = 0; i < total_size; i++) {
    if (curr_size != 0) { to_print = to_print<<1; }
    to_print = to_print | (buf[i] - '0');
    curr_size++;
	
    if (curr_size == 8) {
      fputc(to_print, f_output);
      curr_size = 0;
      to_print = 0;
    }
  }

  if (mpi_rank + 1 == mpi_size) {
    /*
     * If it is the last process writing in the file
     * We might have to add some padding
     */
    if (curr_size != 0) {
      if (curr_size != 8) {
	to_print = to_print << (8-curr_size);
      }
      fputc(to_print, f_output);
    }
    fclose(f_output);
    if (buf != NULL) free(buf);
  } else {
    fclose(f_output);
    if (buf != NULL) free(buf);
    
    MPI_Send(&curr_size, 1, MPI_INT, mpi_rank + 1, 0, MPI_COMM_WORLD);
    MPI_Send(&to_print, 1, MPI_CHAR, mpi_rank + 1, 0, MPI_COMM_WORLD);
  }
    
  MPI_File_close(&fh_input);
  return 0;
}

/*
 * Decompress a file compressed by this huffman algorithm
 * Not parallelized with MPI
 */
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
  int f_size;
  s_table table = generate_table(input, &f_size);
  tree root = convert_from_tbl(table);
  fill_encoding(root, table);

  char * output = (char *) "test_compressed_output.txt";
  FILE * output_file = fopen(output, "w");
  compress(input, output, table, f_size);
  fclose(output_file);

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
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  
  FILE * input_file = fopen(input, "r");
  int f_size;
  s_table table = generate_table(input, &f_size);
  tree root = convert_from_tbl(table);
  fill_encoding(root, table);
  
  compress(input, output, table, f_size);
  fclose(input_file);

  MPI_Finalize();
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
