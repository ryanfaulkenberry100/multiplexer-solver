#include <stdio.h>
#include <stdlib.h>
#include "syntax_tree.h"


#define POP_SIZE 10
#define NUM_GENERATIONS 20
#define MAX_TREE_SIZE 20
#define CROSSOVER_RATE 0.7
#define MUTATION_RATE 0.001

#define MULTIPLEX_IN_SZ 2
#define MULTIPLEX_OUT_SZ (1 << MULTIPLEX_IN_SZ)
#define NUM_CONFIGURATIONS MULTIPLEX_OUT_SZ



/**
 * Fitness case
 */
typedef struct multiplexer_ {
	int input[MULTIPLEX_IN_SZ];
	int output[MULTIPLEX_OUT_SZ];
} multiplexer;

/**
 * Function declarations
 */
void initMul(multiplexer*, const unsigned int);
void initMulTable(multiplexer*);
int computeFitness(node*, multiplexer*);
void computePopulationFitness(node*, multiplexer*, int*);
void generateLookupTable(node*, int*, float*);
int weightedSelect(node*, float*);
int callTree(node*, multiplexer*);
int duplicate(node*, node*, float*, int*);
void mutate(node*);
void crossover(node*, node*);
void getNextGen(node*, node*, int*);
void generatePopulation(node*);
node* generateTree(unsigned int, node*);

