export module graph_cake.types;

import std;
import graph_cake.concepts;

namespace graph_cake {
export template<class T>
struct edge_info {
    std::size_t source = {};
    std::size_t target = {};

    std::optional<std::reference_wrapper<T>> data = {};
};

export enum class edge_type {
    unidirectional,
    bidirectional
};
}   // namespace graph_cake
