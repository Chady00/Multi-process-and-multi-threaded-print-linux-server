#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <limits>
#include <sstream>
#include <queue>

#define INFINITY numeric_limits<double>::infinity()

using namespace std;

class Graph
{
public:
	unordered_set<string> vertices;
	unordered_map<string, vector<pair<string, double>>> edges;
	unordered_map<string, double> distance;
	unordered_map<string, int> updateCount; // To keep track of update count for each vertex

	// Constructors/Destructor for Graph.
	Graph();
	~Graph();

	// Loads the graph.
	void LoadGraph(char *graphFileName);

	// Runs Dijkstra's algorithm, using the specified vertex as the starting vertex.
	void RunDijkstra(char startVertex);

	// Returns the distance of the vertex with the specified name from the starting vertex.
	int GetDistance(char vertex);

	// Returns the number of updates made to the vertex's distance during the last call of RunDijkstra.
	int GetNumberOfUpdates(char vertex);

	void add_vertex(const string &vertex);

	void add_edge(const string &src, const string &dst, double weight);
};

#endif