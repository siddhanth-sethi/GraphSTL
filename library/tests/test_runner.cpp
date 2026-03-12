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

// A* Search Tests

void test_a_star_search() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addEdge('A', 'B', 10);

    auto search = [](char, char) { 
        return 0.0; 
    };

    auto result = a_star(g, 'A', 'B', search);

    assert(result.first == 10.0);
    assert(result.second.size() == 2);
}

// Floyd-Warshall Tests

void test_floyd_warshall() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 3);
    g.addEdge('B', 'C', 4);
    g.addEdge('A', 'C', 10);

    auto fw = floyd_warshall(g);

    assert(fw.dist['A']['B'] == 3);
    assert(fw.dist['A']['C'] == 7);
    assert(fw.dist['B']['C'] == 4);
    assert(fw.dist['A']['A'] == 0);
    assert(fw.dist['B']['B'] == 0);
    assert(fw.dist['C']['C'] == 0);
    assert(!fw.hasNegativeCycle);
}

void test_2_floyd_warshall() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 2);
    g.addEdge('A', 'C', 100);

    auto fw = floyd_warshall(g);
    auto path = fw.path('A', 'C');

    assert(path.size() == 3);
    assert(path[0] == 'A');
    assert(path[1] == 'B');
    assert(path[2] == 'C');
}

void test_floyd_warshall_unreachable() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    // No edge

    auto fw = floyd_warshall(g);
    assert(fw.dist['A']['B'] == numeric_limits<int>::max());

    auto path = fw.path('A', 'B');
    assert(path.empty());
}

void test_floyd_warshall_negative_cycle() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addEdge('A', 'B', -1);
    g.addEdge('B', 'A', -1);

    auto fw = floyd_warshall(g);

    assert(fw.hasNegativeCycle);
}

void test_floyd_warshall_undirected() {
    graph<char, int, false> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addEdge('A', 'B', 2);
    g.addEdge('B', 'C', 3);

    auto fw = floyd_warshall(g);

    assert(fw.dist['A']['B'] == 2);
    assert(fw.dist['B']['A'] == 2);
    assert(fw.dist['A']['C'] == 5);
    assert(fw.dist['C']['A'] == 5);
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

// Kosaraju's SCC Tests

void test_kosaraju_cycle() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);
    // A->B->C->A forms one SCC and D is isolated
    
    auto sccs = kosaraju_scc(g);
    assert(sccs.size() == 2);

    bool foundCycleGroup = false;
    bool found = false;

    for(const auto& comp: sccs) {
        if(comp.size() == 3) {
            foundCycleGroup = true;
        }

        if(comp.size() == 1) {
            found = true;
        }
    }
    assert(foundCycleGroup);
    assert(found);
}

void test_kosaraju_all_separate() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    // No edges

    auto sccs = kosaraju_scc(g);
    assert(sccs.size() == 3);

    for(const auto& comp: sccs){
        assert(comp.size() == 1);
    }
}

void test_kosaraju_two_sccs() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'A', 1);
    g.addEdge('C', 'D', 1);
    g.addEdge('D', 'C', 1);
    g.addEdge('B', 'C', 1);  // one-way bridge
    // SCC1: A<->B,  SCC2: C<->D,  bridge: B->C 
    
    auto sccs = kosaraju_scc(g);
    assert(sccs.size() == 2);

    int sizeTwo = 0;

    for(const auto& comp: sccs){
        if (comp.size() == 2) {
            sizeTwo++;
        }
    }
    assert(sizeTwo == 2);
}

void test_kosaraju_single_node() {
    graph<char, int, true> g;
    g.addNode('X');

    auto sccs =kosaraju_scc(g);

    assert(sccs.size() == 1);
    assert(sccs[0].size() == 1);
    assert(sccs[0][0] == 'X');
}


// Tarjan's SCC Tests

void test_tarjan_scc_cycles() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);
    
    auto components = tarjan_scc(g);
    assert(components.size() == 2);

    bool foundCycleGroup = false;
    
    for(const auto& comp: components){
        if(comp.size() == 3) {
            foundCycleGroup = true;
        }
    }
    
    assert(foundCycleGroup);
}

// Testing for Kosaraju and Tarjan gives the same output

void test_kosaraju_and_tarjan_gives_same_output() {
    graph<char, int, true> g;
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addNode('E');
    g.addEdge('A', 'B', 1);
    g.addEdge('B', 'C', 1);
    g.addEdge('C', 'A', 1);
    g.addEdge('D', 'E', 1);
    g.addEdge('E', 'D', 1);
    
    auto tarjan = tarjan_scc(g);
    auto kosaraju = kosaraju_scc(g);
    
    assert(tarjan.size() == kosaraju.size());
    
    vector<size_t> tsizes;
    vector<size_t> ksizes;
    
    for(auto& c: tarjan) {
        tsizes.push_back(c.size());
    }
    
    for(auto& c: kosaraju) {
        ksizes.push_back(c.size());
    }
    
    sort(tsizes.begin(), tsizes.end());        
    sort(ksizes.begin(), ksizes.end());
    
    assert(tsizes == ksizes);
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
    run_test("Bellman-Ford",test_bellman_ford);
    run_test("Bellman-Ford (Negative Weight)",test_bellman_ford_negative_weight);
    run_test("Bellman-Ford (Negative Cycle)",test_bellman_ford_negative_cycle);
    run_test("Bellman-Ford (Unreachable)",test_bellman_ford_unreachable);
    run_test("Bellman-Ford (Single Node)",test_bellman_ford_single_node);
    cout<<endl;

    // A* Search 
    run_test("A* Search", test_a_star_search);
    cout<<endl;

    // Floyd-Warshall
    run_test("Floyd-Warshall",test_floyd_warshall);
    run_test("Floyd-Warshall 2",test_2_floyd_warshall);
    run_test("Floyd-Warshall (Unreachable)",test_floyd_warshall_unreachable);
    run_test("Floyd-Warshall (Negative Cycle)",test_floyd_warshall_negative_cycle);
    run_test("Floyd-Warshall (Undirected)",test_floyd_warshall_undirected);
    cout<<endl;

    // Kruskal's Mst Test
    run_test("Kruskal's MST",test_kruskal_mst);
    cout<<endl;

    // Kosaraju's SCC
    run_test("Kosaraju (Basic Cycle)",test_kosaraju_cycle);
    run_test("Kosaraju (All Separate)",test_kosaraju_all_separate);
    run_test("Kosaraju (Two SCCs)",test_kosaraju_two_sccs);
    run_test("Kosaraju (Single Node)",test_kosaraju_single_node);
    cout<<endl;
    
    // Tarjan's SCC
    run_test("Tarjan's SCC (Cycles)",test_tarjan_scc_cycles);
    cout<<endl;

    // Kosaraju and Tarjan gives the same output
    run_test("Kosaraju and Tarjan gives the same Output",test_kosaraju_and_tarjan_gives_same_output);
    cout<<endl;
    
    cout << "__________ALL TESTS PASSED SUCCESSFULLY__________ " << endl;

    return 0;
}