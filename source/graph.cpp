#include "graph.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <numeric>
#include <assert.h>

using namespace std;

constexpr const long long BAD_MASKS[] = {
    0,
    (1LL << 0),
    (1LL << 0) | (1LL << 2),
    (1LL << 0) | (1LL << 3) | (1LL << 6),
    (1LL << 0) | (1LL << 4) | (1LL << 8) | (1LL << 12),
    (1LL << 0) | (1LL << 5) | (1LL << 10) | (1LL << 15) | (1LL << 20),
    (1LL << 0) | (1LL << 6) | (1LL << 12) | (1LL << 18) | (1LL << 24) | (1LL << 30),
    (1LL << 0) | (1LL << 7) | (1LL << 14) | (1LL << 21) | (1LL << 28) | (1LL << 35) | (1LL << 42)};

Graph::Graph(vector<vector<int>> adjacency_list) : g(adjacency_list)
{
    V = g.size();
    is_terminal.resize(V);
    for (int v = 0; v < V; ++v)
    {
        if (g[v].empty())
        {
            is_terminal[v] = 1;
            terminals.push_back(v);
        }
        else
        {
            non_terminals.push_back(v);
        }
    }
}

bool Game::SatisfiesC22() const
{
    int count = 0;
    for (int i = 0; i < player_count; ++i)
    {
        int k = 0;
        for (const auto &[o, pref] : preferences[i])
        {
            if (o == -1)
            {
                continue;
            }
            if (graph.is_terminal[o] && pref < preferences[i].at(-1))
            {
                k++;
            }
        }
        if (k >= 2)
        {
            count++;
        }
    }
    return count < 2;
}

bool Game::IsNE(const map<int, int> &strategy) const
{
    for (int player = 0; player < player_count; ++player)
    {
        int current_outcome = Simulate(strategy);
        auto changed_strategies = GetAllStrategiesForPlayer(strategy, player);
        for (const auto new_strategy : changed_strategies)
        {
            int new_outcome = Simulate(new_strategy);
            if (preferences[player].at(new_outcome) > preferences[player].at(current_outcome))
            {
                return false;
            }
        }
    }
    return true;
}

bool Game::HasNE() const
{
    bool has_NE = false;
    for (const auto &strategy : GetAllStrategies())
    {
        if (IsNE(strategy))
        {
            has_NE = true;
            break;
        }
    }
    return has_NE;
}

vector<map<int, int>> Game::GetAllStrategiesForPlayer(const map<int, int> &strategy, int player) const
{
    const auto &controlled_vertices = control[player];
    vector<map<int, int>> strategies = {strategy};
    set<map<int, int>> strategies_set = {strategy};
    for (auto v : controlled_vertices)
    {
        int cur_size = strategies.size();
        for (int i = 0; i < cur_size; ++i)
        {
            const map<int, int> s = strategies[i];
            for (auto u : graph.g[v])
            {
                auto new_strategy = s;
                new_strategy[v] = u;
                if (!strategies_set.contains(new_strategy))
                {
                    strategies.push_back(new_strategy);
                    strategies_set.insert(new_strategy);
                }
            }
        }
    }
    return strategies;
}

vector<map<int, int>> Game::GetAllStrategies() const
{
    const auto &controlled_vertices = graph.non_terminals;
    map<int, int> strategy;
    for (auto v : controlled_vertices)
    {
        strategy[v] = graph.g[v][0];
    }
    vector<map<int, int>> strategies = {strategy};
    set<map<int, int>> strategies_set = {strategy};
    for (auto v : controlled_vertices)
    {
        int cur_size = strategies.size();
        for (int i = 0; i < cur_size; ++i)
        {
            const map<int, int> s = strategies[i];
            for (auto u : graph.g[v])
            {
                map<int, int> new_strategy = s;
                new_strategy[v] = u;
                if (!strategies_set.contains(new_strategy))
                {
                    strategies.push_back(new_strategy);
                    strategies_set.insert(new_strategy);
                }
            }
        }
    }
    return strategies;
}

int Game::Simulate(const map<int, int> &strategy) const
{
    vector<int> used(graph.V);
    int v = 0;
    while (true)
    {
        if (used[v])
        {
            return -1;
        }
        used[v] = 1;
        if (graph.is_terminal[v])
        {
            return v;
        }
        v = strategy.at(v);
    }
}

void Game::Print() const
{
    const string filename = "result_" + to_string(rand()) + ".txt";
    ofstream out(filename);
    cout << "Graph:\n";
    out << "Graph:\n";
    for (int i = 0; i < graph.g.size(); ++i)
    {
        cout << i << " : ";
        out << i << " : ";
        for (auto u : graph.g[i])
        {
            cout << u << ' ';
            out << u << ' ';
        }
        cout << '\n';
        out << '\n';
    }
    cout << "Player count: " << player_count << "\nPreferences:\n";
    out << "Player count: " << player_count << "\nPreferences:\n";
    for (int i = 0; i < preferences.size(); ++i)
    {
        cout << i << " : ";
        out << i << " : ";
        for (auto [x, y] : preferences[i])
        {
            cout << "{" << x << ' ' << y << "} ";
            out << "{" << x << ' ' << y << "} ";
        }
        cout << "\n";
        out << "\n";
    }
    cout << "Control\n";
    out << "Control\n";
    for (int i = 0; i < control.size(); ++i)
    {
        cout << i << " : ";
        out << i << " : ";
        for (auto v : control[i])
        {
            cout << v << ' ';
            out << v << ' ';
        }
        cout << '\n';
        out << '\n';
    }
}

namespace graph
{

    bool checkGraphContainsCycleDFS(int v, const vector<vector<int>> &g, vector<int> &used, vector<int> &recursion_stack)
    {
        used[v] = 1;
        recursion_stack[v] = 1;
        for (auto u : g[v])
        {
            if (!used[u] && checkGraphContainsCycleDFS(u, g, used, recursion_stack))
            {
                return true;
            }
            else if (recursion_stack[u])
            {
                return true;
            }
        }
        recursion_stack[v] = 0;
        return false;
    }

    void checkGraphAllVerticesReachable(int v, const vector<vector<int>> &g, vector<int> &used)
    {
        used[v] = 1;
        for (auto u : g[v])
        {
            if (!used[u])
            {
                checkGraphAllVerticesReachable(u, g, used);
            }
        }
    }

    size_t getGraphHash(const vector<vector<int>> &g)
    {
        int n = g.size();
        vector<int> s(n);
        vector<int> t(n);
        for (int i = 0; i < n; ++i)
        {
            s[i] = g[i].size();
            for (int j = 0; j < g[i].size(); ++j)
            {
                ++t[g[i][j]];
            }
        }
        sort(s.begin(), s.end());
        sort(t.begin(), t.end());
        for (auto el : t)
        {
            s.push_back(el);
        }
        size_t seed = s.size();
        for (const auto &i : s)
        {
            seed ^= std::hash<int>{}(i) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }

    vector<vector<int>> maskToGraph(int n, long long mask)
    {
        vector<vector<int>> g(n);
        int edge_index = 0;
        for (int u = 0; u < n; ++u)
        {
            for (int v = 0; v < n; ++v)
            {
                if (mask & (1LL << edge_index))
                {
                    g[u].push_back(v);
                }
                edge_index++;
            }
        }
        return g;
    }

    long long graphToMask(const vector<vector<int>> &g)
    {
        long long mask = 0;
        int id = 0;
        for (int v = 0; v < g.size(); ++v)
        {
            for (int u = 0; u < g.size(); ++u)
            {
                if (ranges::any_of(g[v], [u](int el)
                                   { return el == u; }))
                {
                    mask |= (1LL << id);
                }
                id++;
            }
        }
        return mask;
    }

    bool checkGraphContainsTwoTerminalVertices(const vector<vector<int>> &g)
    {
        int cnt = 0;
        for (int v = 0; v < g.size() && cnt < 2; ++v)
        {
            if (g[v].size() == 0)
            {
                ++cnt;
            }
        }
        return cnt >= 2;
    }

    bool checkAllVerticesReachableAndContainsCycle(const vector<vector<int>> &g)
    {
        vector<int> used(g.size());
        vector<int> recursion_stack(g.size());
        bool result = true;
        result &= graph::checkGraphContainsCycleDFS(0, g, used, recursion_stack);
        if (!result)
        {
            return result;
        }
        used.resize(g.size(), 0);
        graph::checkGraphAllVerticesReachable(0, g, used);
        result &= ranges::all_of(used, [](const auto &elem)
                                 { return elem == 1; });
        return result;
    }

    void generateGraphs(int n, bool allowed_ties)
    {
        const int thread_cnt = thread::hardware_concurrency();
        vector<thread> threads;
        mutex cerr_mutex;
        for (int num_thread = 0; num_thread < thread_cnt; ++num_thread)
        {
            threads.emplace_back([&, num_thread]()
                                 {
            
            const string filename = "graphs_" + to_string(n) + "_" + to_string(num_thread) + ".txt";
            ofstream out(filename);
            for (long long mask = num_thread; mask < (1LL << (n * n)); mask += thread_cnt) {
                if (!allowed_ties && (BAD_MASKS[n] & mask)) {
                    continue;
                }
                vector<vector<int>> g = graph::maskToGraph(n, mask);
                if (graph::checkGraphContainsTwoTerminalVertices(g) && 
                    graph::checkAllVerticesReachableAndContainsCycle(g)) {
                    lock_guard<mutex> lock(cerr_mutex);
                    out << mask << "\n";
                }
            } });
        }

        for (auto &thread : threads)
        {
            thread.join();
        }

        lock_guard<mutex> lock(cerr_mutex);
        ofstream out("graphs_" + to_string(n) + ".txt");
        for (int num_thread = 0; num_thread < thread_cnt; ++num_thread)
        {
            const string name = "graphs_" + to_string(n) + "_" + to_string(num_thread) + ".txt";
            fstream file(name);
            file.is_open();
            if (file.peek() == EOF)
            {
                file.close();
                remove(name.c_str());
                continue;
            }
            file.close();
            ifstream in(name);
            if (in)
            {
                out << in.rdbuf();
                in.close();
                remove(name.c_str());
            }
        }
    }

    void removeDuplicatesFromGraphs(int n)
    {
        map<size_t, vector<pair<long long, int>>> graphs_masks;
        string name = "graphs_" + to_string(n) + ".txt";
        ifstream in(name);
        ofstream out("optimized_" + name);
        long long mask;
        while (in >> mask)
        {
            vector<vector<int>> g = graph::maskToGraph(n, mask);
            auto cur_hash = graph::getGraphHash(g);
            atomic<bool> ok = true;
            const int thread_cnt = thread::hardware_concurrency();
            vector<thread> threads;
            atomic<int> bad_id = -1;
            for (int num_thread = 0; num_thread < thread_cnt; ++num_thread)
            {
                threads.emplace_back([&, num_thread]()
                                     {
                    for (int i = num_thread; i < graphs_masks[cur_hash].size() && ok; i += thread_cnt)
                    {
                        if (checkIsomorphic(g, graph::maskToGraph(n, graphs_masks[cur_hash][i].first)))
                        {
                            ok = false;
                            bad_id = i;
                            break;
                        }
                    } });
            }
            for (auto &thread : threads)
            {
                thread.join();
            }
            if (ok)
            {
                graphs_masks[cur_hash].emplace_back(mask, 0);
                out << mask << "\n";
            }
            else
            {
                if (bad_id >= 0)
                {
                    graphs_masks[cur_hash][bad_id].second -= 1;
                }
                while (bad_id > 0 && graphs_masks[cur_hash][bad_id].second < graphs_masks[cur_hash][bad_id - 1].second)
                {
                    swap(graphs_masks[cur_hash][bad_id - 1], graphs_masks[cur_hash][bad_id]);
                    --bad_id;
                }
            }
        }
    }

    bool checkIsomorphic(const vector<vector<int>> &g1, const vector<vector<int>> &g2)
    {
        if (g1.size() != g2.size())
        {
            return false;
        }
        int N = g1.size();
        vector<int> s1, s2;
        for (int i = 0; i < N; ++i)
        {
            s1.push_back(g1[i].size());
            s2.push_back(g2[i].size());
        }
        sort(s1.begin(), s1.end());
        sort(s2.begin(), s2.end());
        for (int i = 0; i < N; ++i)
        {
            if (s1[i] != s2[i])
            {
                return false;
            }
        }
        if (g1[0].size() != g2[0].size())
        {
            return false;
        }
        vector<int> p(N);
        iota(p.begin(), p.end(), 0);
        do
        {
            if (p[0] != 0)
            {
                break;
            }
            bool ok = true;
            for (int i = 0; i < N; ++i)
            {
                if (g1[i].size() != g2[p[i]].size())
                {
                    ok = false;
                    break;
                }
                vector<int> v1, v2;
                for (int j = 0; j < g1[i].size(); ++j)
                {
                    v1.push_back(g1[i][j]);
                    v2.push_back(p[g2[p[i]][j]]);
                }
                sort(v1.begin(), v1.end());
                sort(v2.begin(), v2.end());
                for (int j = 0; j < v1.size(); ++j)
                {
                    if (v1[j] != v2[j])
                    {
                        ok = false;
                        break;
                    }
                }
                if (!ok)
                {
                    break;
                }
            }
            if (ok)
            {
                return true;
            }
        } while (next_permutation(p.begin(), p.end()));
        return false;
    }

} // namespace graph

namespace game
{

    void generateGamesByGraphWithPreferences(const Graph &graph, int player_count, vector<map<int, int>> &preferences, vector<set<int>> &control, bool check_C22, int index)
    {
        if (index == graph.non_terminals.size())
        {
            if (ranges::all_of(control, [](const auto &elem)
                               { return !elem.empty(); }))
            {
                vector<vector<int>> control_(control.size());
                for (int i = 0; i < control.size(); ++i)
                {
                    for (const auto &el : control[i])
                    {
                        control_[i].push_back(el);
                    }
                }
                Game game(player_count, graph, preferences, control_);
                if (check_C22 && !game.SatisfiesC22())
                {
                    return;
                }
                if (!game.HasNE())
                {
                    game.Print();
                    exit(0);
                }
            }
            return;
        }
        for (int i = 0; i < player_count; ++i)
        {
            control[i].insert(graph.non_terminals[index]);
            generateGamesByGraphWithPreferences(graph, player_count, preferences, control, check_C22, index + 1);
            control[i].erase(graph.non_terminals[index]);
        }
    }

    void generateGamesByGraph(const Graph &graph, int player_count, vector<map<int, int>> &preferences, bool check_C22, int player)
    {
        if (player_count > graph.non_terminals.size())
        {
            return;
        }
        if (player == -1)
        {
            preferences.resize(player_count);
            generateGamesByGraph(graph, player_count, preferences, check_C22, player + 1);
            return;
        }
        vector<int> outcomes = graph.terminals;
        outcomes.push_back(-1);
        sort(outcomes.begin(), outcomes.end());
        do
        {
            for (int i = 0; i < outcomes.size(); ++i)
            {
                preferences[player][outcomes[i]] = i;
            }
            if (player + 1 == player_count)
            {
                vector<set<int>> control(player_count);
                game::generateGamesByGraphWithPreferences(graph, player_count, preferences, control, check_C22);
            }
            else
            {
                generateGamesByGraph(graph, player_count, preferences, check_C22, player + 1);
            }
        } while (next_permutation(outcomes.begin(), outcomes.end()));
    }

    void searchForCounterexample(int player_count, int n, bool check_C22)
    {
        const string filename = "graphs_" + to_string(n) + ".txt";
        ifstream in(filename);
        vector<long long> masks;
        long long mask;
        while (in >> mask)
        {
            masks.push_back(mask);
        }
        in.close();
        vector<thread> threads;
        const int thread_cnt = thread::hardware_concurrency();
        atomic<int> cnt = 0;
        mutex m;
        for (int num_thread = 0; num_thread < thread_cnt; ++num_thread)
        {
            threads.emplace_back([&, num_thread]()
                                 {
                for (int i = num_thread; i < masks.size(); i += thread_cnt) {
                    Graph gr(graph::maskToGraph(n, masks[i]));
                    vector<map<int, int>> prefs;
                    game::generateGamesByGraph(gr, player_count, prefs, check_C22);
                    ++cnt;
                    if (cnt % 10000 == 0) {
                        lock_guard<mutex> lock(m);
                        cout << cnt / 10000 << "e4 / 6325e4\n";
                    }
                } });
        }
        for (auto &thread : threads)
        {
            thread.join();
        }
    }

} // namespace game
