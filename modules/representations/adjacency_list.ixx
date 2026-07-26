export module graph_cake.representations:adjacency_list;

import graph_cake.concepts;
import graph_cake.types;
import std;

namespace graph_cake {
export template<vertex_like _vertex, edge_like _edge>
class adjacency_list {
    using info = edge_info<_edge&>;

public:
    using vertex = _vertex;
    using edge   = _edge;

    auto add_vertex() -> _vertex& {
        edges.emplace_back();
        return vertices.emplace_back();
    }

    auto add_vertex(_vertex&& vertex) -> _vertex& {
        edges.emplace_back();
        return vertices.emplace_back(std::forward<_vertex>(vertex));
    }

    auto vertex_count() -> std::size_t {
        return vertices.size();
    }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto add_edge(std::size_t source_vertex, std::size_t target_vertex) -> bool try {
        auto& added_edge = edges.at(source_vertex).emplace_back(source_vertex, target_vertex);
        if constexpr (_edge_type == edge_type::bidirectional) {
            if (source_vertex == target_vertex) return true;
            edges.at(target_vertex).emplace_back(added_edge);
        }
        return true;
    } catch (std::out_of_range&) { return false; }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto add_edge(std::size_t source_vertex, std::size_t target_vertex, _edge&& edge) -> bool try {
        auto& added_edge =
            edges.at(source_vertex)
                .emplace_back(source_vertex, target_vertex, std::forward<_edge>(edge));

        if constexpr (_edge_type == edge_type::bidirectional) {
            if (source_vertex == target_vertex) return true;
            edges.at(target_vertex).emplace_back(added_edge);
        }
        return true;
    } catch (std::out_of_range&) { return false; }

    [[nodiscard]] auto get_edge(std::size_t source_vertex, std::size_t target_vertex)
        -> std::optional<info> try {
        auto edge =
            std::ranges::find(edges.at(source_vertex), target_vertex, &edge_info<_edge>::target);
        if (edge == edges.at(source_vertex).end()) return std::nullopt;
        return info {.source = source_vertex, .target = target_vertex, .data = *edge};
    } catch (std::out_of_range&) { return std::nullopt; }

    [[nodiscard]] auto get_outbound_edges_of(std::size_t vertex) -> std::vector<info&> try {
        return {edges.at(vertex)};
    } catch (std::out_of_range&) { return {}; }

    [[nodiscard]] auto get_inbound_edges_of(std::size_t vertex) -> std::vector<info&> try {
        auto isVertexTarget = [&](const auto& target) {
            return target == vertex;
        };
        return get_all_edges() | std::views::filter(isVertexTarget, &info::target);
    } catch (std::out_of_range&) { return {}; }

    [[nodiscard]] auto get_all_vertices() -> std::vector<_vertex&> {
        return {vertices};
    }

    [[nodiscard]] auto get_all_edges() -> std::vector<info&> {
        return edges | std::views::join;
    }

    auto remove_vertex(std::size_t vertex) -> void {
        if (vertex < vertices.size()) vertices.erase(vertices.begin() + vertex);
        if (vertex < edges.size()) edges.erase(edges.begin() + vertex);
        std::ranges::remove(edges | std::views::join, vertex, &info::target);
    }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto remove_edge(std::size_t source_vertex, std::size_t target_vertex) -> void try {
        std::ranges::remove(edges.at(source_vertex), target_vertex, &info::target);
        if constexpr (_edge_type == edge_type::bidirectional)
            std::ranges::remove(edges.at(target_vertex), source_vertex, &info::target);
    } catch (std::out_of_range&) { return; }

private:
    using neighbors = std::vector<edge_info<_edge>>;

    std::vector<_vertex>   vertices = {};
    std::vector<neighbors> edges    = {};
};
}   // namespace graph_cake
