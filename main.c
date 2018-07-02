/**
 * High-level Descriptions:
 *
 *  * A syntax tree is a function which may be called with a multiplexer as input.
 *  	Syntax trees are composed of function nodes, which accept bitwise inputs,
 *  	and terminal nodes, which are variables that each represent one input line
 *  	of a multiplexer.
 *  * A multiplexer is a collection of bitwise inputs and outputs, where input
 *  	configurations map to output configurations in the usual way.
 */

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "syntax_tree.h"
#include "mult_solver.h"


void printTree(node* tree) {

	printf("(");

	if (tree->isTerminal) {
		// If value < MUL_IN_SZ, this terminal is an input val, else output val
		if (tree->terminalVal < MULTIPLEX_IN_SZ) {
			printf("i[%d] ", tree->terminalVal);
		} else {
			printf("o[%d] ", tree->terminalVal - MULTIPLEX_IN_SZ);
		}
	}
	switch (tree->type) {
	case NOT:
		printf("not (");
		printTree(tree->children[0]);
		printf(") ");
		break;
	case AND:
		printf("and (");
		printTree(tree->children[0]);
		printTree(tree->children[1]);
		printf(") ");
		break;
	case OR:
		printf("or (");
		printTree(tree->children[0]);
		printTree(tree->children[1]);
		printf(") ");
		break;
	case IF:
		printf("if (");
		printTree(tree->children[0]);
		printTree(tree->children[1]);
		printTree(tree->children[2]);
		printf(") ");
		break;
	}
	printf(")");
}

void printInfo(node* population, node* offspring) {

	printf("Population:\n");
	for (int i=0; i < POP_SIZE; i++) {
		printTree(population+i);
		printf("\n");
	}
}

int main(int argc, char** argv) {

	srand(time(NULL));
	srand48(time(NULL));

	int fitness[POP_SIZE];
	node population[POP_SIZE];
	node offspring[POP_SIZE];
	multiplexer mulTable[NUM_CONFIGURATIONS];

	initMulTable(mulTable);
	generatePopulation(population);

	int i;
	for (i=0; i<NUM_GENERATIONS; i++) {

		computePopulationFitness(population, mulTable, fitness);
		getNextGen(population, offspring, fitness);
		printInfo(population, offspring);
		freePop(population);
	}
	return 0;
}
