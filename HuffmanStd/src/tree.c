#include <stdlib.h>
#include "tree.h"


/*
 * Basic tree functions
 */
void add_left(tree parent, tree left) {
  parent->left = left;
}

void add_right(tree parent, tree right) {
  parent->right = right;
}

tree new_node(char value, int frequency, tree left, tree right) {
  tree node = malloc(sizeof(tree));
  node->value = value;
  node->left = left;
  node->right = right;
  node->frequency = frequency;

  return node;
}

/*
 * Huffman export to table
 */
s_table convert_to_tbl(tree root) {
  s_table table = init_table(0, 0, NULL); 
  
  return NULL;
}

/*
 * Export & Import functions
 */
void export_tree(tree root, char * filename);
tree inport_tree(tree root, char * filename);
