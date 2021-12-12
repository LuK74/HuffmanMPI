#include <stdio.h>
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
  float frequency;
  char value;
  int flag; // Set to -1 if leaf, if not set to 0
  struct node * left;
  struct node * right;
};

typedef struct node * tree;

/*
 * Basic tree functions
 */
void add_left(tree parent, tree left);
void add_right(tree parent, tree right);
tree new_node(char value, int frequency, int flag, tree left, tree right);

/*
 * Huffman export to table
 */
s_table convert_to_tbl(tree root);

/*
 * HUffman tree from table
 */
tree convert_from_tbl(s_table table);

/*
 * Fill encoding fields of a table
 */
void fill_encoding(tree root, s_table);

/*
 * Export & Import functions
 */
void export_tree(tree root, char * filename);
void write_tree(tree root, FILE * file);
tree read_tree(FILE * input, int * padding);
