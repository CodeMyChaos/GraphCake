export module graph_cake.types;
import graph_cake.concepts;

import std;

namespace graph_cake {
export template<class T>
struct edge_info {
    using data_t = std::conditional_t<std::is_reference_v<T>,
                                      std::reference_wrapper<std::remove_reference_t<T>>, T>;

    std::size_t source = {};
    std::size_t target = {};

    std::optional<data_t> data = {};
};
}   // namespace graph_cake
