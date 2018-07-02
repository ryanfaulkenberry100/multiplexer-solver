#include <stdio.h>
#include <stdlib.h>

#ifndef PARSE_TREE_
#define PARSE_TREE_

/**
 * Type/function definitions for a simple syntax tree
 */
#define OPERATOR_MAX_ARGS 3

/**
 * Function types
 */
enum fType {
		AND,
		OR,
		NOT,
		IF,
		MAX_FUNCTION_TYPES
};

/**
 * Node in the parse tree. Can be either a function, whose children are either
 * functions or terminals, or a terminal, which has no children.
 */
typedef struct node_ {
	int isTerminal;
	int subtreeSize; // num of nodes beneath + this one.
	struct node_* parent;
	union {
		int terminalVal;
		struct {
			enum fType type;
			struct node_* children[OPERATOR_MAX_ARGS];
		};
	};
} node;

/**
 * Function definitions
 */
node* copyTree(node*, node*);
node* getNode(node*, unsigned int);
void resize(node*);

#endif //PARSE_TREE_
