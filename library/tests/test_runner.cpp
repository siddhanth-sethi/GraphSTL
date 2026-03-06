#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include "../include/GraphSTL/graph.h"
#include "../include/GraphSTL/graph_algorithms.h"

using namespace std;
using namespace custom_stl;

// Simple Test Runner Helper
void run_test(string name, void (*test_func)()) {
    cout<<"[RUNNING] "<<name<< "... ";
    try {
        test_func();
        cout << "PASSED" << endl;
    } 
    catch (const exception& e) {
        cout<<"FAILED ("<<e.what()<< ")" << endl;
        exit(1);
    }
}

// ITERATOR TESTS
void test_iterator_with_std_find_if() {
    graph<int, int, true> g;
    g.addNode(10);
    g.addNode(20);
    g.addNode(30);

    auto it = find_if(g.begin(), g.end(),[](const pair<const size_t, int>& p) {
            return p.second == 20;
        }
    );
    assert(it != g.end());
    assert(it->second == 20);
}

void test_iterator_with_std_count_if() {
    graph<int, int, true> g;
    g.addNode(10);
    g.addNode(20);
    g.addNode(30);

    auto cnt = count_if(g.begin(), g.end(),[](const pair<const size_t, int>& p) {
            return p.second > 15;
        }
    );
    assert(cnt == 2);
}

void test_iterator_default_constructible() {
    graph<int, int>::NodeIterator it1;
    graph<int, int>::NodeIterator it2;
    assert(it1 == it2);
}

void test_const_iterator() {
    graph<int, int, true> g;
    g.addNode(1);
    g.addNode(2);

    const auto& cg = g;
    size_t count = 0;
    for (auto it = cg.begin(); it !=cg.end(); ++it) {
        ++count;
    }
    assert(count == 2);
}

// Node Insertion / Removal Tests

void test_add_node() {
    graph<int, int> g;
    g.addNode(10);
    g.addNode(20);

    assert(g.getData().size() == 2);
    assert(g.getData().count(0) == 1);
    assert(g.getData().count(1) == 1);
}

void test_remove_node() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);

    g.removeNode('B');

    assert(g.nodeCount() == 2);
    assert(g.hasNode('A'));
    assert(!g.hasNode('B'));
    assert(g.hasNode('C'));

    auto path = bfs(g, 'A');
    assert(path.size() == 1);
    assert(path[0] == 'A');

    auto pathC = bfs(g, 'C');
    assert(pathC.size() == 2);
}

void test_remove_node_cleans_incoming() {
    graph<char, int, true> g;
    g.addNode('X');
    g.addNode('Y');
    g.addNode('Z');
    g.addEdge('X', 'Y', 1);
    g.addEdge('Z', 'Y', 1);

    g.removeNode('Y');

    assert(g.nodeCount() == 2);

    auto pathX = bfs(g, 'X');
    assert(pathX.size() == 1);

    auto pathZ = bfs(g, 'Z');
    assert(pathZ.size() == 1);
}

// Edge Insertion / Removal Tests

void test_add_edge() {
    graph<string, int, true> g;
    g.addNode("A");
    g.addNode("B");
    g.addEdge("A", "B", 100);
}

void test_remove_edge_directed() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 5);
    g.addEdge('A', 'C', 10);

    g.removeEdge('A', 'B');

    auto path = bfs(g, 'A');
    bool hasB = false;
    for (char c : path) {
        if (c == 'B') {
            hasB = true;
        }
    }

    assert(!hasB);

    bool hasC = false;
    for (char c : path) {
        if (c == 'C') {
            hasC = true;
        }
    }
    assert(hasC);
}

void test_remove_edge_undirected() {
    graph<char, int, false> g;
    g.addNode('A');
    g.addNode('B');
    g.addEdge('A', 'B', 5);

    g.removeEdge('A', 'B');

    auto pathA = bfs(g, 'A');
    assert(pathA.size() == 1);

    auto pathB = bfs(g, 'B');
    assert(pathB.size() == 1);
}

// Cycle Detection Tests

void test_cycle_directed_has_cycle() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);

    assert(g.cyclePresent() == true);
}

void test_cycle_directed_no_cycle() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);

    assert(g.cyclePresent() == false);
}

void test_cycle_directed_self_loop() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addEdge('A', 'A', 1);

    assert(g.cyclePresent() == true);
}

void test_cycle_undirected_has_cycle() {
    graph<char, int, false> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);

    assert(g.cyclePresent() == true);
}

void test_cycle_undirected_no_cycle() {
    graph<char, int, false> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);

    assert(g.cyclePresent() == false);
}

void test_cycle_empty_graph() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    // No edges

    assert(g.cyclePresent() == false);
}

void test_cycle_disconnected_with_cycle() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('C', 'D', 1);
    g.addEdge('D', 'C', 1);

    assert(g.cyclePresent() == true);
}


// BFS Tests

void test_bfs_traversal() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('A', 'C', 1);

    vector<char> path = bfs(g, 'A');

    assert(path.size() == 3);
    assert(path[0] == 'A');
    bool hasB = false;
    bool hasC = false;
    for (char c : path) {
        if (c == 'B') {
            hasB = true;
        }

        if (c == 'C') {
            hasC = true;
        }
    }
    assert(hasB && hasC);
}

// DFS Tests

void test_dfs_traversal() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);

    vector<char> path = dfs(g, 'A');

    assert(path.size() == 3);
    assert(path[0] == 'A');
    bool foundC = false;
    for (char c : path) {
        if (c == 'C') {
            foundC = true;
        }
    }
    assert(foundC);
}

// Dijkstra Tests

void test_dijkstra_shortest_path() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 10);
    g.addEdge('A', 'C', 50);
    g.addEdge('B', 'C', 10);

    auto result = dijkstra(g, 'A', 'C');

    assert(result.first == 20);
    assert(result.second.size() == 3);
    assert(result.second[0] == 'A');
    assert(result.second[1] == 'B');
    assert(result.second[2] == 'C');
}

void test_disconnected_graph() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('Z');

    auto result = dijkstra(g, 'A', 'Z');

    assert(result.first == numeric_limits<int>::max());
    assert(result.second.empty());
}

// Kahn's Topological Sort Tests

void test_kahn_linear() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'D', 1);
    // A->B->C->D
    
    auto order = kahn_topo_sort(g);

    assert(order.size() == 4);
    assert(order[0] == 'A');
    assert(order[1] == 'B');
    assert(order[2] == 'C');
    assert(order[3] == 'D');
}

void test_kahn_diamond() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('A', 'C', 1);
    g.addEdge('B', 'D', 1);
    g.addEdge('C', 'D', 1);

    auto order = kahn_topo_sort(g);

    assert(order.size() == 4);

    assert(order[0] == 'A');
    assert(order[3] == 'D');
    // A must be first, D must be last
    
    bool bBeforeD = false;
    bool cBeforeD = false;

    for(size_t i = 0; i < order.size(); ++i) {
        if (order[i] == 'B') {
            bBeforeD = true;
        }

        if (order[i] == 'C') {
            cBeforeD = true;
        }

        if (order[i] == 'D') {
            assert(bBeforeD && cBeforeD);
        }
    }
}

void test_kahn_has_cycle_returns_empty() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);

    auto order = kahn_topo_sort(g);

    assert(order.empty());
}

void test_kahn_single_node() {
    graph<char, int, true> g;
    g.addNode('X');

    auto order = kahn_topo_sort(g);

    assert(order.size() == 1);
    assert(order[0] == 'X');
}

void test_kahn_disconnected_dag() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('C', 'D', 1);
    // Separate chains - A->B and C->D

    auto order = kahn_topo_sort(g);

    assert(order.size() == 4);
    // A must come before B, C must come before D
    int posA = -1;
    int posB = -1;
    int posC = -1;
    int posD = -1;

    for(int i = 0; i < (int)order.size(); ++i) {
        if (order[i] == 'A') {
            posA = i;
        }

        if (order[i] == 'B') {
            posB = i;
        }

        if (order[i] == 'C') {
            posC = i;
        }

        if (order[i] == 'D') {
            posD = i;
        }
    }
    assert(posA < posB);
    assert(posC < posD);
}

void test_kahn_with_cycle_detection() {
    graph<char, int, true> dag;
    dag.addNode('A');
    dag.addNode('B');
    dag.addEdge('A', 'B', 1);

    assert(!dag.cyclePresent());
    assert(!kahn_topo_sort(dag).empty());

    graph<char, int, true> cyc;
    cyc.addNode('X');
    cyc.addNode('Y');
    cyc.addEdge('X', 'Y', 1);
    cyc.addEdge('Y', 'X', 1);

    assert(cyc.cyclePresent());
    assert(kahn_topo_sort(cyc).empty());
}

// Bellman-Ford Tests

void test_bellman_ford() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 10);
    g.addEdge('A', 'C', 50);
    g.addEdge('B', 'C', 10);

    auto [cost, path] = bellman_ford(g, 'A', 'C');

    assert(cost == 20);
    assert(path.size() == 3);
    assert(path[0] == 'A');
    assert(path[1] == 'B');
    assert(path[2] == 'C');
}

void test_bellman_ford_negative_weight() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 5);
    g.addEdge('B', 'C', -3);
    g.addEdge('A', 'C', 4);
    // A --5--> B --(-3)--> C
    // A --4--> C  (direct but costs 4, whereas B costs 5+(-3)=2)

    auto [cost, path] = bellman_ford(g, 'A', 'C');

    assert(cost == 2);
    assert(path.size() == 3);
    assert(path[0] == 'A');
    assert(path[1] == 'B');
    assert(path[2] == 'C');
}

void test_bellman_ford_negative_cycle() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', -1);
    g.addEdge('C', 'A', -1);
    // A->B(1), B->C(-1), C->A(-1) → negative cycle
    
    auto [cost, path] = bellman_ford(g, 'A', 'C');

    assert(cost == numeric_limits<int>::min());
    assert(path.empty());
}

void test_bellman_ford_unreachable() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('Z');

    auto [cost, path] = bellman_ford(g, 'A', 'Z');

    assert(cost == numeric_limits<int>::max());
    assert(path.empty());
}

void test_bellman_ford_single_node() {
    graph<char, int, true> g;
    g.addNode('A');

    auto [cost, path] = bellman_ford(g, 'A', 'A');

    assert(cost == 0);
    assert(path.size() == 1);
    assert(path[0] == 'A');
}

// Kruskal's MST Tests

void test_kruskal_mst() {
    graph<char, int, false> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 2);
    g.addEdge('A', 'C', 100);

    auto result = kruskal_mst(g);

    assert(result.first == 3);
    assert(result.second.size() == 2);
}


int  main() {

    cout << "__________Running GraphSTL Test Suite__________"<<endl<<endl;

    // Iterator Tests
    run_test("Iterator + std::find_if", test_iterator_with_std_find_if);
    run_test("Iterator + std::count_if", test_iterator_with_std_count_if );
    run_test("Iterator Default Constructible",test_iterator_default_constructible);
    run_test("Const Iterator", test_const_iterator);
    cout<<endl;

    // Node Insertion / Removal Tests
    run_test("Node Insertion",test_add_node);

    run_test("Remove Node (Full Cleanup)",test_remove_node);
    run_test("Remove Node (Incoming Cleanup)", test_remove_node_cleans_incoming);
    cout<<endl;

    // Edge Insertion / Removal Tests
    run_test("Edge Insertion",test_add_edge);

    run_test("Remove Edge (Directed)",test_remove_edge_directed);
    run_test("Remove Edge (Undirected)", test_remove_edge_undirected);
    cout<<endl;

    // Cycle Detection Tests
    run_test("Cycle Check: Directed Has Cycle",test_cycle_directed_has_cycle);
    run_test("Cycle Check: Directed No Cycle",test_cycle_directed_no_cycle);
    run_test("Cycle Check: Directed Self Loop",test_cycle_directed_self_loop);
    run_test("Cycle Check: Undirected Has Cycle",test_cycle_undirected_has_cycle);
    run_test("Cycle Check: Undirected No Cycle",test_cycle_undirected_no_cycle);
    run_test("Cycle Check: Empty Graph",test_cycle_empty_graph);
    run_test("Cycle Check: Disconnected with Cycle",test_cycle_disconnected_with_cycle);
    cout<<endl;

    // BFS Test
    run_test("BFS Traversal",test_bfs_traversal);
    cout<<endl;

    // DFS Test
    run_test("DFS Traversal",test_dfs_traversal);
    cout<<endl;

    // Dijkstra Tests
    run_test("Dijkstra (Shortest Path)",test_dijkstra_shortest_path);
    run_test("Dijkstra (Disconnected)",test_disconnected_graph);
    cout<<endl;

    // Kahn's Topological Sort
    run_test("Kahn (Linear Chain)",test_kahn_linear);
    run_test("Kahn (Diamond DAG)",test_kahn_diamond);
    run_test("Kahn (Cycle Returns Empty)",test_kahn_has_cycle_returns_empty);
    run_test("Kahn (Single Node)",test_kahn_single_node);
    run_test("Kahn (Disconnected DAG)",test_kahn_disconnected_dag);
    run_test("Kahn + Cycle Present",test_kahn_with_cycle_detection);
    cout<<endl;

    // Bellman-Ford
    run_test("Bellman-Ford ",test_bellman_ford);
    run_test("Bellman-Ford (Negative Weight)",test_bellman_ford_negative_weight);
    run_test("Bellman-Ford (Negative Cycle)",test_bellman_ford_negative_cycle);
    run_test("Bellman-Ford (Unreachable)",test_bellman_ford_unreachable);
    run_test("Bellman-Ford (Single Node)",test_bellman_ford_single_node);
    cout<<endl;

    // Kruskal's Mst Test
    run_test("Kruskal's MST",test_kruskal_mst);
    cout<<endl;
    
    cout << "__________ALL TESTS PASSED SUCCESSFULLY__________ " << endl;

    return 0;
}