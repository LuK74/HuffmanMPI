#include "simpl_tbl.h"

/*
 * We'll use the following convention
 * Left = 0
 * Right = 1 
 * for the encoding
 */

/*
 * Also a node with a frequency of -1 we'll be a leaf
 * And in this case, we'll be interested in the "value" field
 */

struct node {
  int frequency;
  char value;
  struct node * left;
  struct node * right;
};

typedef struct node * tree;

/*
 * Basic tree functions
 */
void add_left(tree parent, tree left);
void add_right(tree parent, tree right);
tree new_node(char value, int frequency, tree left, tree right);

/*
 * Huffman export to table
 */
s_table convert_to_tbl(tree root);

/*
 * Export & Import functions
 */
void export_tree(tree root, char * filename);
tree inport_tree(tree root, char * filename);
