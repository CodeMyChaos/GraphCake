//
// Created by kwojc on 6.07.2026.
//

export module representations;
#include <algorithm>
#include <expected>
#include <list>
#include <ranges>
#include <vector>

namespace GraphCake {
    template<typename Vertex, typename Edge>
    struct EdgeInfo_t {
        Edge data;
        Vertex& src;
        Vertex& tgt;
    };

    enum class EdgeType_t {
        Unidirectional,
        Bidirectional
    };

    enum class SetError_t {
        SourceVertexNotExist,
        DestinationVertexNotExist
    };

    template<typename Vertex, typename Edge>
    class AdjacencyList {
    public:
        using VertexType = Vertex;
        using EdgeType = Edge;

        using EdgeInfo = EdgeInfo_t<VertexType, EdgeType>;

        template<EdgeType edge_type = EdgeType::Unidirectional>
        auto add_edge(const EdgeType& edge, const VertexType& source, const VertexType& target)
        -> std::expected<EdgeType&, SetError_t> {
            auto src_node = std::ranges::find(vertices, source, &Node::data);
            if (src_node == vertices.end()) return SetError_t::SourceVertexNotExist;

            auto tgt_node = std::ranges::find(vertices, target, &Node::data);
            if (tgt_node == vertices.end()) return SetError_t::DestinationVertexNotExist;

            auto& [src, src_edges] = *src_node;
            auto& [tgt, tgt_edges] = *tgt_node;
            auto ret = src_edges.emplace_back( EdgeInfo_t { .data = edge, .src = src, .tgt = tgt});

            if constexpr (edge_type == EdgeType::Bidirectional) {
                tgt_edges.emplace_back( EdgeInfo_t { .data = edge, .src = tgt, .tgt = src});
            }

            return ret;
        }

        auto add_vertex(const VertexType& vertex)
        -> VertexType& {
            return vertices.emplace_back( {.data = vertex, .edges = {} });
        }

        auto get_vertex_ref(const VertexType& old_vertex) -> std::optional<VertexType&> {
            auto node = std::ranges::find(vertices, old_vertex, &Node::data);
            return (node == vertices.end() ? std::nullopt : node->data);
        }

        auto get_edge_ref(const VertexType& source, const VertexType& target) -> std::optional<EdgeType&> {
            auto node = std::ranges::find(vertices, source, &Node::data);

            if (node == vertices.end()) return std::nullopt;

            auto& [_, edges] = *node;
            auto target_edge = std::ranges::find(edges, target, &Node::edges::data);

            return (target_edge == edges.end() ? std::nullopt : target_edge->data);
        }

        // Shared adapter API for library features
        auto get_vertices_refs() -> std::vector<VertexType &> {
            std::vector<VertexType&> vertices_refs = {};
            std::ranges::for_each(vertices, [&vertices_refs](auto& node) {
                auto& [v, _] = node;
                vertices_refs.emplace_back(v);
            });
            return vertices_refs;
        }

        // Shared adapter API for library features
        auto get_edges_refs() -> std::vector<EdgeInfo> {
            std::vector<EdgeInfo> edge_refs = {};

            vertices | std::views::transform(&Node::edges)
                     | std::views::join
                     | std::views::transform([&](auto& e) { edge_refs.emplace_back(e); });

            return edge_refs;
        }

    private:
        struct Node {
            VertexType data = {};
            std::vector<EdgeInfo> edges = {};
        };

        std::list<Node> vertices = {};
    };

    template<typename Vertex, typename Edge>
    class AdjacencyMatrix {
    public:
        using VertexType = Vertex;
        using EdgeType = Edge;

        std::vector<VertexType>& get_vertices() {
            return vertices;
        }

        std::vector<EdgeInfo>& get_edges() {
            return std::ranges::join_view(edges);
        }

        std::vector<VertexType> vertices;
        std::vector<std::vector<EdgeType>> edges;
    };
}
