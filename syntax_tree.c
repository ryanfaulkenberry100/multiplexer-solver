/**
 * Functions that act on the structures defined in parse_tree.h
 */
#include "inc/syntax_tree.h"

/**
 * Returns a copy of the provided src tree
 */
node* copyTree(node* src, node* parent) {

	node* dest = (node*)malloc(sizeof(node));;
	dest->parent = parent;

	if (src->isTerminal) {
		dest->isTerminal = 0;
		dest->terminalVal = src->terminalVal;
		dest->subtreeSize = 1;
	} else {
		dest->isTerminal = 1;
		dest->type = src->type;
		switch (dest->type) {
		case NOT:
			dest->children[0] = copyTree(src->children[0], src);
			dest->subtreeSize = dest->children[0]->subtreeSize + 1;
			break;
		case AND:
		case OR:
			dest->children[0] = copyTree(src->children[0], src);
			dest->children[1] = copyTree(src->children[1], src);
			dest->subtreeSize = dest->children[0]->subtreeSize +
								dest->children[1]->subtreeSize + 1;

			break;
		case IF:
			dest->children[0] = copyTree(src->children[0], src);
			dest->children[1] = copyTree(src->children[1], src);
			dest->children[2] = copyTree(src->children[2], src);
			dest->subtreeSize = dest->children[0]->subtreeSize +
								dest->children[1]->subtreeSize +
								dest->children[2]->subtreeSize + 1;
			break;
		}
	}
	return dest;
}



/**
 * Given some node of the tree whose size has changed, updates the
 * subtree size metadata of all it's parent/ancestor nodes back to root.
 */
void resize(node* n) {

	while ((n = n->parent)) {
		switch (n->type) {
		case NOT:
			n->subtreeSize = n->children[0]->subtreeSize + 1;
			break;
		case AND:
		case OR:
			n->subtreeSize =
					n->children[0]->subtreeSize +
					n->children[1]->subtreeSize + 1;
			break;
		case IF:
			n->subtreeSize =
					n->children[0]->subtreeSize +
					n->children[1]->subtreeSize +
					n->children[2]->subtreeSize + 1;
			break;
		}
	}
}

/**
 * Return pointer to the nth node of the tree. Assumes n <= tree->subtreeSize.
 * Nodes are ordered in a leftmost depth-first fashion, such that if a parent
 * is node k, it's leftmost child is node k+1, whose leftmost child is k+2, etc.
 */
node* getNode(node* tree, unsigned int n) {

	if (n == 0 || tree == NULL) {
		return tree;
	}

	n--;
	switch (tree->type) {
	case NOT:
		return getNode(tree->children[0], n);
	case AND:
	case OR:
		if (n < tree->children[0]->subtreeSize) {
			return getNode(tree->children[0], n);
		} else {
			return getNode(tree->children[1], n - tree->children[0]->subtreeSize);
		}
	case IF:
		if (n < tree->children[0]->subtreeSize) {
			return getNode(tree->children[0], n);
		} else  if (n - tree->children[0]->subtreeSize
					< tree->children[1]->subtreeSize) {
			return getNode(tree->children[1], n - tree->children[0]->subtreeSize);
		} else {
			return getNode(tree->children[2],
					(n - tree->children[1]->subtreeSize) - tree->children[0]->subtreeSize);
		}
	}
	fprintf(stderr, "getNode failed\n");
	return NULL;
}
