#include "graph.h"

// Constructor/Destructor for Graph.

using namespace std;

Graph::Graph()
{
    // Initialize the member variables
    this->vertices = unordered_set<string>();
    this->edges = unordered_map<string, vector<pair<string, double>>>();
    this->distance = unordered_map<string, double>();
    this->updateCount = unordered_map<string, int>();
}
Graph::~Graph()
{

    // Clean up vertices
    vertices.clear();

    // Clean up edges
    for (auto it = edges.begin(); it != edges.end(); ++it)
    {
        it->second.clear();
    }
    edges.clear();

    // Clean up distance
    distance.clear();

    // Clean up updateCount
    updateCount.clear();
}

void Graph::add_vertex(const string &vertex)
{
    this->vertices.insert(vertex);
    this->distance[vertex] = -1;
}

void Graph::add_edge(const string &src, const string &dst, double weight)
{

    this->edges[src].emplace_back(dst, weight);
}

void Graph::LoadGraph(char *graphFileName)
{
    ifstream file(graphFileName);
    if (file.is_open())
    {
        string line;
        bool edgesSection = false;
        while (getline(file, line))
        {
            if (!line.empty() && line[0] != '#')
            { // Ignore comments and empty lines
                if (line == "[Edges]")
                {
                    edgesSection = true;
                    continue;
                }
                if (edgesSection)
                {
                    string src, dst;
                    double weight;
                    istringstream iss(line);

                    if (iss >> src >> dst >> weight)
                    {
                        if (weight < 0)
                        {
                            // cout << "Error: Negative edge weight encountered in graph file." << endl;
                            this->~Graph();
                            return;
                        }
                        if (src != dst) // Check if source and destination vertices are different
                        {
                            add_vertex(src);
                            // cout << "I added " << src << endl;
                            add_vertex(dst);
                            add_edge(src, dst, weight);
                        }
                    }
                }
                else
                {
                    if (line == "[Vertices]")
                        continue;
                    add_vertex(line);
                }
            }
        }
        // for (const auto &vertex : this->vertices)
        // {
        //     std::cout << vertex << " ";
        // }
        // cout << endl
        //      << "edges:" << endl;
        // for (const auto &vertex : edges)
        // {
        //     std::cout << "Vertex: " << vertex.first << " - Edges: ";
        //     for (const auto &edge : vertex.second)
        //     {
        //         std::cout << "(" << edge.first << ", " << edge.second << ") ";
        //     }
        //     std::cout << std::endl;
        // }
        file.close();
    }
    else
    {
        cerr << "Failed to open file: " << graphFileName << endl;
        this->~Graph();
        return;
    }
}

// Runs Dijkstra's algorithm, using the specified vertex as the starting vertex.
void Graph::RunDijkstra(char startVertex)
{
    // Initialize distances to infinity for all vertices
    for (const auto &vertex : vertices)
    {
        distance[vertex] = INFINITY;
    }

    // Set distance of startVertex to 0

    string myString(1, startVertex);
    distance[myString] = 0;

    // Create a priority queue to store vertices to visit
    priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;
    pq.push(make_pair(0, myString));

    while (!pq.empty())
    {
        string currentVertex = pq.top().second;
        double currentDistance = pq.top().first;
        pq.pop();

        // Skip if already visited
        if (currentDistance > distance[currentVertex])
        {
            continue;
        }

        // Update distances of adjacent vertices
        for (const auto &edge : edges[currentVertex])
        {
            string adjacentVertex = edge.first;
            double edgeWeight = edge.second;

            double newDistance = currentDistance + edgeWeight;
            if (newDistance < distance[adjacentVertex])
            {
                distance[adjacentVertex] = newDistance;
                pq.push(make_pair(newDistance, adjacentVertex));
                updateCount[adjacentVertex]++;
            }
        }
    }
    // for (const auto &vertexDistancePair : distance)
    // {
    //     std::cout << "Vertex: " << vertexDistancePair.first << ", Distance: " << vertexDistancePair.second << std::endl;
    // }
}

// Returns the distance of the vertex with the specified name from the starting vertex.
int Graph::GetDistance(char vertex)
{
    std::string myString(1, vertex);
    if (distance.find(myString) != distance.end()) // if found
    {
        return distance[myString];
    }
    return -1;
}

// Returns the number of updates made to the vertex's distance during the last call of RunDijkstra.
int Graph::GetNumberOfUpdates(char vertex)
{
    string vertexName(1, vertex);
    if (distance.find(vertexName) != distance.end()) // if vertex found
    {
        if (updateCount.find(vertexName) != updateCount.end()) // if update count exists
        {
            return updateCount[vertexName];
        }
        return 0;
    }
    return -1; // vertex not found
}