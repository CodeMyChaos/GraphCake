#pragma once

#include <algorithm>
#include <compare>
#include <cstdint>
#include <iostream>
#include <optional>
#include <ranges>
#include <vector>

namespace Graphs {
using NodeId     = uint32_t;
using WeightType = int32_t;

struct EdgeInfo {
    NodeId                    source;
    NodeId                    destination;
    std::optional<WeightType> weight;

    auto operator<=>(const EdgeInfo&) const = default;
};

enum class GraphDirectionality {
    directed,
    undirected
};

class GraphReader {
public:
    friend std::ostream& operator<<(std::ostream& os, const GraphReader& g) {
        os << g.show();
        return os;
    }

    virtual EdgeInfo findEdge(const EdgeInfo&) const = 0;
    virtual uint32_t nodesAmount() const             = 0;
    virtual uint32_t getIncomingDegree(NodeId) const = 0;
    virtual uint32_t getOutgoingDegree(NodeId) const = 0;

    virtual std::vector<NodeId>   getNodeIds() const                   = 0;
    virtual std::vector<NodeId>   getOutgoingNeighborsOf(NodeId) const = 0;
    virtual std::vector<NodeId>   getIncomingNeighborsOf(NodeId) const = 0;
    virtual std::vector<EdgeInfo> getEdges() const                     = 0;

    virtual GraphDirectionality getDirectionality() const = 0;

    virtual uint32_t graphDegree() const {
        uint32_t graphDegree = 0;
        for (auto node : getNodeIds()) {
            auto maxNodeDegree = std::max(getIncommingDegree(node), getOutgoingDegree(node));
            graphDegree        = std::max(graphDegree, maxNodeDegree);
        }
        return graphDegree;
    }

    virtual std::vector<NodeId> getNeighborsOf(NodeId node) const {
        auto neighbors = getOutgoingNeighborsOf(node);
        auto inNeighborsView =
            getIncommingNeighborsOf(node) | std::views::filter([neighbors](auto& elem) {
                return std::ranges::find(neighbors, elem) == neighbors.end();
            });

        std::ranges::copy(inNeighborsView, std::back_inserter(neighbors));
        std::ranges::sort(neighbors);
        return neighbors;
    }

    auto operator<=>(const GraphReader& other) const {
        return compare(other);
    }

    bool operator==(const GraphReader& other) const {
        return compare(other) == std::strong_ordering::equal;
    }

    virtual ~GraphReader() = default;

protected:
    virtual std::string show() const = 0;

    virtual std::strong_ordering compare(const GraphReader& other) const {
        auto nodeComp = nodesAmount() <=> other.nodesAmount();
        if (nodeComp == std::strong_ordering::less or nodeComp == std::strong_ordering::greater) {
            return nodeComp;
        }
        bool equal = true;
        for (auto& srcNode : getNodeIds()) {
            if (getNeighborsOf(srcNode) != other.getNeighborsOf(srcNode)) {
                equal = false;
                break;
            }
            for (auto& tgtNode : getNodeIds()) {
                if (findEdge({srcNode, tgtNode}) != other.findEdge({srcNode, tgtNode})) {
                    equal = false;
                    break;
                }
            }
        }
        return equal ? std::strong_ordering::equal : std::strong_ordering::equivalent;
    }
};

class GraphWriter {
public:
    virtual void setEdge(const EdgeInfo&) = 0;

    virtual void setEdges(const std::vector<EdgeInfo>& edges) {
        for (const auto& edge : edges) { setEdge(edge); }
    }

    virtual void addNodes(uint32_t)          = 0;
    virtual void removeNode(NodeId)          = 0;
    virtual void removeEdge(const EdgeInfo&) = 0;
    virtual void reset()                     = 0;
};

class Graph : public GraphReader, public GraphWriter {
public:
    Graph()          = default;
    virtual ~Graph() = default;
};
}   // namespace Graphs
