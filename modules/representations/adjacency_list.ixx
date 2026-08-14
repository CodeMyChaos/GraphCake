export module graph_cake.representations:adjacency_list;

import graph_cake.concepts;
import graph_cake.types;
import std;

namespace graph_cake {
export template<class _vertex, class _edge>
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

    auto add_vertex(this auto&& self) noexcept {
        if constexpr (not std::is_const_v<decltype(self)>) {
            self.edges.emplace_back();
            return std::reference_wrapper {self.vertices.emplace_back()};
        }
    }

    auto add_vertex(this auto&& self, _vertex&& vertex) noexcept {
        if constexpr (not std::is_const_v<decltype(self)>) {
            self.edges.emplace_back();
            return self.vertices.emplace_back(std::forward<_vertex>(vertex));
        }
    }

    [[nodiscard]] auto vertex_count(this auto&& self) noexcept {
        return self.vertices.size();
    }

    template<edge_type _type = edge_type::unidirectional>
    auto add_edge(this auto&& self, std::size_t source, std::size_t target) noexcept {
        using S         = decltype(self);
        using edge_pair = std::pair<info<S>, info<S>>;

        if constexpr (std::is_const_v<S>) return;

        if (source >= self.edges.size()) return edge_pair {};
        if constexpr (_type == edge_type::bidirectional) {
            if (target >= self.edges.size()) return edge_pair {};
        }

        auto  ret   = edge_pair {};
        auto& first = self.edges[source].emplace_back(source, target);
        ret.first   = info<S> {first.source, first.target};

        if constexpr (_type == edge_type::bidirectional) {
            if (source != target) {
                auto& second = self.edges[target].emplace_back(first);
                ret.second   = info<S> {second.source, second.target};
            }
            else { ret.second = ret.first; }
        }
        return ret;
    }

    template<edge_type _type = edge_type::unidirectional>
    auto add_edge(this auto&& self, std::size_t source, std::size_t target, _edge&& edge) noexcept {
        using S         = decltype(self);
        using edge_pair = std::pair<info<S>, info<S>>;

        if constexpr (std::is_const_v<S>) return;

        if (source >= self.edges.size()) return edge_pair {};
        if constexpr (_type == edge_type::bidirectional) {
            if (target >= self.edges.size()) return edge_pair {};
        }

        auto  ret   = edge_pair {};
        auto& first = self.edges[source].emplace_back(source, target, std::forward<_edge>(edge));
        if (not first.data) return ret;
        ret.first = info<S> {first.source, first.target, first.data.value()};

        if constexpr (_type == edge_type::bidirectional) {
            if (source != target) {
                auto& second = self.edges[target].emplace_back(first);
                if (not second.data) return ret;
                ret.second = info<S> {second.source, second.target, second.data.value()};
            }
            else { ret.second = ret.first; }
        }
        return ret;
    }

    [[nodiscard]] auto get_edge(this auto&& self, std::size_t source, std::size_t target) noexcept {
        using S = decltype(self);

        if (source >= self.edges.size()) return info<S> {};
        auto edge = std::ranges::find(self.edges[source], target, &edge_info<_edge>::target);

        if (edge == self.edges[source].end()) return info<S> {};
        if (not edge->data) return info<S> {};
        return info<S> {.source = edge->source, .target = edge->target, .data = edge->data.value()};
    }

    [[nodiscard]] auto get_outbound_edges_of(this auto&& self, std::size_t vertex) noexcept {
        using S = decltype(self);

        if (vertex >= self.vertices.size()) return std::vector<info<S>> {};
        return std::vector<info<S>> {self.edges[vertex].begin(), self.edges[vertex].end()};
    }

    [[nodiscard]] auto get_inbound_edges_of(this auto&& self, std::size_t vertex) noexcept {
        using S = decltype(self);

        auto is_vertex_the_target = [&](const auto& edge) {
            if (not edge.data) return false;
            return *edge.data == vertex;
        };
        auto filtered = self.get_all_edges() | std::views::filter(is_vertex_the_target);
        return std::vector<info<S>> {filtered.begin(), filtered.end()};
    }

    [[nodiscard]] auto get_all_vertices(this auto&& self) noexcept {
        using vertex_ref = std::reference_wrapper<vertex<decltype(self)>>;
        return std::vector<vertex_ref> {self.vertices.begin(), self.vertices.end()};
    }

    [[nodiscard]] auto get_all_edges(this auto&& self) noexcept {
        auto ret       = std::vector<info<decltype(self)>> {};
        auto all_edges = self.edges | std::views::join;

        return std::vector<info<decltype(self)>> {all_edges.begin(), all_edges.end()};
    }

    auto remove_vertex(this auto&& self, std::size_t vertex) {
        using S = decltype(self);
        if constexpr (std::is_const_v<S>) return;

        if (vertex < self.vertices.size()) self.vertices.erase(self.vertices.begin() + vertex);
        if (vertex < self.edges.size()) self.edges.erase(self.edges.begin() + vertex);

        // REMAKE THE IMPLEMENTATION, THIS ONE DOES NOT WORK

        auto all_edges = self.edges | std::views::join;

        all_edges.erase(std::ranges::remove(all_edges.begin(), all_edges.end(), vertex,
                                            &edge_info<_edge>::target));
    }

    template<edge_type _type = edge_type::unidirectional>
    auto remove_edge(this auto&& self, std::size_t source, std::size_t target) {
        using S = decltype(self);
        if constexpr (std::is_const_v<S>) return;

        // REMAKE THE IMPLEMENTATION, THIS ONE DOES NOT WORK

        if (source < self.edges.size())
            (void)std::ranges::remove(self.edges[source], target, &edge_info<_edge>::target);
        if constexpr (_type == edge_type::bidirectional)
            if (target < self.edges.size())
                (void)std::ranges::remove(self.edges[target], &edge_info<_edge>::source);
    }

private:
    using neighbors = std::vector<edge_info<_edge>>;

    std::vector<_vertex>   vertices = {};
    std::vector<neighbors> edges    = {};
};

static_assert(graph_like<adjacency_list<int, int>>);
}   // namespace graph_cake
