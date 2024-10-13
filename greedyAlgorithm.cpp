#include <iostream>
#include <vector>
#include <algorithm>

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
    Graph g(5);
    g.addEdge(0, 1);
    g.addEdge(0, 2);
    g.addEdge(1, 2);
    g.addEdge(1, 3);
    g.addEdge(2, 3);
    g.addEdge(3, 4);

    vector<int> coloring = g.greedyColoring();

    cout << "Colors assigned to the vertices:\n";
    for (int i = 0; i < coloring.size(); ++i) {
        cout << "Vertice " << i << ": color " << coloring[i] << endl;
    }

    return 0;
}
