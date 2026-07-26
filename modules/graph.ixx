export module graph_cake.graph;
import graph_cake.concepts;
import graph_cake.types;
import std;

namespace graph_cake {
namespace {
using info = std::vector<edge_info<double>>;

template<class T>
auto make_all_edges_callback() {
    return [](const std::uint8_t* buffer) -> info {
        if (not buffer) { return {}; }
        const auto* g = reinterpret_cast<const T*>(buffer);
        return static_cast<info>(g->all_edges());
    };
}

template<class T>
auto make_all_vertices_callback() {
    return [](const std::uint8_t* buffer) -> std::vector<std::size_t> {
        if (not buffer) { return {}; }
        const auto* g = reinterpret_cast<const T*>(buffer);
        return std::views::iota(0, g->vertex_count());
    };
}

template<class T>
auto make_inbound_edges_of_callback() {
    return [](const std::uint8_t* buffer, std::size_t vertex) -> info {
        if (not buffer) { return {}; }
        const auto* g = reinterpret_cast<const T*>(buffer);
        return static_cast<info>(g->inbound_edges_of(vertex));
    };
}

template<class T>
auto make_outbound_edges_of_callback() {
    return [](const std::uint8_t* buffer, std::size_t vertex) -> info {
        if (not buffer) { return {}; }
        const auto* g = reinterpret_cast<const T*>(buffer);
        return static_cast<info>(g->outbound_edges_of(vertex));
    };
}

template<class T>
auto make_vertex_count_callback() {
    return [](const std::uint8_t* buffer) -> std::size_t {
        if (not buffer) { return {}; }
        const auto* g = reinterpret_cast<const T*>(buffer);
        return g->vertex_count();
    };
}
}   // namespace

export class graph {
public:
    template<vertex_like _vertex, edge_like _edge, template<class, class> class T>
    explicit graph(T<_vertex, _edge>&& graph)
        requires (graph_like<_vertex, _edge, T, edge_info>)
    {
        using Type = decltype(graph);

        // perfect forwarding
        new(buffer) Type(std::forward<Type>(graph));
        all_edges_callback         = make_all_edges_callback<Type>();
        all_vertices_callback      = make_all_vertices_callback<Type>();
        inbound_edges_of_callback  = make_inbound_edges_of_callback<Type>();
        outbound_edges_of_callback = make_outbound_edges_of_callback<Type>();
        vertex_count_callback      = make_vertex_count_callback<Type>();
    }

    [[nodiscard]] auto all_edges() const -> info {
        return all_edges_callback(buffer);
    }

    [[nodiscard]] auto all_vertices() const -> std::vector<std::size_t> {
        return all_vertices_callback(buffer);
    }

    [[nodiscard]] auto inbound_edges_of(std::size_t vertex) const -> info {
        return inbound_edges_of_callback(buffer, vertex);
    }

    [[nodiscard]] auto outbound_edges_of(std::size_t vertex) const -> info {
        return outbound_edges_of_callback(buffer, vertex);
    }

    [[nodiscard]] auto vertex_count() const -> std::size_t {
        return vertex_count_callback(buffer);
    }

private:
#define FN_PTR(name, ret_type, ...) ret_type(*name) __VA_ARGS__ = {}

    FN_PTR(all_edges_callback, info, (const std::uint8_t*));
    FN_PTR(all_vertices_callback, std::vector<std::size_t>, (const std::uint8_t*));
    FN_PTR(inbound_edges_of_callback, info, (const std::uint8_t*, std::size_t));
    FN_PTR(outbound_edges_of_callback, info, (const std::uint8_t*, std::size_t));
    FN_PTR(vertex_count_callback, std::size_t, (const std::uint8_t*));

#undef FN_PTR
    std::uint8_t buffer[64] = {};
};
}   // namespace graph_cake
