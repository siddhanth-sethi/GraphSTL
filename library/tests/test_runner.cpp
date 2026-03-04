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

// UNIT TESTS

void test_add_node() {
    graph<int, int> g;
    g.addNode(10);
    g.addNode(20);

    assert(g.getData().size() == 2);
    assert(g.getData().count(0) == 1);
    assert(g.getData().count(1) == 1);
}

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



int  main() {
    cout << "==================================" << endl;
    cout << "    Running GraphSTL Test Suite   " << endl;
    cout << "==================================" << endl;

    // Iterator Tests
    run_test("Iterator + std::find_if", test_iterator_with_std_find_if);
    run_test("Iterator + std::count_if", test_iterator_with_std_count_if );
    run_test("Iterator Default Constructible",test_iterator_default_constructible);
    run_test("Const Iterator", test_const_iterator);

    // Node Insertion / Removal Tests
    run_test("Node Insertion",test_add_node);

    run_test("Remove Node (Full Cleanup)",test_remove_node);
    run_test("Remove Node (Incoming Cleanup)", test_remove_node_cleans_incoming);

    // Edge Insertion / Removal Tests
    run_test("Edge Insertion",test_add_edge);

    run_test("Remove Edge (Directed)",test_remove_edge_directed);
    run_test("Remove Edge (Undirected)", test_remove_edge_undirected);
    
    cout << "==================================" << endl;
    cout << "    ALL TESTS PASSED SUCCESSFULLY " << endl;
    cout << "==================================" << endl;

    return 0;
}