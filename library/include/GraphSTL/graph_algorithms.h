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
}

#endif