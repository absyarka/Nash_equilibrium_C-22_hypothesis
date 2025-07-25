#include "graph.h"

#include <iostream>
#include <assert.h>

using namespace std;

void testGurvichExampleDoesNotHaveNE()
{
    int player_count = 3;
    vector<vector<int>> g = {
        {1, 2},
        {3, 4},
        {3, 6},
        {2, 5},
        {},
        {},
        {}};
    vector<map<int, int>> preferences = {
        {{-1, 3}, {5, 2}, {4, 1}, {6, 0}},
        {{6, 3}, {4, 2}, {5, 1}, {-1, 0}},
        {{-1, 3}, {4, 2}, {6, 1}, {5, 0}}};
    vector<vector<int>> control = {
        {0},
        {1, 3},
        {2}
    };
    Game game(player_count, Graph(g), preferences, control);
    assert(!game.HasNE());
}

int main()
{
    testGurvichExampleDoesNotHaveNE();
}