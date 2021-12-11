#include <stdlib.h>
#include <stdio.h>
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

tree new_node(char value, int frequency, int flag, tree left, tree right) {
  tree node = malloc(sizeof(struct node));
  node->value = value;
  node->left = left;
  node->right = right;
  node->flag = flag;
  node->frequency = frequency;

  return node;
}

tree node_from_entry(s_entry entry) {
  tree node = malloc(sizeof(struct node));
  node->value = entry->key;
  node->left = NULL;
  node->right = NULL;
  node->flag = -1;
  node->frequency = entry->frequency;

  return node;
}

/*
 * Huffman export to table
 */
s_table convert_to_tbl(tree root) {
  s_table table = init_table(0, 0, NULL); 
  
  return table;
}

/*
 * Huffman tree from table
 */
tree convert_from_tbl(s_table table) {
  tree parent = NULL;
  tree right = NULL;
  tree left = NULL;

  tree previous = NULL;

  int i;
  
  for (i = table->n_entries-1; i >= 1; i = i - 2) {
    parent = new_node(0, -1, 0, NULL, NULL);
    left = node_from_entry((table->entries)[i]);
    right = node_from_entry((table->entries)[i-1]);

    add_left(parent, left);
    add_right(parent, right);

    if (previous != NULL) {
      left = previous;
      right = parent;
      parent = new_node(0, -1, 0, NULL, NULL);

      add_left(parent, left);
      add_right(parent, right);
    }
    
    previous = parent;
  }

  if (i == 0) {
    left = previous;
    right = node_from_entry((table->entries)[0]);
    parent = new_node(0, -1, 0, NULL, NULL);

    add_left(parent, left);
    add_right(parent, right);
    
  } else {
    parent = previous;
  }
  
  return parent;  
}


/*
 * Export & Import functions
 */
void explore_tree(tree node, int encoding, int depth, FILE * fp) {
  if (node->flag == -1) {
    char * encoding_str = malloc(sizeof(char)*(depth+3));
    for (int i = 0; i < depth; i++) {
      encoding_str[depth+1-i] = '0' + ((encoding>>i)&1);
    }
    encoding_str[depth+2] = '\n';
    encoding_str[0] = node->value;
    encoding_str[1] = ':';

    fprintf(fp, encoding_str);
  } else {
    if (node->left != NULL) {
      explore_tree(node->left, encoding<<1, depth+1, fp);
    }
    if (node->right != NULL) {
      explore_tree(node->right, (encoding<<1)+1, depth+1, fp);
    }
  }
}

void export_tree(tree root, char * filename) {
  FILE * file = fopen(filename, "w");
  if (root->flag == -1) {
    char * encoding_str = malloc(sizeof(char)*4);
    encoding_str[3] = '\n';
    encoding_str[2] = root->value;
    encoding_str[1] = ':';
    encoding_str[0] = '0';

    fprintf(file, encoding_str);
  } else {
    if (root->left != NULL) {
      explore_tree(root->left, 0, 1, file);
    }
    if (root->right != NULL) {
      explore_tree(root->right, 1, 1, file);
    }
  }

  fclose(file);
}

tree inport_tree(tree root, char * filename);
