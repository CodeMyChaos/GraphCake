module;

#include <ranges>

export module graph_cake.representations:adjacency_matrix;
import graph_cake.concepts;
import graph_cake.types;
import std;

namespace graph_cake {
export template<vertex_like _vertex, edge_like _edge>
class adjacency_matrix {
public:
    using vertex = _vertex;
    using edge   = _edge;

    using info = edge_info<_edge*>;

    auto add_vertex(_vertex&& vertex = {}) -> _vertex& {
        edges.emplace_back(edges.size());
        for (auto& row : edges) row.emplace_back();
        return vertices.emplace_back(std::forward<_vertex>(vertex));
    }

    auto vertex_count() -> std::size_t {
        return vertices.size();
    }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto add_edge(std::size_t source_vertex, std::size_t target_vertex, _edge&& edge = {})
        -> bool try {
        auto val = edges.at(source_vertex).at(target_vertex).emplace(std::forward<_edge>(edge));
        if constexpr (_edge_type == edge_type::bidirectional)
            edges.at(target_vertex).at(source_vertex).emplace(val);
        return true;
    } catch (std::out_of_range&) { return false; }

    [[nodiscard]] auto get_edge(std::size_t source_vertex, std::size_t target_vertex)
        -> std::optional<info> try {
        return edges.at(source_vertex).at(target_vertex).transform([&](const auto& value) {
            return info {.data = value, .source = source_vertex, .target = target_vertex};
        });
    } catch (std::out_of_range&) { return std::nullopt; }

    [[nodiscard]] auto get_outbound_edges_of(std::size_t vertex) -> std::vector<info> try {
        auto result = std::vector<info> {};
        for (const auto& [index, opt] : edges.at(vertex) | std::views::enumerate)
            if (opt) result.emplace_back(opt.value(), vertex, index);
        return result;
    } catch (std::out_of_range&) { return {}; }

    [[nodiscard]] auto get_inbound_edges_of(std::size_t vertex) -> std::vector<info> try {
        auto result = std::vector<info> {};
        for (const auto& [index, row] : edges | std::views::enumerate)
            if (row.at(vertex)) result.emplace_back(row.at(vertex).value(), index, vertex);
        return result;
    } catch (std::out_of_range&) { return {}; }

    [[nodiscard]] auto get_all_vertices() -> std::vector<_vertex&> {
        return {vertices};
    }

    [[nodiscard]] auto get_all_edges() -> std::vector<info> {
        auto result = std::vector<info> {};
        for (const auto& [source, row] : edges | std::views::enumerate)
            for (const auto& [target, edge] : row | std::views::enumerate)
                if (edge) result.emplace_back(edge.value(), source, target);
        return result;
    }

    auto remove_vertex(std::size_t vertex) -> void {
        if (vertex < vertices.size()) vertices.erase(vertices.begin() + vertex);
        if (vertex < edges.size()) edges.erase(edges.begin() + vertex);
        for (auto& row : edges)
            if (vertex < row.size()) row.erase(row.begin() + vertex);
    }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto remove_edge(std::size_t source_vertex, std::size_t target_vertex) -> void try {
        edges.at(source_vertex).at(target_vertex).reset();
        if constexpr (_edge_type == edge_type::bidirectional)
            edges.at(target_vertex).at(source_vertex).reset();
    } catch (std::out_of_range&) { return; }

private:
    using edge_row = std::vector<std::optional<_edge>>;

    std::vector<_vertex>  vertices = {};
    std::vector<edge_row> edges    = {};
};
}   // namespace graph_cake
