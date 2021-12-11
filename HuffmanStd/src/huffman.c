#include <stdio.h>
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



int main(int argc, char * argv[]) {
  FILE * file = fopen(argv[1], "r");
  s_table table = generate_table(file);
  tree root = convert_from_tbl(table);
  export_tree(root, "tree.txt");
  
  fclose(file);
  return 0;
}
