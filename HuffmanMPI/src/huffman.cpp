#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <mpi.h>
#include "tree.h"

int mpi_rank, mpi_size;

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

  /* int n_entries = 0;
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
      }
    }

  }*/
  
  /*if (mpi_rank == 0) {  
    int n_entries = 0;
    for (int i = 1; i < mpi_size; i++) {                                                  
      MPI_Recv(&n_entries, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);                      
      char val_entry;                                                                      
      int n_occurences = 0;                                                                
      for (int j = 0; j < n_entries; j++) {
	MPI_Recv(&val_entry, 1, MPI_CHAR, i, 0, MPI_COMM_WORLD, &status);
	MPI_Recv(&n_occurences, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
	increment_n_entry(final_table, val_entry, n_occurences);
	total_occ += n_occurences;
      }
    }

    for (int i = 0; i < table->n_entries; i++) {
      increment_n_entry(final_table, (table->entries[i]->key), (table->entries[i]->occurences));
    }
    
    MPI_Bcast(&(final_table->n_entries), 1, MPI_INT, 0, MPI_COMM_WORLD);
    for (int i = 0; i < final_table->n_entries; i++) {
      MPI_Bcast(&(final_table->entries[i]->key), 1, MPI_CHAR, 0, MPI_COMM_WORLD);
      MPI_Bcast(&(final_table->entries[i]->occurences), 1, MPI_INT, 0, MPI_COMM_WORLD);
    }
    
  } else {
    MPI_Send(&(table->n_entries), 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    for (int i = 0; i < table->n_entries; i++) {
      MPI_Send(&(table->entries[i]->key), 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
      MPI_Send(&(table->entries[i]->occurences), 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    int n_entries = 0;
    MPI_Bcast(&n_entries, 1, MPI_INT, 0, MPI_COMM_WORLD);                      
    char val_entry;                                                                      
    int n_occurences = 0;                                                                
    for (int j = 0; j < n_entries; j++) {
      MPI_Bcast(&val_entry, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
      MPI_Bcast(&n_occurences, 1, MPI_INT, 0, MPI_COMM_WORLD);
      increment_n_entry(final_table, val_entry, n_occurences);
      total_occ += n_occurences;
    }                                                                                    
    }*/

  build_frequency(final_table, total_occ);
  *f_size = total_occ;
  
  return final_table;
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

int compress_mpi(char * input, char * output, s_table table, tree root, int f_size) {
  double allstarttime, allendtime;
  allstarttime = MPI_Wtime();
  
  MPI_File fh_input;
  MPI_Status status;
  MPI_Offset file_size;
  MPI_File_open(MPI_COMM_WORLD, input, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh_input);
  //MPI_File_open(MPI_COMM_WORLD, output, MPI_MODE_RDWR, MPI_INFO_NULL, &fh_output);
  MPI_File_get_size(fh_input, &file_size);
  
  int to_read = file_size/mpi_size;
  if (mpi_rank + 1 == mpi_size) {
    to_read += (file_size%mpi_size);
  }
  
  char * input_buf = (char*) malloc(sizeof(char)*to_read);
  MPI_File_read_at(fh_input, mpi_rank*(file_size/mpi_size) ,input_buf, to_read, MPI_CHAR, &status);
  
  s_entry entry = NULL;

  int buf_size = f_size;
  char * buf = (char *) malloc(sizeof(char)*buf_size);
  
  int total_size = 0;
  int total_occ = 0;

  int index = 1;
  char c = input_buf[0];

  double starttime, endtime;

  starttime = MPI_Wtime();
  while(index <= to_read) {
    total_occ++;
    entry = get_entry(table, c);
    if (entry == NULL) return -1;
    for (int i = 0; entry->encoding[i] != '\0'; i++) {
      buf[total_size] = entry->encoding[i];
      total_size++;
      if (total_size >= buf_size) {
	char * old_buf = buf;
	buf = (char *) malloc(sizeof(char)*(buf_size*2));
	for (int i = 0; i < buf_size; i++) { buf[i] = old_buf[i] ; }
	buf_size *= 2;
	if (old_buf != NULL) free(old_buf);
      }
    } //else {
	//to_print = to_print<<1;
      //}
      c = input_buf[index];
      index++;
  }
  endtime = MPI_Wtime();
  printf("That took %f seconds for %d\n", endtime-starttime, mpi_rank);
  
  // modify this part in order to write when we receive the data
  // should be quite simple and a good gain of time

  if (mpi_rank + 1 == mpi_size) {
    double starttime, endtime;
    
    starttime = MPI_Wtime();
     
    int final_size = 0; 
    int * buf_sizes = (int*) malloc(sizeof(int)*(mpi_size-1));
    for (int i = 0; i < mpi_size - 1; i++) {
      MPI_Recv(buf_sizes+i, 1, MPI_INT, i, 0, MPI_COMM_WORLD, &status);
      final_size += buf_sizes[i];
    }
    endtime = MPI_Wtime();
    printf("Waiting for everyone took %f seconds\n", endtime-starttime);
    
    
    final_size += total_size;	
    int padding = (final_size%8 == 0 ? 0 : 8 - final_size%8);

    FILE * f_output = fopen(output, "w");
    write_table(f_output, table, padding, file_size, (final_size/8)+(final_size%8 != 0 ? 1 : 0));
    fclose(f_output);
    
    char to_print = 0;
    int curr_size = 0;

    MPI_Send(&curr_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    if (mpi_size != 1) {
      MPI_Recv(&curr_size, 1, MPI_INT, mpi_rank-1, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&to_print, 1, MPI_CHAR, mpi_rank-1, 0, MPI_COMM_WORLD, &status);
    }

    
    starttime = MPI_Wtime();
     
    f_output = fopen(output, "a");
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
	
    if (curr_size != 0) {
      if (curr_size != 8) {
	to_print = to_print << (8-curr_size);
      }
      fputc(to_print, f_output);
    }

    endtime = MPI_Wtime();
    printf("Writting in the file took %f seconds\n", endtime-starttime);
    
    if (buf != NULL) free(buf);
    fclose(f_output);
    MPI_File_close(&fh_input);
  
  } else {
    int curr_size = 0;
    char to_print = 0;

    MPI_Send(&total_size, 1, MPI_INT, mpi_size - 1, 0, MPI_COMM_WORLD);

    //MPI_File_seek(fh_output, 0, MPI_SEEK_END);
    if (mpi_rank != 0) {
      MPI_Recv(&curr_size, 1, MPI_INT, mpi_rank - 1, 0, MPI_COMM_WORLD, &status);
      MPI_Recv(&to_print, 1, MPI_CHAR, mpi_rank - 1, 0, MPI_COMM_WORLD, &status);
    } else {
      MPI_Recv(&curr_size, 1, MPI_INT, mpi_size - 1, 0, MPI_COMM_WORLD, &status);
    }
    
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
    fclose(f_output);
    if (buf != NULL) free(buf);
    
    MPI_Send(&curr_size, 1, MPI_INT, mpi_rank + 1, 0, MPI_COMM_WORLD);
    MPI_Send(&to_print, 1, MPI_CHAR, mpi_rank + 1, 0, MPI_COMM_WORLD);

    MPI_File_close(&fh_input);
  } 
  //char * buffer = malloc(sizeof(char)*total_size);
  //read(tube[0], buffer, total_size);
  allendtime = MPI_Wtime();
  printf("It took in total %f for %d\n", allendtime-allstarttime, mpi_rank);
  
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
  s_table table = generate_table(input, &f_size);
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
  MPI_Init(NULL, NULL);
  MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
  MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
  
  FILE * input_file = fopen(input, "r");
  int f_size;
  s_table table = generate_table(input, &f_size);
  tree root = convert_from_tbl(table);
  fill_encoding(root, table);
  
  compress_mpi(input, output, table, root, f_size);
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
