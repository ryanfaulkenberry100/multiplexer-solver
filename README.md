# multiplexer-solver
A genetic algorithm which produces lisp-like functions that map a given multiplexer's input configuration to a correct output configuration. A population member's fitness is calculated by how many of the possible multiplexer states it solves correctly, and the functions with the greatest fitness are the most likely to be copied (wholly or partially) to the next generation of functions.

For example, suppose we're using a 2x4 multiplexer, and we have the following population member: 
```
(if (and i[0] o[1])
    o[3]
    o[2])
```
which says "If input line 0 and output line 1 are both true, then return output line 3, otherwise return output line 2 (where greater numbered lines are the least significant).

We determine this member's fitness by executing every possible multiplexer state on the population member, and for every case that returns "1" one point is added to the member's fitness.

For the multiplexer with input [0,1] and output [0,0,1,0], since (i[0] and o[1]) evaluates to false, the member will return o[2], which for this multiplexer, happens to be "1". Therefore the member will gain a fitness point for this case.

For the multiplexer with input [1,0] and output [0,1,0,0], since (i[0] and o[1]) evaluates to true, the member will return o[3], which for this multiplexer, happens to be "0". Therefore the member will not gain a fitness point for this case.

This member's total fitness score turns out to be 1 out of a possible 4, which is not so hot. Unless the rest of the population is as ugly as he is, his odds of passing a part of himself to the next generation are slim.

As intelligent humans, we can come up with a few members with perfect fitness pretty easily. For example, an unelegant nested-if solution: 

```
(if (and i[0] i[1])
    o[0]
    (if i[0]
        o[1]
        (if i[1]
            o[2]
            o[3]))))
```
But it's interesting to watch the population organically evolve to solve this problem, and the possible number of configurations with perfect fitness is bounded only by the maximum size of the population members.
