#pragma once

#include <vector>
#include <map>
#include <set>

using namespace std;

struct Graph
{
    Graph(vector<vector<int>> adjacency_list);

    int V;
    vector<vector<int>> g;
    vector<int> is_terminal;
    vector<int> non_terminals;
    vector<int> terminals;
};

struct Game
{
    Game(int player_count, const Graph &graph, const vector<map<int, int>> &preferences, const vector<vector<int>> &control)
        : player_count(player_count),
          graph(graph),
          preferences(preferences),
          control(control)
    {
    }

    int player_count;
    Graph graph;
    vector<map<int, int>> preferences;
    vector<vector<int>> control;

    bool SatisfiesC22() const;
    bool IsNE(const map<int, int> &strategy) const;
    bool HasNE() const;
    int Simulate(const map<int, int> &strategy) const;
    vector<map<int, int>> GetAllStrategiesForPlayer(const map<int, int> &strategy, int player) const;
    vector<map<int, int>> GetAllStrategies() const;
    void Print() const;
};

namespace graph
{

    bool checkGraphContainsCycleDFS(int v, const vector<vector<int>> &g, vector<int> &used, vector<int> &recursion_stack);

    void checkGraphAllVerticesReachable(int v, const vector<vector<int>> &g, vector<int> &used);

    size_t getGraphHash(const vector<vector<int>> &g);

    vector<vector<int>> maskToGraph(int n, long long mask);

    long long graphToMask(const vector<vector<int>>& g);

    bool checkGraphContainsTwoTerminalVertices(const vector<vector<int>> &g);

    bool checkAllVerticesReachableAndContainsCycle(const vector<vector<int>> &g);

    void generateGraphs(int n, bool allowed_ties = false);

    void removeDuplicatesFromGraphs(int n);

    bool checkIsomorphic(const vector<vector<int>> &g1, const vector<vector<int>> &g2);

} // namespace graph

namespace game
{

    void generateGamesByGraphWithPreferences(const Graph &graph, int player_count, vector<map<int, int>> &preferences, vector<set<int>> &control, int index=0);

    void generateGamesByGraph(const Graph &graph, int player_count, vector<map<int, int>> &preferences, int player = -1);

    void searchForCounterexample(int player_count, int n);

} // namespace game
