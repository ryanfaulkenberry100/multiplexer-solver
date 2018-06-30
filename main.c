#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "inc/parse_tree.h"

#define FALSE 0
#define TRUE 1
#define POP_SIZE 10
#define MULTIPLEX_IN_SZ 2
#define MULTIPLEX_OUT_SZ (1 << MULTIPLEX_IN_SZ)
#define NUM_CONFIGURATIONS MULTIPLEX_OUT_SZ
#define NUM_GENERATIONS 20
#define CROSSOVER_RATE 0.7
#define MUTATION_RATE 0.001

/**
 * Fitness case
 */
typedef struct multiplexer_ {
	int input[MULTIPLEX_IN_SZ];
	int output[MULTIPLEX_OUT_SZ];
} multiplexer;

/**
 * Initializes the provided fitness case to correspond to the provided value.
 * Since this is a multiplexer, the input array is the binary value of "value",
 * and the output array is all 0s except for output[value], which is 1.
 * Assumes that value is no greater than exp(2, MULTIPLEX_IN_SZ) - 1.
 */
void initMul(multiplexer* mul, const unsigned int value) {

	for (int i=0; i<MULTIPLEX_IN_SZ; i++) {
		mul->input[i] = value & (1 << i);
	}
	for (int i=0; i<MULTIPLEX_OUT_SZ; i++) {
		mul->output[i] = 0;
	}
	mul->output[value] = 1;
}

/**
 * Populates the provided fitcase array with every possible multiplexer state.
 * This function assumes that the global multiplexer has a number of output
 * lines equal to exp(2, the number of input lines), and that the provided
 * array's size is greater than or equal to the number of possible multiplexer
 * configurations (also equal to the number of output lines).
 */
void initMulTable(multiplexer* mulTable) {

	int i, j;
	for (i=0; i<NUM_CONFIGURATIONS; i++) {
		initMul(mulTable+i, i);
	}
}

/**
 * Recursively traverses the tree, substitutes terminal multiplexer indices
 * with the actual value contained in the provided multiplexer, and parses the
 * algorithm, such that the root call returns some value {0,1}.
 */
int recParseTree(node* n, multiplexer* mul) {

	if (n->isTerminal) {
		// If value < MUL_IN_SZ, this terminal is an input val, else output val
		if (n->terminalVal < MULTIPLEX_IN_SZ) {
			return mul->input[n->terminalVal];
		} else {
			return mul->output[n->terminalVal - MULTIPLEX_IN_SZ];
		}
	} else {
		switch (n->type) {
		case AND:
			return recParseTree(n->children[0], mul) &
					recParseTree(n->children[1], mul);
		case OR:
			return recParseTree(n->children[0], mul) |
					recParseTree(n->children[1], mul);
		case NOT:
			return (recParseTree(n->children[0], mul) == 0) ?
					1 : 0;
		case IF:
			return (recParseTree(n->children[0], mul) == 0) ?
					recParseTree(n->children[1], mul) :
					recParseTree(n->children[2], mul);
		default:
			fprintf(stderr, "Corrupted parse tree node.\n");
			exit(0);
		}
	}
}

/**
 * Returns the result of computing the algorithm on the provided multiplexer.
 * If the algorithm returns 1, it has successfully determined the value of
 * the multiplexer's output bit, and gets a fitness point (note that the
 * alg must return some mul index, not simply "1").
 *
 * TODO: remove if no metadata processing is needed
 */
int attemptCase(multiplexer* mul, algorithm* solution) {

	return recParseTree((node*) solution, mul);
}

/**
 * Returns the fitness of the provided individual based on the number of
 * multiplexer outputs it correctly predicts when given the inputs from
 * the mulTable.
 */
int computeFitness(algorithm* solution, multiplexer* mulTable) {

	int i, fitness;
	for (i=0; i<NUM_CONFIGURATIONS; i++) {
		fitness += attemptCase(mulTable+i, solution);
	}
	return fitness;
}

/**
 * Computes the fitness of each member i of the population and stores the result
 * in fitnessTable[i]. Assumes that fitnessTable's size is greater than or equal
 * to the population size.
 */
void computePopulationFitness(algorithm* population, multiplexer* mulTable, int* fitnessTable) {

	int i;
	for (i=0; i<POP_SIZE; i++) {
		fitnessTable[i] = computeFitness(population+i, mulTable);
	}
}

/**
 * Generates a probability table for each member of the given population
 * such that individuals with greater fitness have a higher probability of
 * being chosen in the rouletteSelect.
 */
void generateLookupTable(algorithm* population, int* fitness, float* probTable) {

	int totalFit = 0;
	int i;
	for (i=0; i<POP_SIZE; i++) {
		totalFit += fitness[i];
	}

	float previousProbability = 0.0;
	float probability;
	for (i=0; i<POP_SIZE; i++) {
		probability = (float) fitness[i] / (float) totalFit;
	    probTable[i] = previousProbability + probability;
	    previousProbability += probability;
	}
}

/**
 * Duplicates ~10% of the population to the offspring group based on a
 * fitness-weighted selection, and zeroes their fitness and recomputes the
 * probability table so that they can't reproduce again. Returns the number of
 * individuals copied.
 */
int duplicate(algorithm* population, algorithm* offspring, float* probTable, int* fitness) {

	int i, index;

	// Number of duplicates must be even
	int tenthPop = (POP_SIZE / 10) % 2 == 0 ?
			POP_SIZE / 10 :
			(POP_SIZE / 10) + 1;
	for (i=0; i < tenthPop; i++) {
		index = weightedSelect(population, probTable);
		copyTree(population + index, offspring[i]);
		fitness[index] = 0; // No longer eligible for reproduction
	}
	generateLookupTable(population, fitness, probTable);
	return i;
}

/**
 * Copies a tree from node src to node dest
 */
void copyTree(node* src, node* dest) {

	node new;
	if (src->isTerminal) {

	}
}

/**
 * Copies algorithm src to algorithm dest
 */
void copyAlg(algorithm* src, algorithm* dest) {

	dest->root = copyTree(src->root);
	dest->nodeCt = src->nodeCt;
}

/**
 * Using a probabilistic, fitness-proportionate weighted selection, choose 10%
 * of the population to be duplicated exactly, and choose the other 90% to be
 * duplicated with crossover, then subjected to a chance of mutation.
 */
void getNextGen(algorithm* population, algorithm* offspring, int* fitness) {

	int locus;
	int i, index;
	float r;
	float probTable[POP_SIZE];
	algorithm* child1, child2;


	// Generate a lookup table for a fitness-weighted random selection
	generateLookupTable(population, fitness, probTable);

	// Copy ~10% of the population with no crossover
	int numCopied = duplicate(population, offspring, probTable, fitness);

	// Copy members from the other ~90% with crossover
	for (i=numCopied; i<POP_SIZE/2; i++) {

		child1 = offspring + i;
		child2 = offspring +
				(POP_SIZE + numCopied) / 2 + i; //numCopied guaranteed even

		// Select two parents to produce offspring
		copyAlg(&population[weightedSelect(population, probTable)], child1);
		copyAlg(&population[weightedSelect(population, probTable)], child2);

		crossover(child1, child2);
		mutate(child1);
		mutate(child2);
	}
}

int main(int argc, char** argv) {

	srand(time(NULL));
	srand48(time(NULL));

	int fitness[POP_SIZE];
	algorithm population[POP_SIZE];
	algorithm offspring[POP_SIZE];
	multiplexer mulTable[NUM_CONFIGURATIONS];

	initFitnessCases(mulTable);
	generatePopulation(population);

	int i;
	for (i=0; i<NUM_GENERATIONS; i++) {

		computePopulationFitness(population, mulTable, fitness);
		getNextGen(population, offspring, fitness);
		printInfo(population, offspring);
	}
	return 0;
}
