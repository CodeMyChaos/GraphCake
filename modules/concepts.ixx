export module graphCake.concepts;
import std;

namespace graphCake::concepts {
    export template<typename T>
    concept VertexCompliant = std::default_initializable<T> && std::movable<T>;

    export template<typename T>
    concept EdgeCompliant = std::default_initializable<T> and std::convertible_to<T, double> && std::movable<T>;
}

