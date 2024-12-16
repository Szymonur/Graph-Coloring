#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <numeric>
#include <random>
#include <chrono> 

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

    vector<int> greedyColoring() {
        vector<int> result(V, -1);
        result[0] = 0; // Start coloring from the first vertex

        for (int u = 1; u < V; ++u) {
            vector<bool> usedColors(V, false);
            for (int neighbor : adj[u]) {
                if (result[neighbor] != -1) {
                    usedColors[result[neighbor]] = true;
                }
            }

            int color = 0;
            while (color < V && usedColors[color]) {
                ++color;
            }

            result[u] = color;
        }

        return result;
    }
};

int evaluate(const Graph& graph, const vector<int>& color) {
    int conflicts = 0;
    for (int u = 0; u < graph.V; ++u) {
        for (int neighbor : graph.adj[u]) {
            if (u < neighbor && color[u] == color[neighbor]) {
                ++conflicts;
            }
        }
    }
    return conflicts;
}

void pathRelinking(const vector<int>& initial, const vector<int>& target, vector<int>& bestSolution, int& bestConflicts, const Graph& graph) {
    vector<int> current = initial;
    int currentConflicts = evaluate(graph, current);

    for (size_t i = 0; i < initial.size(); ++i) {
        if (current[i] != target[i]) {
            current[i] = target[i];
            int newConflicts = evaluate(graph, current);
            if (newConflicts < bestConflicts) {
                bestConflicts = newConflicts;
                bestSolution = current;
            }
            currentConflicts = newConflicts;
        }
    }
}

void tabuSearch(Graph& graph, int maxColors, int maxIterations, int initialTabuTenure, vector<int>& initialColor) {
    using namespace std::chrono; // Umożliwia używanie chrono::steady_clock

    srand(time(0));
    vector<int> color = initialColor;
    int currentConflicts = evaluate(graph, color);

    vector<vector<int>> tabuList(graph.V, vector<int>(maxColors, 0));

    int bestConflicts = currentConflicts;
    vector<int> bestColor = color;
    int tabuTenure = initialTabuTenure;
    int noImprovementCount = 0;
    const int restartThreshold = 100;

    vector<vector<int>> eliteSolutions;
    vector<int> verticesByConflict(graph.V);
    iota(verticesByConflict.begin(), verticesByConflict.end(), 0);

    // liczenie kolorw  algorytmem zachłannym
    int initialColorsUsed = *max_element(initialColor.begin(), initialColor.end()) + 1;

    // Stwórz generator losowy i rozkład
    random_device rd;
    mt19937 g(rd());

    // Rozpocznij śledzenie czasu
    auto startTime = steady_clock::now();

    for (int iteration = 0; iteration < maxIterations; ++iteration) {
        // Sprawdź, czy upłynęło już zbyt dużo czasu


        int bestNeighborConflicts = INT_MAX;
        int bestVertex = -1;
        int bestColorChoice = -1;

        // Losowe przetasowanie wierzchołków
        shuffle(verticesByConflict.begin(), verticesByConflict.end(), g);

        for (int v : verticesByConflict) {
            int originalColor = color[v];
            for (int c = 0; c < maxColors; ++c) {
                if (c != originalColor) {
                    color[v] = c;
                    int newConflicts = evaluate(graph, color);

                    if ((tabuList[v][c] == 0 || newConflicts < bestConflicts) && newConflicts < bestNeighborConflicts) {
                        bestNeighborConflicts = newConflicts;
                        bestVertex = v;
                        bestColorChoice = c;
                    }
                }
            }
            color[v] = originalColor; // Reset color
        }

        if (bestVertex != -1 && bestColorChoice != -1) {
            color[bestVertex] = bestColorChoice;
            currentConflicts = bestNeighborConflicts;
            tabuList[bestVertex][bestColorChoice] = tabuTenure;

            if (currentConflicts < bestConflicts) {
                bestConflicts = currentConflicts;
                bestColor = color;
                eliteSolutions.push_back(bestColor);
                tabuTenure = initialTabuTenure;
                noImprovementCount = 0;
            } else {
                tabuTenure = min(tabuTenure + 1, graph.V);
                noImprovementCount++;
            }
        }

        // Zmniejszenie wartości tabu dla wszystkich elementów
        for (int v = 0; v < graph.V; ++v) {
            for (int c = 0; c < maxColors; ++c) {
                if (tabuList[v][c] > 0) {
                    tabuList[v][c]--;
                }
            }
        }

        // Sprawdzenie warunku zakończenia: liczba kolorów w najlepszym rozwiązaniu
        set<int> uniqueColors(bestColor.begin(), bestColor.end());
        if (uniqueColors.size() < initialColorsUsed - 1) {
            cout << "Tabu search found a better solution with " << bestConflicts << " conflicts.\n";
            cout << "Number of colors used in the best solution: " << uniqueColors.size() << endl;
            break; // Zakończ algorytm, gdy liczba kolorów jest mniejsza o 2 niż w rozwiązaniu zachłannym
        }

        if (noImprovementCount >= restartThreshold) {
            if (!eliteSolutions.empty()) {
                const vector<int>& eliteSolution = eliteSolutions[rand() % eliteSolutions.size()];
                pathRelinking(color, eliteSolution, bestColor, bestConflicts, graph);
                color = bestColor;
                currentConflicts = bestConflicts;
            } else {
                // Restart z nowym losowym kolorowaniem
                generate(color.begin(), color.end(), [&] { return rand() % maxColors; });
                currentConflicts = evaluate(graph, color);
            }
            noImprovementCount = 0;
            tabuTenure = initialTabuTenure;
            // cout << "Restart in iteration " << iteration << endl;
        }

        // if (bestConflicts == 0 && elapsedTime >= maxTimeMs) {
        if (bestConflicts == 0) {
            break; // Stop if an optimal solution is found
        }
    }

    set<int> uniqueColors(bestColor.begin(), bestColor.end());
    if (uniqueColors.size() > maxColors) {
        cout << "WARNING: Number of used colors exceeds maxColors: " << uniqueColors.size() << " > " << maxColors << endl;
    }
    cout << "\nNumber of used colors: " << uniqueColors.size() << endl;
}

int main() {
    ifstream inputFile("./testCases/queen6.txt");
    if (!inputFile) {
        cerr << "Error opening file" << endl;
        return 1;
    }

    int vertices;
    inputFile >> vertices;
    Graph graph(vertices);

    int u, v;
    while (inputFile >> u >> v) {
        graph.addEdge(u - 1, v - 1); // Adjust for 0-based index
    }

    inputFile.close();

    vector<int> initialColor = graph.greedyColoring();
    int initialColorsUsed = *max_element(initialColor.begin(), initialColor.end()) + 1;
    cout << "Initial coloring (greedy algorithm): " << initialColorsUsed << " colors\n";

    // Parameters
    int maxColors = 100;
    int maxIterations = 3000;
    int initialTabuTenure = 20;

    // Tabu search
    tabuSearch(graph, maxColors, maxIterations, initialTabuTenure, initialColor);

    return 0;
}
