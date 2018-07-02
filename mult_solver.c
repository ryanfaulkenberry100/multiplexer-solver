/**
 *
 */

#include "mult_solver.h"
#include "syntax_tree.h"

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

	int i;
	for (i=0; i<NUM_CONFIGURATIONS; i++) {
		initMul(mulTable+i, i);
	}
}

/**
 * Frees the memory stored in the population array
 */
void freePop(node* population) {

	for (int i=0; i < POP_SIZE; i++) {
		freeTree(population + i);
	}
}

/**
 * Helper function to randomly divide a number into 2 nonzero numbers whose sum
 * equals the original.
 */
void divide2(unsigned int* src, unsigned int* dest) {
	*dest = (rand() % (*src - 1)) + 1;
	*src -= *dest;
}

/**
 * Helper function to randomly divide a number into 3 numbers whose sum equals
 * the original. Results are swapped around at random to account for
 * non-uniformity of the distribution
 */
void divide3(unsigned int* src, unsigned int* dest1, unsigned int* dest2) {

	*dest1 = (rand() % (*src - 1)) + 1;
	*src -= *dest1;
	*dest2 = (rand() % (*src - 1)) + 1;
	*src -= *dest2;

	// shuffle
	unsigned int* temp;
	if (rand() % 2) {
		temp = src;
		src = dest1;
		dest1 = temp;
	}
	if (rand() % 2) {
		temp = dest2;
		dest2 = src;
		src = temp;
	}
	if (rand() % 2) {
		temp = dest2;
		dest2 = dest1;
		dest1 = temp;
	}
}

/**
 * Returns a randomly generated syntax tree with n nodes.
 */
node* generateTree(unsigned int r, node* parent) {

	node* n = (node*)malloc(sizeof(node));
	int type;
	unsigned int s, t;

    n->subtreeSize = r;
    n->isTerminal = 0;
    n->parent = parent;

    switch (n->subtreeSize) {
    case 1:
    	n->isTerminal = 1;
		n->terminalVal = rand() % MULTIPLEX_IN_SZ + MULTIPLEX_OUT_SZ;
		return n;
    case 2:
    	n->type = NOT;
    	n->children[0] = generateTree(1, n);
    	break;
    case 3:
    	n->type = rand() % 2 ? AND : OR;
    	n->children[0] = generateTree(1, n);
    	n->children[1] = generateTree(1, n);
    	break;
    default:
    	r--;
    	type = rand() % 3;
    	switch (type) {
    	case 0:
    		n->type = NOT;
    		n->children[0] = generateTree(r, n);
    		break;
		case 1:
			n->type = rand() % 2 ? AND : OR;
			divide2(&r, &s);
			n->children[0] = generateTree(r, n);
			n->children[1] = generateTree(s, n);
			break;
		case 2:
			n->type = IF;
			divide3(&r, &s, &t);
			n->children[0] = generateTree(r, n);
			n->children[1] = generateTree(s, n);
			n->children[2] = generateTree(t, n);
			break;
    	}
    	break;
    }
    fprintf(stderr, "Tree generation failed\n");
    return NULL;
}


/**
 * Returns an array of randomly generated syntax trees.
 */
void generatePopulation(node* population) {

	for (int i=0; i < POP_SIZE; i++) {
		population[i] = *(generateTree((rand() % (MAX_TREE_SIZE - 1)) + 1, NULL));
	}
}


/**
 * Returns the fitness of the provided individual based on the number of
 * multiplexer outputs it correctly predicts when given the inputs from
 * the mulTable.
 */
int computeFitness(node* solution, multiplexer* mulTable) {

	int i, fitness;
	for (i=0; i<NUM_CONFIGURATIONS; i++) {
		fitness += callTree(solution, mulTable+i);
	}
	return fitness;
}

/**
 * Computes the fitness of each member i of the population and stores the result
 * in fitnessTable[i]. Assumes that fitnessTable's size is greater than or equal
 * to the population size.
 */
void computePopulationFitness(node* population, multiplexer* mulTable, int* fitnessTable) {

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
void generateLookupTable(node* population, int* fitness, float* probTable) {

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
 * Selects a member of the given population such that probability of
 * selection is a function of fitness, using a provided probability
 * table. Returns the index at which the chosen member may be found
 * in the population array.
 */
int weightedSelect(node* population, float* probTable) {

    float r = (float)drand48();
    int i;

    // Find the bin which the random number fits
    for (i=0; i < POP_SIZE; i++) {
    	if (r < probTable[i]) {
    		return i;
    	}
    }
    // In case of floating point roundoff error
    return POP_SIZE - 1;
}


/**
 * A syntax tree is a function which may be called with a multiplexer as input.
 * The tree's stored multiplexer variables are replaced by the actual bitwise
 * inputs/outputs of the provided multiplexer, and the tree is executed to
 * produce a result bit.
 */
int callTree(node* n, multiplexer* mul) {

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
			return callTree(n->children[0], mul) &
					callTree(n->children[1], mul);
		case OR:
			return callTree(n->children[0], mul) |
					callTree(n->children[1], mul);
		case NOT:
			return (callTree(n->children[0], mul) == 0) ?
					1 : 0;
		case IF:
			return (callTree(n->children[0], mul) == 1) ?
					callTree(n->children[1], mul) :
					callTree(n->children[2], mul);
		default:
			fprintf(stderr, "Corrupted parse tree node.\n");
			exit(0);
		}
	}
}

/**
 * Duplicates ~10% of the population to the offspring group based on a
 * fitness-weighted selection, and zeroes their fitness and recomputes the
 * probability table so that they can't reproduce again. Returns the number of
 * individuals copied.
 */
int duplicate(node* population, node* offspring, float* probTable, int* fitness) {

	int i, index;

	// Number of duplicates must be even
	int tenthPop = (POP_SIZE / 10) % 2 == 0 ?
			POP_SIZE / 10 :
			(POP_SIZE / 10) + 1;
	for (i=0; i < tenthPop; i++) {
		index = weightedSelect(population, probTable);
		offspring[i] = *(copyTree(population + index, NULL));
		fitness[index] = 0; // No longer eligible for reproduction
	}
	generateLookupTable(population, fitness, probTable);
	return i;
}

/**
 * Replace a randomly selected node on the provided tree with a randomly
 * generated subtree.
 */
void mutate(node* tree) {

}

/**
 * Execute crossover on the provided parents to produce mixed offspring.
 */
void crossover(node* child1, node* child2) {

	node* node1;
	node* node2;

	// Randomly select an index in each tree
	unsigned int index1 = rand() % child1->subtreeSize;
	unsigned int index2 = rand() % child2->subtreeSize;

	// Traverse the tree and get the indexed node
	node1 = getNode(child1, index1);
	node2 = getNode(child2, index2);

	// Swap the selected nodes of each tree
	for (int i=0; i<OPERATOR_MAX_ARGS; i++) {
		// Find the parent's child and replace with node from other tree
		if (node1->parent->children[i] == node1) {
			node1->parent->children[i] = node2;
			break;
		}
	}
	for (int i=0; i<OPERATOR_MAX_ARGS; i++) {
		// Find the parent's child and replace with node from other tree
		if (node2->parent->children[i] == node2) {
			node2->parent->children[i] = node1;
			break;
		}
	}

	// Recompute the subtree sizes of each tree.
	resize(node1);
	resize(node2);
}

/**
 * Using a probabilistic, fitness-proportionate weighted selection, choose 10%
 * of the population to be duplicated exactly, and choose the other 90% to be
 * duplicated with crossover, then subjected to a chance of mutation.
 */
void getNextGen(node* population, node* offspring, int* fitness) {

	int i;
	float probTable[POP_SIZE];
	node *child1, *child2;


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
		child1 = copyTree(&population[weightedSelect(population, probTable)], NULL);
		child2 = copyTree(&population[weightedSelect(population, probTable)], NULL);

		float r = drand48();
		if (r < CROSSOVER_RATE) {
			crossover(child1, child2);
		}
		mutate(child1);
		mutate(child2);

		offspring[i] = *child1;
		offspring[(POP_SIZE + numCopied) / 2 + i] = *child2;
	}
}
