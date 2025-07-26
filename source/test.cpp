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
    {}};

void test4PersonGame()
{
    int player_count = 4;
    const vector<vector<int>> g = {
        {1, 7},
        {2, 3},
        {4, 5, 6},
        {4, 9},
        {5, 10},
        {6, 11},
        {7, 12},
        {3, 8},
        {},
        {},
        {},
        {},
        {}};
    vector<map<int, int>> preferences = {
        {{9, 5}, {11, 4}, {10, 3}, {8, 2}, {12, 1}, {-1, 0}},
        {{-1, 5}, {8, 4}, {10, 3}, {11, 2}, {12, 1}, {9, 0}},
        {{-1, 5}, {12, 4}, {8, 3}, {9, 2}, {10, 1}, {11, 0}},
        {{8, 5}, {9, 4}, {10, 3}, {12, 2}, {11, 1}, {-1, 0}}};
    vector<vector<int>> control = {
        {0, 2},
        {1, 4, 6},
        {3, 7},
        {5}};
    Graph gr(g);
    Game game(player_count, Graph(gr), preferences, control);
    assert(!game.HasNE());
    assert(!game.SatisfiesC22());
}

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

void testGurvichGraphGamesDoNotSuit()
{
    const string filename = "graphs_7.txt";
    ofstream out(filename);
    out << graph::graphToMask(GurvichGraph) << '\n';
    out.close();
    game::searchForCounterexample(3, 7); // doesn't find a game
    cout << "check that noting is printed yet\n";
    game::searchForCounterexample(3, 7, false); // finds a game
}

int main()
{
    test4PersonGame();
    testGurvichExampleDoesNotHaveNE();
    testGurvichGraphGamesDoNotSuit();
}
