export module graphCake.graph;
import graphCake.concepts;
import std;

namespace graphCake::graph {
    export template<concepts::EdgeCompliant Edge>
    struct EdgeInfo {
        Edge& data;
        std::size_t source{};
        std::size_t target{};
    };

    export enum class EdgeType {
        Unidirectional,
        Bidirectional
    };
}


