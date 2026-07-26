export module graph_cake.concepts;
import std;

namespace graph_cake {
export template<class T>
concept vertex_like = std::movable<T>;

export template<class T>
concept edge_like = std::convertible_to<std::decay_t<T>, double>;

export template<class _vertex, class _edge, template<class, class> class _graph,
                template<class> class ret>
concept graph_like = requires (_graph<_vertex, _edge> g, ret<_edge&> r, std::size_t n) {
    requires edge_like<decltype(g.edge)>;
    requires edge_like<decltype(r.data)>;
    requires vertex_like<decltype(g.vertex)>;
    requires std::is_same_v<decltype(r.data), _edge&>;

    { g.get_all_edges() } -> std::same_as<std::vector<ret<_edge&>>>;
    { g.get_inbound_edges_of(n) } -> std::same_as<std::vector<ret<_edge&>>>;
    { g.get_outbound_edges_of(n) } -> std::same_as<std::vector<ret<_edge&>>>;
    { g.vertex_count() } -> std::same_as<std::size_t>;
};
}   // namespace graph_cake
