#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <cstdlib>
#include <ctime>

using namespace std;

void generateGraph(int vertices, int edges, const string& filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Error opening file!\n";
        return;
    }

    file << vertices << "\n";

    set<pair<int, int>> uniqueEdges;
    srand(time(0)); 

    while (uniqueEdges.size() < edges) {
        int u = rand() % vertices;
        int v = rand() % vertices;

        if (u != v && uniqueEdges.find({min(u, v), max(u, v)}) == uniqueEdges.end()) {
            uniqueEdges.insert({min(u, v), max(u, v)});
        }
    }

    for (const auto& edge : uniqueEdges) {
        file << edge.first + 1 << " " << edge.second + 1 << "\n";
    }

    file.close();
    cout << "Graf został zapisany do pliku: " << filename << "\n";
}

int main() {
    int vertices, edges;
    string filename;
    vertices = 100;
    edges = 500;
    cout << "Podaj nazwę pliku do zapisu grafu: ";
    cin >> filename;

    // Sprawdzamy, czy liczba krawędzi jest dopuszczalna 
    if (edges > vertices * (vertices - 1) / 2) {
        cerr << "Za dużo krawędzi dla podanej liczby wierzchołków!\n";
        return 1;
    }

    generateGraph(vertices, edges, filename);

    return 0;
}
