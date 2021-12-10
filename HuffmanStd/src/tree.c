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


void explore_and_fill(tree node, s_table table) {
  if (node->frequency != -1) {
    s_entry entry = create_entry(node->value, node->frequency);
    add_entry(table, entry);
  }

  if (node->left != NULL) {
    explore_and_fill(node->left, table);
  }

  if (node->right != NULL) {
    explore_and_fill(node->right, table);
  }
}

/*
 * Huffman export to table
 */
s_table convert_to_tbl(tree root) {
  s_table table = init_table(0, 0, NULL); 

  if (root != NULL) {
    explore_and_fill(root, table);
  }
  
  return table;
}

/*
 * Export & Import functions
 */
void export_tree(tree root, char * filename);
tree inport_tree(tree root, char * filename);
