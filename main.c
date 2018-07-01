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
	}
	return 0;
}
