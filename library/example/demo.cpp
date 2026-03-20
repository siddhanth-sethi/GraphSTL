#include <iostream>
#include "../include/GraphSTL/graph.h"
#include "../include/GraphSTL/graph_algorithms.h"

using namespace std;
using namespace custom_stl;

int main() {

    // graph < character node, integer weight, directed = true >
    graph<char, int, true> g;

    // Adding Nodes
    g.addNode('A');
    g.addNode('B');
    g.addNode('C');
    g.addNode('D');
    g.addNode('E');
    g.addNode('F');

    // Connecting the Nodes
    g.addEdge('A', 'B', 4);
    g.addEdge('A', 'C', 2);
    g.addEdge('B', 'D', 10);
    g.addEdge('C', 'E', 3);
    g.addEdge('C', 'D', 5);
    g.addEdge('D', 'F', 1);
    g.addEdge('E', 'F', 5);

    // Adding Cycle to test Tarjan's Implementation
    g.addEdge('F', 'A', 2);

    // Adjacency List
    cout<<"Printing Adjacency List :"<< endl;
    g.printAdjacencyList();
    cout<<endl;

    cout<<"Graph Algorithms :"<<endl<<endl;

    // BFS    
    cout<<"BFS ( Starting Node 'A' ) : "<<endl;
    auto bfs_order = bfs(g,'A');
    for(auto i: bfs_order) {
        cout<<i<<" ";
    }
    cout<<endl<<endl;

    // DFS
    cout<<"DFS ( Starting Node 'A' ) : "<<endl;
    auto dfs_order = dfs(g, 'A');
    for(auto i: dfs_order) {
        cout<<i<<" ";
    }
    cout<<endl<<endl;

    // Cycle Detection
    cout<<"Cycle Detection :"<<endl;
    cout<<"Cycle Present : "<<(g.cyclePresent() ? "Yes" : "No")<<endl<<endl;

    // Kahn's Topological Sort
    cout<<"Kahn's Topological Sort :"<<endl;
    auto topo_order = kahn_topo_sort(g);
    if(topo_order.empty()) {
        cout<<"Cycle detected, not a DAG"<<endl;
    }

    else {
        for(auto i: topo_order) {
            cout<<i<<" ";
        }
    }
    cout<<endl<<endl;

    // Dijkstra
    cout<<"Dijkstra ( 'A' -> 'F' ) :"<<endl;
    auto shortest_path = dijkstra(g,'A','F');
    if( shortest_path.second.empty() ) {
        cout<<"Unreachable"<<endl;
    }

    else{
        cout<<"Cost :"<< shortest_path.first<<endl;
        cout<<"Path : ";
        for(auto i: shortest_path.second) {
            cout<<i<<" ";
        }
    }

    cout<<endl<<endl;

    // Bellman Ford
    cout<<"Bellman-Ford ( 'A' -> 'F' ) :"<<endl;
    auto bf = bellman_ford(g,'A','F');
    cout<<"Cost :"<<bf.first<<endl;
    cout<<"Path : ";
    for(auto i: bf.second) {
        cout<<i<<" ";
    }
    cout<<endl<<endl;

    // A* Search
    cout<<"A* Search ( 'A' -> 'F' ) :"<<endl;
    auto a_star_search =a_star(g,'A', 'F', 
    [](char, char) { 
            return 0.0; 
        }
    );

    cout<<"Cost :"<<a_star_search.first<<endl;
    cout<<"Path :";
    for( auto i: a_star_search.second) {
        cout<<i<<" ";
    }

    cout<<endl<<endl;

    // Floyd-Warshall
    cout<<"Floyd-Warshall :"<<endl;
    auto fw = floyd_warshall(g);
    cout<<"Shortest distance A->F : "<<fw.dist['A']['F'];
    cout<<endl<<endl;

    // Kosaraju's SCC
    cout<<"Kosaraju's SCC :"<<endl;
    auto kosaraju_components = kosaraju_scc(g);
    int component_no = 1;
    for(auto& i: kosaraju_components) {
        cout<<"Component "<<component_no++<<" : { ";
        for(auto j: i) {
            cout<<j<<" ";
        }
        cout<<"}"<<endl;
    }
    cout<<endl;

    // Tarjan's SCC
    cout<<"Tarjan's SCC :"<<endl;
    auto components = tarjan_scc(g);
    int component_number = 1;
    
    for(auto& i: components){
        cout<<"Component "<<component_number++<<" : { ";
        for(auto j: i) {
            cout<<j<<" ";
        }
        cout<<"}"<<endl;
    }
    cout<<endl;

    // Kruskal's MST     
    cout<<"Kruskal's MST :"<<endl;
    auto min_spanning_tree = kruskal_mst(g);
    cout<<"Total Weight :"<<min_spanning_tree.first<<endl;
    for(auto& i: min_spanning_tree.second) {
        cout<<"Edge :"<<i.first<<" - "<<i.second<<endl;
    }
    cout<<endl;

    // Node/Edge Removal
    cout<<"Node/Edge Removal"<<endl;
    cout<<"Removing edge A->B"<<endl;
    g.removeEdge('A','B');
    g.printAdjacencyList();
    cout<<endl;

    cout<<"Removing node 'D' (and all its edges)"<<endl;
    g.removeNode('D');
    g.printAdjacencyList();
    cout<<endl;

    // STL iterator
    cout<<"STL iterators"<<endl;
    cout<<"All remaining nodes via std::for_each :"<<endl;
    for_each(g.begin(), g.end(), 
    [](const auto& p) {
            cout<<p.second<<" ";
        }
    );
    cout << endl;

    return 0;
}