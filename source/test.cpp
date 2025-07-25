#include "graph.h"

#include <fstream>
#include <iostream>
#include <assert.h>

using namespace std;

const vector<vector<int>> GurvichGraph = {
    {1, 2},
    {3, 4},
    {3, 6},
    {2, 5},
    {},
    {},
    {}
};

void testGurvichExampleDoesNotHaveNE()
{
    int player_count = 3;
    vector<vector<int>> g = GurvichGraph;
    vector<map<int, int>> preferences = {
        {{-1, 3}, {5, 2}, {4, 1}, {6, 0}},
        {{6, 3}, {4, 2}, {5, 1}, {-1, 0}},
        {{-1, 3}, {4, 2}, {6, 1}, {5, 0}}};
    vector<vector<int>> control = {
        {0},
        {1, 3},
        {2}};
    Game game(player_count, Graph(GurvichGraph), preferences, control);
    assert(!game.HasNE());
    assert(!game.SatisfiesC22());
}

void testGurvichGraphGamesDoNotSuit() {
    const string filename = "graphs_7.txt";
    ofstream out(filename);
    out << graph::graphToMask(GurvichGraph);
    out.close();
    game::searchForCounterexample(3, 7);
    remove(filename.c_str());
}

int main()
{
    testGurvichExampleDoesNotHaveNE();
    testGurvichGraphGamesDoNotSuit();
}