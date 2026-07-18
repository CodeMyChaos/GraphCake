module;

#include <expected>

export module graphCake.representations;
import std;
import graphCake.concepts;
import graphCake.graph;

namespace graphCake::representations {
    export enum class SetError {
        Ok,
        SourceVertexNotExist,
        TargetVertexNotExist
    };

    export template<concepts::VertexCompliant Vertex, concepts::EdgeCompliant Edge>
    class AdjacencyList {
    public:
        using Vertex_t = Vertex;
        using Edge_t = Edge;

        using EdgeInfo = graph::EdgeInfo<Edge>;

        auto add_vertex() -> Vertex& {
            edges.emplace_back();
            return vertices.emplace_back();
        }

        auto add_vertex(Vertex&& vertex) -> Vertex& {
            edges.emplace_back();
            return vertices.emplace_back(std::forward<Vertex>(vertex));
        }

        auto vertex_count() -> std::size_t {
            return vertices.size();
        }

        template<graph::EdgeType edge_type = graph::EdgeType::Unidirectional>
        auto add_edge(std::size_t source_vertex, std::size_t target_vertex) -> SetError {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return SetError::SourceVertexNotExist;
            if (target_vertex >= vertices.size() or target_vertex >= edges.size()) return SetError::TargetVertexNotExist;

            auto& added_edge = edges.at(source_vertex).emplace_back();
            added_edge.target = target_vertex;

            if constexpr (edge_type == graph::EdgeType::Bidirectional) {
                if (source_vertex == target_vertex) return SetError::Ok;
                edges.at(target_vertex).emplace_back(added_edge);
            }
            return SetError::Ok;
        }

        template<graph::EdgeType edge_type = graph::EdgeType::Unidirectional>
        auto add_edge(std::size_t source_vertex, std::size_t target_vertex, Edge&& edge) -> SetError {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return SetError::SourceVertexNotExist;
            if (target_vertex >= vertices.size() or target_vertex >= edges.size()) return SetError::TargetVertexNotExist;

            auto& added_edge = edges.at(source_vertex).emplace_back(std::forward<Edge>(edge), target_vertex);

            if constexpr (edge_type == graph::EdgeType::Bidirectional) {
                if (source_vertex == target_vertex) return SetError::Ok;
                edges.at(target_vertex).emplace_back(added_edge);
            }
            return SetError::Ok;
        }

        [[nodiscard]] auto get_edge(std::size_t source_vertex, std::size_t target_vertex) -> std::optional<EdgeInfo> {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return std::nullopt;
            if (target_vertex >= vertices.size() or target_vertex >= edges.size()) return std::nullopt;

            auto edge = std::ranges::find(edges.at(source_vertex), target_vertex, &TargetedEdge::target);

            if (edge == edges.at(source_vertex).end()) return std::nullopt;
            return EdgeInfo {.data = edge->edge, .source = source_vertex, .target = edge->target};
        }

        [[nodiscard]] auto get_outbound_edges_of(std::size_t vertex) -> std::vector<EdgeInfo> {
            if (vertex >= vertices.size() or vertex >= edges.size()) return {};

            auto toEdgeInfo = [&](const auto& edge) {
                return EdgeInfo{.data = edge.edge, .source = vertex, .target = edge.target};
            };
            return edges.at(vertex) | std::views::transform(toEdgeInfo);
        }

        [[nodiscard]] auto get_inbound_edges_of(std::size_t vertex) -> std::vector<EdgeInfo> {
            if (vertex >= vertices.size() or vertex >= edges.size()) return {};

            auto isVertexTarget = [&](const auto& target) {
                return target == vertex;
            };
            return get_all_edges() | std::views::filter(isVertexTarget, &EdgeInfo::target);
        }

        [[nodiscard]] auto get_all_vertices() -> std::vector<Vertex&> {
            return { vertices };
        }

        [[nodiscard]] auto get_all_edges() -> std::vector<EdgeInfo> {
            std::vector<EdgeInfo> all_edges = {};

            for (const auto index : std::views::iota(0, edges.size())) {
                auto toEdgeInfo = [&](const auto& edge) {
                    return EdgeInfo{.data = edge.edge, .source = index, .target = edge.target};
                };
                all_edges.emplace_back(edges.at(index) | std::views::transform(toEdgeInfo));
            }
            return all_edges;
        }

        auto remove_vertex(std::size_t vertex) -> void {
            if (vertex >= vertices.size() or vertex >= edges.size()) return;

            vertices.remove(vertex);
            edges.remove(vertex);
            std::ranges::remove(edges | std::views::join, vertex, &TargetedEdge::target);
        }

        template<graph::EdgeType edge_type = graph::EdgeType::Unidirectional>
        auto remove_edge(std::size_t source_vertex, std::size_t target_vertex) -> void {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return;
            if (target_vertex >= vertices.size() or target_vertex >= edges.size()) return;

            std::ranges::remove(edges.at(source_vertex), target_vertex, &TargetedEdge::target);

            if constexpr (edge_type == graph::EdgeType::Bidirectional) {
                std::ranges::remove(edges.at(target_vertex), source_vertex, &TargetedEdge::target);
            }
        }

    private:
        struct TargetedEdge {
            Edge edge{};
            std::size_t target{};
        };

        std::vector<Vertex> vertices = {};
        std::vector<std::vector<TargetedEdge>> edges = {};
    };

    export template<concepts::VertexCompliant Vertex, concepts::EdgeCompliant Edge>
    class AdjacencyMatrix {
    public:
        using Vertex_t = Vertex;
        using Edge_t = Edge;

        using EdgeInfo = graph::EdgeInfo<Edge>;

        auto add_vertex(Vertex&& vertex = {}) -> Vertex& {
            edges.emplace_back({edges.size(), std::nullopt});
            for (auto& row : edges) {
                row.emplace_back(std::nullopt);
            }
            return vertices.emplace_back(std::forward<Vertex>(std::move(vertex)));
        }

        auto vertex_count() -> std::size_t {
            return vertices.size();
        }

        template<graph::EdgeType edge_type = graph::EdgeType::Unidirectional>
        auto add_edge(std::size_t source_vertex, std::size_t target_vertex, Edge&& edge = {}) -> SetError {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return SetError::SourceVertexNotExist;
            if (target_vertex >= vertices.size() or target_vertex >= edges.at(source_vertex).size()) return SetError::TargetVertexNotExist;

            if constexpr (edge_type == graph::EdgeType::Bidirectional) {
                if (target_vertex >= edges.size()) return SetError::TargetVertexNotExist;
                if (source_vertex >= edges.at(target_vertex).size()) return SetError::SourceVertexNotExist;
            }

            auto& added_edge = (edges[source_vertex][target_vertex] = std::forward<Edge>(std::move(edge)));

            if constexpr (edge_type == graph::EdgeType::Bidirectional) {
                edges[target_vertex][source_vertex] = added_edge;
            }

            return SetError::Ok;
        }

        [[nodiscard]] auto get_edge(std::size_t source_vertex, std::size_t target_vertex) -> std::optional<EdgeInfo> {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return std::nullopt;
            if (target_vertex >= vertices.size() or target_vertex >= edges.at(source_vertex).size()) return std::nullopt;

            if (not edges[source_vertex][target_vertex].has_value()) {
                return std::nullopt;
            }
            return EdgeInfo {.data = edges[source_vertex][target_vertex].value(), .source = source_vertex, .target = target_vertex};
        }

        [[nodiscard]] auto get_outbound_edges_of(std::size_t vertex) -> std::vector<EdgeInfo> {
            if (vertex >= vertices.size() or vertex >= edges.size()) return {};

            std::vector<EdgeInfo> outbound_edges = {};
            for (auto index : std::views::iota(0, edges[vertex].size())) {
                if (edges[vertex][index].has_value()) {
                    outbound_edges.emplace_back(EdgeInfo{.data = edges[vertex][index].value(), .source = vertex, .target = index});
                }
            }
            return outbound_edges;
        }

        [[nodiscard]] auto get_inbound_edges_of(std::size_t vertex) -> std::vector<EdgeInfo> {
            if (vertex >= vertices.size() or vertex >= edges.size()) return {};

            std::vector<EdgeInfo> inbound_edges = {};
            for (auto index : std::views::iota(0, edges.size())) {
                if (vertex >= edges[index].size()) throw std::runtime_error{"AdjacencyMatrix is not square"};

                if (edges[index][vertex].has_value()) {
                    inbound_edges.emplace_back(EdgeInfo{.data = edges[index][vertex].value(), .source = index, .target = vertex});
                }
            }
            return inbound_edges;
        }

        [[nodiscard]] auto get_all_vertices() -> std::vector<Vertex&> {
            return { vertices };
        }

        [[nodiscard]] auto get_all_edges() -> std::vector<EdgeInfo> {
            std::vector<EdgeInfo> all_edges = {};

            for (const auto& source : std::views::iota(0, edges.size())) {
                for (const auto& target : std::views::iota(0, edges[source].size())) {
                    if (edges[source][target].has_value()) {
                        all_edges.emplace_back(EdgeInfo {.data = edges[source][target].value(), .source = source, .target = target});
                    }
                }
            }
            return all_edges;
        }

        auto remove_vertex(std::size_t vertex) -> void {
            if (vertex >= vertices.size() or vertex >= edges.size()) return;

            vertices.remove(vertex);
            edges.remove(vertex);
            std::ranges::for_each(edges, [&](auto& row) {
                row.remove(vertex);
            });
        }

        template<graph::EdgeType edge_type = graph::EdgeType::Unidirectional>
        auto remove_edge(std::size_t source_vertex, std::size_t target_vertex) -> void {
            if (source_vertex >= vertices.size() or source_vertex >= edges.size()) return;
            if (target_vertex >= vertices.size() or target_vertex >= edges.at(source_vertex).size()) return;

            edges[source_vertex][target_vertex] = std::nullopt;
        }

    private:
        std::vector<Vertex> vertices = {};
        std::vector<std::vector<std::optional<Edge>>> edges = {};
    };
}
