#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

using namespace std;

class Graph {
    public:
        Graph(int vertices);
        void addEdge(int u, int v);
        vector<int> greedyColoring();

    private:
        int V; 
        vector<vector<int>> adj; 
};

Graph::Graph(int vertices) : V(vertices) {
    adj.resize(V);
}

void Graph::addEdge(int u, int v) {
    adj[u].push_back(v);
    adj[v].push_back(u);
}

vector<int> Graph::greedyColoring() {
    vector<int> result(V, -1); 
    vector<bool> available(V, false); 

    result[0] = 0;

    for (int u = 1; u < V; ++u) {
        for (int neighbor : adj[u]) {
            if (result[neighbor] != -1) {
                available[result[neighbor]] = true;
            }
        }

        int color;
        for (color = 0; color < V; ++color) {
            if (!available[color]) {
                break;
            }
        }

        result[u] = color;

        fill(available.begin(), available.end(), false);
    }

    return result; 
}

int main() {
    ifstream inputFile("tests.txt");
    if (!inputFile) {
        cerr << "Error with opening file tests.txt" << endl;
        return 1;
    }

    int vertices;
    inputFile >> vertices;
    Graph g(vertices);

    int u, v;
    while (inputFile >> u >> v) {
        g.addEdge(u - 1, v - 1);  // Zakładając, że wierzchołki są numerowane od 1
    }

    inputFile.close();


    vector<int> coloring = g.greedyColoring();

    cout << "Colors assigned to the vertices:\n";
    for (int i = 0; i < coloring.size(); ++i) {
        cout << "Vertice " << i << ": color " << ++coloring[i] << "\n";
    }

    int maxColor = *max_element(coloring.begin(), coloring.end()) + 1;
    cout << "\nLiczba użytych kolorów: " << maxColor << endl;

    return 0;
}

// generowanie instancji 
// oraz ich wczytywanie 


// g++ -std=c++17  greedyAlgorithm.cpp -o greedyAlgorithm && ./greedyAlgorithm 