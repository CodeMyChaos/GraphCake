export module graph_cake.concepts;
import std;

namespace graph_cake {
export enum class edge_type {
    unidirectional,
    bidirectional
};

template<class T>
using vertex = T::template vertex<T>;

template<class T>
using edge = T::template edge<T>;

template<class T>
using info = T::template info<T>;

template<class T>
concept vertex_like = std::movable<std::decay_t<T>>;

template<class T>
concept edge_like = requires {
    requires std::movable<std::decay_t<T>>;
    requires std::convertible_to<std::decay_t<T>, double>;
};

export template<class T>
concept const_graph_interface = requires (const T t, std::size_t n, info<const T> i) {
    requires vertex_like<vertex<const T>>;
    requires edge_like<edge<const T>>;

    requires std::is_aggregate_v<info<const T>>;
    requires std::is_convertible_v<typename decltype(i.data)::value_type, edge<const T>&>;
    requires std::is_const_v<typename decltype(i.data)::value_type::type>;

    { t.vertex_count() };
    { t.get_edge(n, n) };
    { t.get_outbound_edges_of(n) };
    { t.get_inbound_edges_of(n) };
    { t.get_all_vertices() };
    { t.get_all_edges() };
};

export template<class T>
concept mutable_graph_interface =
    requires (T t, vertex<T>&& v, edge<T>&& e, std::size_t n, info<T> i) {
        requires vertex_like<vertex<T>>;
        requires edge_like<edge<T>>;

        requires std::is_aggregate_v<info<T>>;
        requires std::is_convertible_v<typename decltype(i.data)::value_type, edge<const T>&>;
        requires not std::is_const_v<typename decltype(i.data)::value_type::type>;

        { t.add_vertex() };
        { t.add_vertex(std::move(v)) };
        { t.vertex_count() };
        { t.template add_edge<edge_type::unidirectional>(n, n) };
        { t.template add_edge<edge_type::bidirectional>(n, n, std::move(e)) };
        { t.get_edge(n, n) };
        { t.get_outbound_edges_of(n) };
        { t.get_inbound_edges_of(n) };
        { t.get_all_vertices() };
        { t.get_all_edges() };
        { t.remove_vertex(n) };
        { t.template remove_edge<edge_type::unidirectional>(n, n) };
        { t.template remove_edge<edge_type::bidirectional>(n, n) };
    };

export template<class T>
concept graph_like = const_graph_interface<T> and mutable_graph_interface<T>;
}   // namespace graph_cake
