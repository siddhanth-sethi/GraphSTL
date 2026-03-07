#ifndef GRAPH_ALGORITHMS_H
#define GRAPH_ALGORITHMS_H

#include "graph.h"

#include <vector>
#include <queue>
#include <stack>
#include <unordered_set>
#include <unordered_map>
#include <functional>
#include <limits>
#include <algorithm>
#include <cmath>

namespace custom_stl {

    // BFS — returns visited nodes in breadth-first order
    template <typename T, typename Weight, bool IsDirected> 
    std::vector<T> bfs(const graph<T, Weight, IsDirected>& g, const T& startData) {
        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        std::vector<T> result;
        NodeID startID = g.getNodeID(startData);
        const auto& adj = g.getAdjList();

        std::queue<NodeID> q;
        std::unordered_set<NodeID> visited;

        q.push(startID);
        visited.insert(startID);

        while (!q.empty()) {
            NodeID curr = q.front();
            q.pop();
            result.push_back(g.getNodeData(curr));

            auto it = adj.find(curr);
            if (it != adj.end()) {
                for (const auto& edge : it->second) {
                    if (visited.find(edge.targetNodeID) == visited.end()) {
                        visited.insert(edge.targetNodeID);
                        q.push(edge.targetNodeID);
                    }
                }
            }
        }
        return result;
    }

    // DFS — returns visited nodes in depth-first order
    template <typename T, typename Weight, bool IsDirected> 
    std::vector<T> dfs(const graph<T, Weight, IsDirected>& g, const T& startData) {

        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        std::vector<T> result;
        NodeID startID = g.getNodeID(startData);
        const auto& adj = g.getAdjList();

        std::stack<NodeID> s;
        std::unordered_set<NodeID> visited;
        s.push(startID);

        while(!s.empty()) {
            NodeID curr = s.top();
            s.pop();

            if(visited.find(curr) == visited.end()) {
                visited.insert(curr);
                result.push_back(g.getNodeData(curr));

                auto it = adj.find(curr);
                if(it != adj.end()) {
                    const auto& edges = it->second;
                    for(auto rit = edges.rbegin(); rit != edges.rend(); ++rit) {
                        if(visited.find(rit->targetNodeID) == visited.end()) {
                            s.push(rit->targetNodeID);
                        }
                    }
                }
            }
        }
        return result;
    }

    // Dijkstra SSSP — returns {cost, path_vector}
    template <typename T, typename Weight, bool IsDirected> 
    std::pair<Weight, std::vector<T>> dijkstra(const graph<T, Weight, IsDirected>& g ,const T& srcData, const T& destData) {

        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        NodeID startID = g.getNodeID(srcData);
        NodeID endID = g.getNodeID(destData);
        const auto& nodeMap = g.getData();
        const auto& adj = g.getAdjList();

        std::unordered_map<NodeID, Weight> dist;
        std::unordered_map<NodeID, NodeID> parent;

        for (const auto& kv : nodeMap) {
            dist[kv.first] = std::numeric_limits<Weight>::max();
        }

        dist[startID] = Weight{0};

        using PQ = std::pair<Weight, NodeID>;
        std::priority_queue<PQ, std::vector<PQ>, std::greater<PQ>> pq;
        pq.push({Weight{0}, startID});

        while(!pq.empty()) {
            Weight d = pq.top().first;
            NodeID u = pq.top().second;
            pq.pop();

            if(d > dist[u]) {
                continue;
            }

            if(u == endID) {
                break;
            }

            auto it = adj.find(u);

            if(it != adj.end()) {
                for(const auto& edge : it->second) {
                    Weight newDist = dist[u] + edge.weight;
                    if(newDist < dist[edge.targetNodeID]) {
                        dist[edge.targetNodeID] = newDist;
                        parent[edge.targetNodeID] = u;
                        pq.push({newDist, edge.targetNodeID});
                    }
                }
            }
        }

        std::vector<T> path;
        if(dist[endID] == std::numeric_limits<Weight>::max()) {
            return {dist[endID], path};
        }

        for(NodeID at = endID; at != startID; at = parent[at]) {
            path.push_back(g.getNodeData(at));
        }

        path.push_back(g.getNodeData(startID));
        std::reverse(path.begin(), path.end());

        return {dist[endID], path};
    }

    
    // Kahn's Topological Sort — returns vector<T> in topological order
    // Returns empty vector if graph has a cycle (not a DAG) , it is only meaningful for directed graphs.

    template <typename T, typename Weight, bool IsDirected>
    std::vector<T> kahn_topo_sort(const graph<T, Weight, IsDirected>& g) {
        static_assert(IsDirected, "Kahn's topological sort requires a directed graph");

        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        const auto& nodeMap = g.getData();
        const auto& adj= g.getAdjList();

        std::unordered_map<NodeID, size_t> inDegree;
        for (const auto& kv : nodeMap) {
            inDegree[kv.first] = 0;
        }

        for (const auto& kv : adj) {
            for (const auto& edge : kv.second) {
                inDegree[edge.targetNodeID]++;
            }
        }

        std::queue<NodeID> q;
        for (const auto& kv : inDegree) {
            if (kv.second == 0) {
                q.push(kv.first);
            }
        }

        std::vector<T> result;
        result.reserve(nodeMap.size());

        while( !q.empty()) {
            NodeID curr = q.front();
            q.pop();
            result.push_back(g.getNodeData(curr));

            auto it = adj.find(curr);
            if (it != adj.end()) {
                for (const auto& edge : it->second) {
                    inDegree[edge.targetNodeID]--;
                    if (inDegree[edge.targetNodeID] == 0) {
                        q.push(edge.targetNodeID);
                    }
                }
            }
        }

        if (result.size() != nodeMap.size()) {
            // Cycle Detected
            return {}; 
        }

        return result;
    }

    // Bellman-Ford SSSP — returns {cost, path} , works with negative weights
    // In case a negative cycle is reachable, it returns { numeric_limits::min(), {} }
    // Complexity: O(V * E)
    template <typename T, typename Weight, bool IsDirected>
    std::pair<Weight, std::vector<T>> bellman_ford(const graph<T, Weight, IsDirected>& g,const T& srcData,const T& destData) {

        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        NodeID startID = g.getNodeID(srcData);
        NodeID endID = g.getNodeID(destData);
        const auto& nodeMap = g.getData();
        const auto& adj = g.getAdjList();

        struct FlatEdge { 
            NodeID u;
            NodeID v;
            Weight w; 
        };

        std::vector<FlatEdge> edges;

        for (const auto& kv : adj) {
            for (const auto& edge : kv.second) {
                edges.push_back({kv.first, edge.targetNodeID, edge.weight});
            }
        }

        std::unordered_map<NodeID, Weight> dist;
        std::unordered_map<NodeID, NodeID> parent;

        for (const auto& kv : nodeMap) {
            dist[kv.first] = std::numeric_limits<Weight>::max();
        }

        dist[startID] = Weight{0};

        size_t V = nodeMap.size();

        for(size_t i = 0; i < V - 1; ++i) {
            bool updated = false;

            for (const auto& e : edges) {
                if (dist[e.u] != std::numeric_limits<Weight>::max() && dist[e.u] + e.w < dist[e.v]) {
                    dist[e.v] = dist[e.u] + e.w;
                    parent[e.v] = e.u;
                    updated = true;
                }
            }

            if (!updated) {
                break;
              } 
        }

        // Negative cycle check
        for(const auto& e: edges) {
            if(dist[e.u] != std::numeric_limits<Weight>::max() && dist[e.u] + e.w < dist[e.v]) {
                // Negative cycle detected
                return {std::numeric_limits<Weight>::min(), {}};
            }
        }

        std::vector<T> path;

        if(dist[endID] == std::numeric_limits<Weight>::max()) {
            return {dist[endID], path};
        }

        for(NodeID at = endID; at != startID; at = parent[at]) {
            path.push_back(g.getNodeData(at));
        }
        
        path.push_back(g.getNodeData(startID));
        std::reverse(path.begin(), path.end());

        return {dist[endID], path};
    }

    // Kruskal's MST — returns {total_weight, edge_list}
    template <typename T, typename Weight, bool IsDirected>
    std::pair<Weight, std::vector<std::pair<T, T>>> kruskal_mst(const graph<T, Weight, IsDirected>& g) {
        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        const auto& adj = g.getAdjList();

        std::vector<std::pair<T, T>> mst_edges;
        Weight total_weight = Weight{0};

        struct FlatEdge { 
            NodeID u;
            NodeID v;
            Weight w; 
        };

        std::vector<FlatEdge> allEdges;

        for (const auto& kv : adj){
            NodeID u = kv.first;
            for (const auto& edge : kv.second) {
                if( IsDirected || u < edge.targetNodeID) {
                    allEdges.push_back({u, edge.targetNodeID, edge.weight});
                }
            }
        }

        std::sort(allEdges.begin(), allEdges.end(),
        [](const FlatEdge& a, const FlatEdge& b) { 
            return a.w < b.w; 
        }
    );

        DSU dsu(g.getNextID());

        for (const auto& edge: allEdges) {
            if (dsu.find(edge.u) != dsu.find(edge.v)) {
                dsu.unite(edge.u, edge.v);
                total_weight += edge.w;
                mst_edges.push_back({g.getNodeData(edge.u), g.getNodeData(edge.v)});
            }
        }
        return {total_weight, mst_edges};
    }

    // Kosaraju's SCC ( Strongly Connected Components )
    // Complexity: O(V + E)
    template <typename T, typename Weight, bool IsDirected>
    std::vector<std::vector<T>> kosaraju_scc(const graph<T, Weight, IsDirected>& g){

        static_assert(IsDirected, "Kosaraju's SCC requires a directed graph");

        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        const auto& nodeMap = g.getData();
        const auto& adj = g.getAdjList();

        std::unordered_set<NodeID> visited;
        std::vector<NodeID> finishOrder;

        finishOrder.reserve(nodeMap.size());

        std::function<void(NodeID)> dfs1 = [&](NodeID u) {
            visited.insert(u);
            auto it = adj.find(u);

            if (it != adj.end()) {
                for (const auto& edge : it->second) {
                    if (visited.find(edge.targetNodeID) == visited.end()) {
                        dfs1(edge.targetNodeID);
                    }
                }
            }
            finishOrder.push_back(u);
        };

        for (const auto& kv : nodeMap) {
            if (visited.find(kv.first) == visited.end()) {
                dfs1(kv.first);
            }
        }

        std::unordered_map<NodeID,std::vector<NodeID>> transposed;

        for (const auto& kv : adj) {
            for (const auto& edge : kv.second) {
                transposed[edge.targetNodeID].push_back(kv.first);
            }
        }

        visited.clear();
        std::vector<std::vector<T>> result;

        std::function<void(NodeID, std::vector<T>&)> dfs2 = [&](NodeID u, std::vector<T>& comp) {
            visited.insert(u);
            comp.push_back(g.getNodeData(u));
            auto it = transposed.find(u);

            if( it != transposed.end()) {
                for( NodeID v : it->second ){
                    if (visited.find(v) == visited.end()) {
                        dfs2(v, comp);
                    }
                }
            }
        };

        for (auto it = finishOrder.rbegin(); it != finishOrder.rend(); ++it) {
            if (visited.find(*it) == visited.end()) {
                std::vector<T> component;

                dfs2(*it, component);
                result.push_back(std::move(component));
            }
        }

        return result;
    }

    // Tarjan's SCC — returns vector of Strongly Connected Components
    template <typename T, typename Weight, bool IsDirected>
    std::vector<std::vector<T>> tarjan_scc(const graph<T, Weight, IsDirected>& g) {
        using NodeID = typename graph<T, Weight, IsDirected>::NodeID;

        const auto& nodeMap = g.getData();
        const auto& adj= g.getAdjList();

        std::vector<std::vector<T>> result;
        std::unordered_map<NodeID, int>  ids, low;
        std::unordered_map<NodeID, bool> onStack;
        std::stack<NodeID> st;

        int idCounter = 0;

        for (const auto& kv : nodeMap) {
            ids[kv.first] = -1;
            onStack[kv.first] = false;
        }

        std::function<void(NodeID)> strongConnect = [&](NodeID at) {
            st.push(at);
            onStack[at] = true;
            ids[at] = low[at] = idCounter++;

            auto it = adj.find(at);

            if (it != adj.end()) {
                for (const auto& edge : it->second) {
                    NodeID to = edge.targetNodeID;
                    if (ids[to] == -1) {
                        strongConnect(to);
                        low[at] = std::min(low[at], low[to]);
                    }

                    else if (onStack[to]) {
                        low[at] = std::min(low[at], ids[to]);
                    }
                }
            }

            if (ids[at] == low[at]) {
                std::vector<T> component;

                while (true) {
                    NodeID node = st.top();
                    st.pop();

                    onStack[node] = false;
                    component.push_back(g.getNodeData(node));

                    if (node == at) {
                        break;
                    }
                }
                result.push_back(component);
            }
        };

        for (const auto& kv : nodeMap) {
            if (ids[kv.first] == -1) {
                strongConnect(kv.first);
            }
        }
        return result;
    }
    
}

#endif