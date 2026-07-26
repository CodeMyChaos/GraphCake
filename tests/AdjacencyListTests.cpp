#include <gtest/gtest.h>

using namespace testing;

import graph_cake.representations;
import graph_cake.graph;
import graph_cake.types;

namespace graph_cake {
namespace {
struct adjacency_list_tests : public Test {
    adjacency_list_tests() {
        constructed = 0;
        destroyed   = 0;
        moved       = 0;
        copied      = 0;
    }

    struct spy {
        spy() {
            (*constructed)++;
        }

        spy(const spy&) {
            (*copied)++;
        }

        spy(spy&& other) noexcept {
            (*moved)++;
            other.destroyed   = nullptr;
            other.copied      = nullptr;
            other.moved       = nullptr;
            other.constructed = nullptr;
        }

        ~spy() {
            if (destroyed) (*destroyed)++;
        }

        spy& operator=(const spy& other) {
            if (this == &other) return *this;
            (*copied)++;
            return *this;
        }

        spy& operator=(spy&& other) noexcept {
            (*moved)++;
            other.destroyed   = nullptr;
            other.copied      = nullptr;
            other.moved       = nullptr;
            other.constructed = nullptr;
            return *this;
        }

        operator double() const {
            return 1.0;
        }

        int* constructed = &(adjacency_list_tests::constructed);
        int* destroyed   = &(adjacency_list_tests::destroyed);
        int* moved       = &(adjacency_list_tests::moved);
        int* copied      = &(adjacency_list_tests::copied);
    };

    static int constructed;
    static int destroyed;
    static int moved;
    static int copied;
};

int adjacency_list_tests::constructed = 0;
int adjacency_list_tests::destroyed   = 0;
int adjacency_list_tests::moved       = 0;
int adjacency_list_tests::copied      = 0;

TEST_F(adjacency_list_tests, default_graph_doesnt_create_or_destroy_vertices) {
    { auto sut = adjacency_list<spy, double> {}; }
    EXPECT_EQ(constructed, 0);
    EXPECT_EQ(destroyed, 0);
}

TEST_F(adjacency_list_tests, graph_correctly_manage_vertex_lifetime) {
    {
        auto sut = adjacency_list<spy, double> {};
        sut.add_vertex();
        sut.add_vertex();
    }
    EXPECT_EQ(constructed, 2);
    EXPECT_GE(moved, 1);   // account for reallocation
    EXPECT_EQ(copied, 0);
    EXPECT_EQ(destroyed, 2);
}

TEST_F(adjacency_list_tests, graph_correctly_manage_unidirection_edge_lifetime) {
    {
        auto sut = adjacency_list<int, spy> {};
        sut.add_vertex();
        sut.add_vertex();
        (void)sut.add_edge(0, 1);
    }
    EXPECT_EQ(constructed, 1);
    EXPECT_EQ(moved, 0);
    EXPECT_EQ(copied, 0);
    EXPECT_EQ(destroyed, 1);
}

TEST_F(adjacency_list_tests, graph_correctly_manage_bidirectional_edge_lifetime) {
    {
        auto sut = adjacency_list<int, spy> {};
        sut.add_vertex();
        sut.add_vertex();
        (void)sut.add_edge<edge_type::bidirectional>(0, 1);
    }
    EXPECT_EQ(constructed, 1);
    EXPECT_GE(moved, 0);
    EXPECT_EQ(copied, 1);
    EXPECT_EQ(destroyed, 2);
}

TEST_F(adjacency_list_tests, graph_correctly_manage_unidirectional_param_edge_lifetime) {
    {
        auto s   = spy {};
        auto sut = adjacency_list<int, spy> {};
        sut.add_vertex();
        sut.add_vertex();
        (void)sut.add_edge(0, 1, std::move(s));
    }
    EXPECT_EQ(constructed, 1);   // the initial spy object
    EXPECT_EQ(moved, 1);
    EXPECT_EQ(copied, 0);
    EXPECT_EQ(destroyed, 1);
}

TEST_F(adjacency_list_tests, graph_correctly_manages_bidirectional_param_edge_lifetime) {
    {
        auto s   = spy {};
        auto sut = adjacency_list<int, spy> {};
        sut.add_vertex();
        sut.add_vertex();
        (void)sut.add_edge<edge_type::bidirectional>(0, 1, std::move(s));
    }
    EXPECT_EQ(constructed, 1);   // the initial spy object
    EXPECT_EQ(moved, 1);
    EXPECT_EQ(copied, 1);
    EXPECT_EQ(destroyed, 2);
}

TEST_F(adjacency_list_tests, add_bidirectional_loop_dont_create_duplicate) {
    {
        auto sut = adjacency_list<int, spy> {};
        sut.add_vertex();
        sut.add_vertex();
        sut.add_edge<edge_type::bidirectional>(0, 0);
    }
    EXPECT_EQ(constructed, 1);
    EXPECT_EQ(moved, 0);
    EXPECT_EQ(copied, 0);
    EXPECT_EQ(destroyed, 1);
}

TEST_F(adjacency_list_tests, default_graph_doesnt_create_or_destroy_edges) {
    { auto sut = adjacency_list<int, spy> {}; }
    EXPECT_EQ(constructed, 0);
    EXPECT_EQ(destroyed, 0);
}
}   // namespace
}   // namespace graph_cake
