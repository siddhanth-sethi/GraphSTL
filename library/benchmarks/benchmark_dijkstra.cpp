#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <string>

#include "../include/GraphSTL/graph.h"
#include "../include/GraphSTL/graph_algorithms.h"

using namespace std;
using namespace custom_stl;


// Compares Dijkstra (Priority queue based) - O((V+E)log V) vs Dijkstra_Naive(Linear scan based) - O(V^2)

graph<int, int, true> random_graph_builder(int n, double density, int max_w, unsigned seed_val) {
    graph<int, int, true> g;
    for(int i = 0; i < n; ++i) {
        g.addNode(i);
    }

    mt19937 rng(seed_val);

    uniform_real_distribution<double> coin(0.0, 1.0);
    uniform_int_distribution<int> random_weight(1, max_w);

    for(int i = 0; i < n; ++i){
        for(int j = 0; j < n; ++j){
            if(  i != j && coin(rng) < density ){
                g.addEdge(i, j, random_weight(rng));
            }
        }
    }
    return g;
}

template <typename Func>
double get_exec_time(Func&& func){
    auto start = chrono::high_resolution_clock::now();
    func();
    auto end = chrono::high_resolution_clock::now();
    return chrono::duration<double, milli>(end - start).count();
}

void test_performance(int n, double density) {
    const unsigned test_seed = 42;
    const int max_weight = 100;

    cout<<"Testing n = "<<n<<" | density = "<< density<<" | approx edges = "<< static_cast<int>(n * n * density)<<endl;

    auto g = random_graph_builder(n, density, max_weight, test_seed);

    int src = 0;
    int dest = n - 1;

    pair<int, vector<int>> result_dijkstra;
    double time_dijkstra = get_exec_time([&]() {
        result_dijkstra = dijkstra(g, src, dest);
    });

    pair<int, vector<int>> result_dijkstra_naive;
    double time_dijkstra_naive = get_exec_time([&]() {
        result_dijkstra_naive = dijkstra_naive(g, src, dest);
    });

    bool is_same = (result_dijkstra.first == result_dijkstra_naive.first);

    cout<<"Dijkstra : "<< time_dijkstra<<" ms (cost = "<<result_dijkstra.first<<")"<<endl;
    cout<<"Naive Dijkstra : "<<time_dijkstra_naive<<" ms  (cost = "<<result_dijkstra_naive.first<<")"<<endl;
    cout<<"Results match : "<<(is_same ? "YES" : "NO *** MISMATCH ***")<<endl;
    cout<<"Speedup : "<<(time_dijkstra_naive / (time_dijkstra > 0 ? time_dijkstra : 0.001))<<"x"<<endl;
    cout << endl;
}

int main() {

    cout<<"GraphSTL Dijkstra Benchmark"<<endl;

    // Small graph
    test_performance(100,  0.3);

    // Medium graph
    test_performance(500,  0.2);

    // Larger graph
    test_performance(1000, 0.1);

    // Dense graph
    test_performance(500,  0.5);

    cout<<"Benchmark complete."<<endl;
    return 0;
}