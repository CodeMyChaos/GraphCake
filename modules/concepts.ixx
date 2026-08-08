export module graph_cake.concepts;
import std;

namespace graph_cake {
export template<class T>
concept vertex_like = std::movable<T>;

export template<class T>
concept edge_like = std::movable<T> and std::convertible_to<std::decay_t<T>, double>;

export template<class T>
concept const_graph_interface = requires (const T t, std::size_t n) {
    typename T::vertex;
    typename T::edge;
    typename T::info;
    typename T::const_info;

    requires edge_like<typename T::edge>;
    requires vertex_like<typename T::vertex>;
    requires std::is_aggregate_v<typename T::info>;
    requires std::is_aggregate_v<typename T::const_info>;

    { t.vertex_count() } -> std::same_as<std::size_t>;
    { t.get_edge(n, n) } -> std::same_as<std::optional<typename T::const_info>>;
    { t.get_outbound_edges_of(n) } -> std::same_as<std::vector<typename T::const_info>>;
    { t.get_inbound_edges_of(n) } -> std::same_as<std::vector<typename T::const_info>>;
    { t.get_all_vertices() } -> std::same_as<std::vector<const typename T::vertex&>>;
    { t.get_all_edges() } -> std::same_as<std::vector<typename T::const_info>>;
};

export template<class T>
concept mutable_graph_interface =
    requires (T t, typename T::vertex&& v, typename T::edge&& e, std::size_t n) {
        typename T::vertex;
        typename T::edge;
        typename T::info;

        requires vertex_like<typename T::vertex>;
        requires edge_like<typename T::edge>;
        requires std::is_aggregate_v<typename T::info>;

        { t.add_vertex() } -> std::same_as<typename T::vertex&>;
        { t.add_vertex(v) } -> std::same_as<typename T::vertex&>;
        { t.add_edge(n, n) } -> std::same_as<bool>;
        { t.add_edge(n, n, e) } -> std::same_as<bool>;
        { t.remove_vertex(n) } -> std::void_t;
        { t.remove_edge(n, n) } -> std::void_t;

        { t.get_edge(n, n) } -> std::same_as<std::optional<typename T::info>>;
        { t.get_outbound_edges_of(n) } -> std::same_as<std::vector<typename T::info>>;
        { t.get_inbound_edges_of(n) } -> std::same_as<std::vector<typename T::info>>;
        { t.get_all_vertices() } -> std::same_as<std::vector<typename T::vertex&>>;
        { t.get_all_edges() } -> std::same_as<std::vector<typename T::info>>;
    };

export template<class T>
concept graph_like = const_graph_interface<T> and mutable_graph_interface<T>;
}   // namespace graph_cake
