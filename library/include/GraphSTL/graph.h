#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <unordered_map>
#include <map>
#include <algorithm>
#include <queue>
#include <stack>
#include <unordered_set>
#include <functional>
#include <limits>
#include <cmath>
#include <stdexcept>
#include <iostream>
#include <iterator>

// Namespace : custom_stl
namespace custom_stl {

    // Edge structure
    template <typename Weight >
    struct Edge {
        size_t targetNodeID;
        Weight weight;
    };

    // DSU (Disjoint Set Union) helper for Kruskal's Algorithm
    struct DSU {
        std::vector<size_t> parent;
        std::vector<size_t > rank;
        DSU(size_t n) : parent(n), rank(n, 0) {
            for (size_t i = 0; i < n; ++i) parent[i] = i;
        }

        size_t find(size_t i) {
            if (parent[i] == i) return i;
            return parent[i] = find(parent[i]);
        }

        void unite(size_t i, size_t j) {
            size_t ri = find(i), rj = find(j);
            if (ri == rj) return;
            if (rank[ri] <rank[rj]) std::swap(ri, rj);
            parent[rj] = ri;
            if (rank[ri] == rank[rj]) ++rank[ri];
        }
    };

    template <typename T, typename Weight = int, bool IsDirected = false>
    class graph {
    public:
        using NodeID = size_t;

        // STL Compliant Forward Iterator
        class NodeIterator{
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<const NodeID, T>;
            using pointer = value_type*;
            using reference = value_type&;

            // Default ctor
            NodeIterator() = default;

            explicit NodeIterator(typename std::unordered_map<NodeID, T>::iterator iter): internalIter(iter) {}

            reference operator*() const{ 
                return *internalIter; 
            }

            pointer operator->() const{
                return &(*internalIter); 
            }

            NodeIterator& operator++(){ 
                ++internalIter; return *this; 
            }

            NodeIterator operator++(int) { 
                NodeIterator tmp = *this; 
                ++internalIter; 
                return tmp; 
            }

            friend bool operator==(const NodeIterator& a, const NodeIterator& b){
                return a.internalIter == b.internalIter;
            }

            friend bool operator!=(const NodeIterator& a, const NodeIterator& b) {
                return a.internalIter != b.internalIter;
            }

        private:
            typename std::unordered_map<NodeID, T>::iterator internalIter{};
        };

        class ConstNodeIterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = std::pair<const NodeID, T>;
            using pointer = const value_type*;
            using reference = const value_type&;

            ConstNodeIterator() = default;

            explicit ConstNodeIterator(typename std::unordered_map<NodeID,T>::const_iterator iter) : internalIter(iter){}

            reference operator*() const { 
                return *internalIter; 
            }

            pointer operator->() const { 
                return &(*internalIter); 
            }

            ConstNodeIterator& operator++() { 
                ++internalIter; 
                return *this; 
            }

            ConstNodeIterator operator++(int) { 
                ConstNodeIterator tmp = *this; 
                ++internalIter; 
                return tmp; 
            }

            friend bool operator==(const ConstNodeIterator& a, const ConstNodeIterator& b) {
                return a.internalIter == b.internalIter;
            }

            friend bool operator!=(const ConstNodeIterator& a, const ConstNodeIterator& b) {
                return a.internalIter != b.internalIter;
            }

        private:
            typename std::unordered_map<NodeID, T>::const_iterator internalIter{};
        };

        using iterator = NodeIterator;
        using const_iterator = ConstNodeIterator;

    private:
        std::unordered_map<NodeID, T> nodeData;
        std::unordered_map<T, NodeID> reverseNodeData;
        std::unordered_map<NodeID, std::vector<Edge<Weight>>> adjList;
        NodeID nextID = 0;

        NodeID getID(const T& data) const{
            auto it = reverseNodeData.find(data);
            if (it == reverseNodeData.end() ){
                throw std::runtime_error("Node not found");
            }
            return it->second;
        }

    public:
        //Using compiler defaults for RAII
        graph() = default;
        ~graph() = default;
        graph(const graph& other) = default;
        graph& operator=(const graph& other) = default;
        graph(graph&& other) noexcept = default;
        graph& operator=(graph&& other) noexcept = default;

        // Iterators
        iterator begin(){ 
            return iterator( nodeData.begin() );
        }

        iterator end(){ 
            return iterator( nodeData.end() );
        }

        const_iterator begin() const{ 
            return const_iterator( nodeData.begin() );
        }

        const_iterator end() const { 
            return const_iterator( nodeData.end() ); 
        }

        const_iterator cbegin() const { 
            return const_iterator( nodeData.begin() );
        }

        const_iterator cend() const{ 
            return const_iterator( nodeData.end() );
        }

        // Public Accessors
        const std::unordered_map<NodeID, T>& getData() const { 
            return nodeData; 
        }

        const std::unordered_map<NodeID, std::vector<Edge<Weight>>>& getAdjList() const { 
            return adjList; 
        }

        NodeID getNodeID(const T& data) const{ 
            return getID(data); 
        }

        const T& getNodeData(NodeID id) const { 
            return nodeData.at(id); 
        }

        size_t nodeCount() const{ 
            return nodeData.size(); 
        }

        NodeID getNextID() const{ 
            return nextID; 
        }

        bool hasNode(const T& data) const {
            return reverseNodeData.find(data) != reverseNodeData.end();
        }

        static constexpr bool isDirected() { 
            return IsDirected; 
        }

        // Core Operations
        NodeID addNode(const T& data) {
            if (reverseNodeData.find(data) != reverseNodeData.end()) {
                return reverseNodeData[data];
            }
            NodeID id = nextID++;
            nodeData[id] = data;
            reverseNodeData[data] = id;
            return id;
        }

        void addEdge(const T& srcData, const T& destData, Weight weight) {
            NodeID src  = getID(srcData);
            NodeID dest = getID(destData);
            adjList[src].push_back({dest, weight});
            if (!IsDirected) {
                adjList[dest].push_back({src, weight});
            }
        }

         // Remove a node and all edges connected to it.
        void removeNode(const T& data) {
            NodeID id = getID(data);

            adjList.erase(id);

            for (auto& kv : adjList) {
                auto& edges = kv.second;
                edges.erase(
                    std::remove_if(edges.begin(), edges.end(),
                        [id](const Edge<Weight>& e) { 
                            return e.targetNodeID == id; 
                        }
                    ),
                    edges.end());
            }

            reverseNodeData.erase(data);
            nodeData.erase(id);
        }

        // Remove a specific edge (src → dest).  
        // For undirected graphs the reverse edge (dest → src) is also removed.
        void removeEdge(const T& srcData, const T& destData) {
            NodeID src = getID(srcData);
            NodeID dest = getID(destData);

            auto eraseTarget = []( std::vector<Edge<Weight>>& edges, NodeID target) {
                edges.erase(
                    std::remove_if(edges.begin(), edges.end(),
                        [target](const Edge<Weight>& e) { 
                            return e.targetNodeID == target; 
                        }
                    ),
                    edges.end());
            };

            if (adjList.count(src)) { 
                eraseTarget(adjList[src], dest);
            }

            if (!IsDirected && adjList.count(dest)) { 
                eraseTarget(adjList[dest], src);
            }
        }

        // Utility
        void printAdjacencyList() const {

            for (NodeID id = 0; id < nextID; ++id) {
                if (nodeData.find(id) == nodeData.end()) {
                    continue;
                }
                const T& srcData = nodeData.at(id);
                std::cout << srcData << " -> ";

                if (adjList.count(id)){
                    for (const auto& edge : adjList.at(id)) {
                        std::cout << nodeData.at(edge.targetNodeID)
                                  << "(" << edge.weight << ") ";
                    }
                }
                std::cout << "\n";
            }
        }

        // Cycle Detection — returns true if the graph contains a cycle
        bool cyclePresent() const {
            if constexpr (IsDirected) {
                // 0 = WHITE (unvisited), 1 = GRAY (in stack), 2 = BLACK (done)
                std::unordered_map<NodeID, int> color;
                for (const auto& kv : nodeData)
                    color[kv.first] = 0;

                std::function<bool(NodeID)> dfsVisit = [&](NodeID u) -> bool {
                    color[u] = 1;
                    auto it = adjList.find(u);
                    if (it != adjList.end()) {
                        for (const auto& edge : it->second) {
                            if (color[edge.targetNodeID] == 1) {
                                return true;  
                            }

                            if (color[edge.targetNodeID] == 0 && dfsVisit(edge.targetNodeID)) {
                                return true;
                            }
                        }
                    }
                    color[u] = 2;
                    return false;
                };

                for (const auto& kv : nodeData) {
                    if (color[kv.first] == 0 && dfsVisit(kv.first)) {
                        return true;
                    }
                }

                return false;

            }

            else {
                // Undirected: visited node that isn't parent → cycle
                std::unordered_set<NodeID> visited;

                std::function<bool(NodeID, NodeID)> dfsVisit =
                    [&](NodeID u, NodeID parent) -> bool {
                    visited.insert(u);
                    auto it = adjList.find(u);
                    if (it != adjList.end()) {
                        for (const auto& edge : it->second) {
                            if (visited.find(edge.targetNodeID) == visited.end()) {
                                if (dfsVisit(edge.targetNodeID, u))
                                    return true;
                            }
                            else if (edge.targetNodeID != parent) {
                                return true;
                            }
                        }
                    }
                    return false;
                };

                for (const auto& kv : nodeData) {
                    if (visited.find(kv.first) == visited.end()) {
                        NodeID sentinel = std::numeric_limits<NodeID>::max();
                        if (dfsVisit(kv.first, sentinel)) {
                            return true;
                        }
                    }
                }
                return false;
            }
        }
    };
}

#endif 