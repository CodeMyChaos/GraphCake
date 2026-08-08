export module graph_cake.representations:adjacency_list;

import graph_cake.concepts;
import graph_cake.types;
import std;
#include <ranges>

namespace graph_cake {
export template<vertex_like _vertex, edge_like _edge>
class adjacency_list {
public:
    template<class Self>
    using vertex =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const _vertex, _vertex>;

    template<class Self>
    using edge =
        std::conditional_t<std::is_const_v<std::remove_reference_t<Self>>, const _edge, _edge>;

    template<class Self>
    using info = edge_info<edge<Self>&>;

    auto& add_vertex() noexcept {
        edges.emplace_back();
        return vertices.emplace_back();
    }

    auto& add_vertex(_vertex&& vertex) noexcept {
        edges.emplace_back();
        return vertices.emplace_back(std::forward<_vertex>(vertex));
    }

    [[nodiscard]] auto vertex_count() noexcept {
        return vertices.size();
    }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto add_edge(this auto&& self, std::size_t source, std::size_t target) noexcept {
        using ret_type = std::pair<info<decltype(self)>, info<decltype(self)>>;

        if (source >= self.edges.size()) return ret_type {};

        if constexpr (_edge_type == edge_type::bidirectional) {
            if (target >= self.edges.size()) return ret_type {};
        }
        auto [first, second] = ret_type {};

        first = self.edges[source].emplace_back(source, target);
        if constexpr (_edge_type == edge_type::bidirectional) {
            if (source == target) return ret_type {first, first};
            second = self.edges.at[target].emplace_back(first);
        }
        return ret_type {first, second};
    }

    template<edge_type _edge_type = edge_type::unidirectional>
    auto add_edge(this auto&& self, std::size_t source, std::size_t target, _edge&& edge) noexcept {
        using ret_type = std::pair<info<decltype(self)>, info<decltype(self)>>;

        if (source >= self.edges.size()) return ret_type {};

        if constexpr (_edge_type == edge_type::bidirectional) {
            if (target >= self.edges.size()) return ret_type {};
        }
        auto [first, second] = ret_type {};

        first = self.edges[source].emplace_back(source, target, std::forward<_edge>(edge));
        if constexpr (_edge_type == edge_type::bidirectional) {
            if (source == target) return ret_type {first, first};
            second = self.edges.at[target].emplace_back(first);
        }
        return ret_type {first, second};
    }

    [[nodiscard]] auto get_edge(this auto&& self, std::size_t source, std::size_t target) noexcept {
        using ret_type = info<decltype(self)>;

        if (source >= self.edges.size()) return {};
        auto edge = std::ranges::find(self.edges[source], target, &edge_info<_edge>::target);

        if (edge == self.edges[source].end()) return {};
        return ret_type {.source = edge->source, .target = edge->target, .data = edge->data};
    }

    [[nodiscard]] auto get_outbound_edges_of(this auto&& self, std::size_t vertex) noexcept {
        using ret_type = info<decltype(self)&>;

        if (vertex >= self.vertices.size()) return std::vector<ret_type> {};

        auto to_info_type = [](auto&& e) {
            return ret_type {.source = e.source, .target = e.target, .data = e.data};
        };
        return self.edges[vertex] | std::views::transform(to_info_type)
               | std::ranges::to<std::vector>();
    }

    [[nodiscard]] auto get_inbound_edges_of(this auto&& self, std::size_t vertex) noexcept {
        using ret_type = std::vector<edge<decltype(self)>&>;

        auto is_vertex_the_target = [&](const auto& target) {
            return target == vertex;
        };
        return ret_type {self.get_all_edges()
                         | std::views::filter(is_vertex_the_target, &info<decltype(self)>::target)};
    }

    [[nodiscard]] auto get_all_vertices(this auto&& self) noexcept {
        using ret_type = std::vector<vertex<decltype(self)&>>;
        return ret_type {self.vertices};
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
} static_assert(graph_like<adjacency_list<int, int>>);
}   // namespace graph_cake
