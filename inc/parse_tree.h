
#define OPERATOR_MAX_ARGS 3

/**
 * Function types
 */
enum fType {
		AND,
		OR,
		NOT,
		IF
};

/**
 * Node in the parse tree. Can be either a function, whose children are either
 * functions or terminals, or a terminal, which has no children.
 */
typedef struct node_ {
	int isTerminal;
	union {
		int terminalVal;
		struct {
			enum fType type;
			struct node_* children[OPERATOR_MAX_ARGS];
		};
	};
} node;

/**
 * Container for the root node of an algorithm and metadata for the parse tree.
 */
typedef struct algorithm_ {
	node* root;
	int nodeCt;
} algorithm;
