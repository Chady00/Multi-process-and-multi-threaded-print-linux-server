#include <stdio.h>
#include <stdlib.h>

#include "graph.h"
#include "infinity.h"

// Returns the value of INFINITY.
int GetINFINITY()
{
	return -1;
}

/* You may use this file to test your program.
	This file will not be submitted, as a different test5.cpp will be used.
  Every required method is included here so that you can verify that
		you have included everything the test engine will call. */
int main(int argv, char **argc)
{
	Graph mainGraph; // Graph to use for testing

	mainGraph.LoadGraph("graph.txt");

	mainGraph.RunDijkstra('a');

	mainGraph.GetDistance('b');
	mainGraph.GetNumberOfUpdates('b');
}
