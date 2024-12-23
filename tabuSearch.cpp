#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;
class Graph {
public:
    int V; 
    vector<vector<int>> adj;

    Graph(int V) {
        this->V = V;
        adj.resize(V);
    }

    void addEdge(int u, int v) {
        adj[u].push_back(v);
        adj[v].push_back(u);
    }
};

// Funkcja oceny, która zwraca liczbę konfliktów
int evaluate(const Graph& graph, const vector<int>& color) {
    int conflicts = 0;
    for (int u = 0; u < graph.V; u++) {
        for (int neighbor : graph.adj[u]) {
            if (color[u] == color[neighbor])
                conflicts++;
        }
    }
    return conflicts / 2; // konflikt jest liczony dwukrotnie
}


void tabuSearch(Graph& graph, int maxColors, int maxIterations, int initialTabuTenure) {
    srand(time(0));

    vector<int> color(graph.V, 0);
    int currentConflicts = evaluate(graph, color);
    vector<vector<int>> tabuList(graph.V, vector<int>(maxColors, 0));

    int bestConflicts = currentConflicts;
    vector<int> bestColor = color;

    int tabuTenure = initialTabuTenure; // Początkowa długość tabu

    for (int iteration = 0; iteration < maxIterations; iteration++) {
        int bestNeighborConflicts = INT_MAX;
        int bestVertex = -1;
        int bestColorChoice = -1;

        // Znajdź najlepsze dozwolone przemieszczenie
        for (int v = 0; v < graph.V; v++) {
            int originalColor = color[v];
            for (int c = 0; c < maxColors; c++) {
                if (c != originalColor && tabuList[v][c] == 0) {
                    color[v] = c;
                    int newConflicts = evaluate(graph, color);

                    if (newConflicts < bestNeighborConflicts) {
                        bestNeighborConflicts = newConflicts;
                        bestVertex = v;
                        bestColorChoice = c;
                    }
                }
            }
            color[v] = originalColor;
        }

        // Aktualizuj rozwiązanie
        if (bestVertex != -1 && bestColorChoice != -1) {
            color[bestVertex] = bestColorChoice;
            currentConflicts = bestNeighborConflicts;
            tabuList[bestVertex][bestColorChoice] = tabuTenure;

            // Zmień długość tabu adaptacyjnie
            if (currentConflicts < bestConflicts) {
                bestConflicts = currentConflicts;
                bestColor = color;
                tabuTenure = initialTabuTenure; // Resetuj długość tabu
            } else {
                tabuTenure = min(tabuTenure + 1, graph.V); // Zwiększ długość tabu
            }
        }

        // Zmniejsz wartości tabu
        for (int v = 0; v < graph.V; v++) {
            for (int c = 0; c < maxColors; c++) {
                if (tabuList[v][c] > 0)
                    tabuList[v][c]--;
            }
        }

        if (bestConflicts == 0)
            break;
    }

    if (bestConflicts == 0) {
        cout << "Znaleziono rozwiązanie bez konfliktów." << endl;
    } else {
        cout << "Najlepsze znalezione rozwiązanie ma " << bestConflicts << " konfliktów." << endl;
    }

    set<int> uniqueColors(bestColor.begin(), bestColor.end());
    cout << "\nLiczba użytych kolorów: " << uniqueColors.size() << endl;
}

int main() {

    ifstream inputFile("./testCases/gc1000.txt");
    if (!inputFile) {
        cerr << "Błąd przy otwieraniu pliku" << endl;
        return 1;
    }

    int vertices;
    inputFile >> vertices;
    Graph graph(vertices);

    int u, v;
    while (inputFile >> u >> v) {
        graph.addEdge(u - 1, v - 1);  // wierzchołki są numerowane od 1
    }

    inputFile.close();

    // Parametry 
    int maxColors = 154;
    int maxIterations = 2000;
    int initialTabuTenure = 13;

    tabuSearch(graph, maxColors, maxIterations, initialTabuTenure);

    return 0;
}
