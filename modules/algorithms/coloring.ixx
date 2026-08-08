export module graph_cake.algorithms:coloring;

import graph_cake.representations;
import graph_cake.types;
import std;
import graph_cake.concepts;

namespace graph_cake {
namespace {
auto greedy_coloring(const std::vector<std::size_t>&       permutation,
                     const std::vector<edge_info<double>>& edges) -> std::vector<std::size_t> {
    auto coloring = std::vector {permutation.size(), std::numeric_limits<std::size_t>::max()};

    coloring[permutation.front()] = 0u;
    for (auto vertex : permutation | std::views::drop(1)) {
        auto vertex_edges = edges | std::views::filter([&](const auto& e) {
                                return (e.source == vertex || e.destination == vertex);
                            }, &);
    }

    return coloring;
}
}   // namespace

template<graph_like G>
export auto greedy_coloring(const graph& graph) -> std::vector<std::size_t> {
    return greedy_coloring(graph.all_vertices(), graph.all_edges());
}
}   // namespace graph_cake
